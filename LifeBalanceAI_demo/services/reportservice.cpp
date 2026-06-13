#include "reportservice.h"
#include "aimanager.h"
#include "databasemanager.h"
#include "services/airesponseparser.h"

#include <QCoreApplication>
#include <QDate>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QStandardPaths>
#include <QStringList>
#include <QImage>
#include <QPainter>
#include <QFont>
#include <QFontMetrics>
#ifdef Q_OS_ANDROID
#include <QJniObject>
#endif

namespace LifeBalanceAI {
namespace Services {

ReportService::ReportService(QObject *parent)
    : QObject(parent)
{
    connect(&AIManager::instance(), &AIManager::reportGenerated,
            this, &ReportService::onReportAiResponse);
    connect(&AIManager::instance(), &AIManager::imageGenerated,
            this, &ReportService::onImageGenerated);
}

bool ReportService::canGenerateReport(int userId, bool adminOverride)
{
    auto &db = DatabaseManager::instance();
    Models::UserInfo info = db.getUserInfo(userId);
    if (userId <= 0 || info.uid <= 0)
        return false;

    if (db.getLatestPlanId(userId) < 0)
        return false;

    const QString todayStr = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
    const auto history = getReportHistory(userId);
    for (const auto &r : history) {
        if (r.type != QStringLiteral("deep_analysis") && r.createdAt.startsWith(todayStr))
            return false;
    }

    if (adminOverride)
        return true;

    if (info.role == QStringLiteral("Ascendant"))
        return true;

    return info.streakDays >= 30;
}

void ReportService::generateReport(int userId, const QString &reportType)
{
    m_pendingUserId = userId;
    m_pendingType = reportType;

    const QString systemPrompt = QStringLiteral(
        "你是一位专业的健康报告分析师。请根据用户画像、打卡数据和执行情况，"
        "生成一份简洁、温暖、可行动的健康周报摘要。必须严格输出 JSON 格式，"
        "不得包含 Markdown 标记。\n\n"
        "输出格式:\n"
        "{\n"
        "  \"summary\": \"总体评价(2-3句话)\",\n"
        "  \"diet_score\": 饮食评分(1-10的整数),\n"
        "  \"exercise_score\": 运动评分(1-10的整数),\n"
        "  \"completion_rate\": 完成率百分比数字,\n"
        "  \"highlights\": [\"亮点1\", \"亮点2\"],\n"
        "  \"suggestions\": [\"建议1\", \"建议2\"],\n"
        "  \"next_week_goal\": \"下周目标建议(一句话)\"\n"
        "}\n\n"
        "注意: diet_score 和 exercise_score 必须是 1-10 的整数；"
        "completion_rate 是百分比数字，例如 85。");

    const QString userPrompt = buildReportPrompt(userId, reportType);
    if (userPrompt.isEmpty()) {
        emit reportError(userId, QStringLiteral("无法获取用户数据"));
        return;
    }

    AIManager::instance().sendRequest(systemPrompt, userPrompt, QStringLiteral("reportGenerated"));
}

void ReportService::exportReport(int reportId, const QString &format)
{
    Q_UNUSED(format);

    auto &db = DatabaseManager::instance();
    Models::ReportData report = db.getReportById(reportId);
    if (report.rid <= 0 || report.aiSummary.isEmpty()) {
        emit reportError(-1, QStringLiteral("报告未找到"));
        return;
    }

    QJsonObject data;
    data[QStringLiteral("report_id")] = report.rid;
    data[QStringLiteral("user_id")] = report.uid;
    data[QStringLiteral("type")] = report.type;
    data[QStringLiteral("start_date")] = report.startDate;
    data[QStringLiteral("end_date")] = report.endDate;
    data[QStringLiteral("ai_summary")] = report.aiSummary;
    data[QStringLiteral("created_at")] = report.createdAt;

    Models::UserInfo info = db.getUserInfo(report.uid);
    Models::ProfileData profile = db.getProfile(report.uid);
    data[QStringLiteral("phone")] = info.phone;
    data[QStringLiteral("nickname")] = profile.nickname;
    data[QStringLiteral("age")] = profile.age;
    data[QStringLiteral("height")] = profile.height;
    data[QStringLiteral("weight")] = profile.weight;
    data[QStringLiteral("gender")] = profile.gender;

    m_pendingExportReportId = reportId;
    m_pendingExportJson = QString::fromUtf8(QJsonDocument(data).toJson(QJsonDocument::Compact));

    // OpenAI image generation is intentionally bypassed for deterministic local PNG export.
    onImageGenerated(QString());
}

#ifdef Q_OS_ANDROID
static QImage renderReportToImage(const QJsonObject &data)
{
    const int imgW = 600;
    const int pad = 24;
    const int contentW = imgW - pad * 2;

    const QString nickname = data[QStringLiteral("nickname")].toString();
    const QString startDate = data[QStringLiteral("start_date")].toString();
    const QString endDate = data[QStringLiteral("end_date")].toString();
    const QString summary = data[QStringLiteral("ai_summary")].toString();
    const int age = data[QStringLiteral("age")].toInt();
    const double height = data[QStringLiteral("height")].toDouble();
    const double weight = data[QStringLiteral("weight")].toDouble();
    const QString gender = data[QStringLiteral("gender")].toString();

    // Calculate height needed for summary text
    QFont bodyFont(QStringLiteral("MiSans"), 11);
    QFontMetrics fm(bodyFont);
    QRect textRect = fm.boundingRect(QRect(0, 0, contentW, 10000),
                                     Qt::TextWordWrap, summary);
    const int summaryH = textRect.height() + 24;

    const int imgH = 180 + summaryH + 120; // header + summary + scores/footer
    QImage image(imgW, imgH, QImage::Format_ARGB32);
    image.fill(Qt::white);

    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing);

    // Title
    QFont titleFont(QStringLiteral("MiSans Medium"), 18);
    titleFont.setBold(true);
    p.setFont(titleFont);
    p.setPen(QColor(0x1A, 0x1A, 0x1A));
    p.drawText(QRect(pad, pad, contentW, 30), Qt::AlignLeft | Qt::AlignVCenter,
               QString::fromUtf8("健康周报")); // 健康周报

    // Divider
    p.setPen(QColor(0x4C, 0xAF, 0x7F));
    p.drawLine(pad, pad + 38, imgW - pad, pad + 38);

    // User info
    QFont infoFont(QStringLiteral("MiSans"), 11);
    p.setFont(infoFont);
    p.setPen(QColor(0x66, 0x66, 0x66));
    QString infoLine = QString::fromUtf8("昵称: ") + nickname
                     + QString::fromUtf8("  |  年龄: ") + QString::number(age)
                     + QString::fromUtf8("  |  身高: ") + QString::number(height, 'f', 1) + QStringLiteral("cm")
                     + QString::fromUtf8("  |  体重: ") + QString::number(weight, 'f', 1) + QStringLiteral("kg");
    if (!gender.isEmpty())
        infoLine += QString::fromUtf8("  |  ") + gender;
    p.drawText(QRect(pad, pad + 46, contentW, 22), Qt::AlignLeft | Qt::AlignVCenter, infoLine);

    // Date range
    if (!startDate.isEmpty() && !endDate.isEmpty()) {
        p.drawText(QRect(pad, pad + 70, contentW, 22), Qt::AlignLeft | Qt::AlignVCenter,
                   startDate + QStringLiteral(" ~ ") + endDate);
    }

    // Summary
    int y = pad + 102;
    p.setFont(bodyFont);
    p.setPen(QColor(0x33, 0x33, 0x33));
    p.drawText(QRect(pad, y, contentW, summaryH), Qt::TextWordWrap, summary);
    y += summaryH + 16;

    // Divider
    p.setPen(QColor(0xE8, 0xE8, 0xE8));
    p.drawLine(pad, y, imgW - pad, y);
    y += 20;

    // Footer
    QFont footerFont(QStringLiteral("MiSans"), 10);
    p.setFont(footerFont);
    p.setPen(QColor(0x99, 0x99, 0x99));
    p.drawText(QRect(pad, y, contentW, 20), Qt::AlignCenter,
               QString::fromUtf8("由 LifeBalance AI 生成"));

    p.end();
    return image;
}

static bool saveToAndroidPictures(const QString &sourcePath, const QString &fileName)
{
    // Save to public Downloads directory (visible in gallery after media scan)
    const QString downloadsDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (downloadsDir.isEmpty())
        return false;

    QDir().mkpath(downloadsDir);
    const QString destPath = downloadsDir + QStringLiteral("/") + fileName;
    if (!QFile::copy(sourcePath, destPath))
        return false;

    // Trigger media scanner so the file appears in gallery
    QJniObject fileObj = QJniObject::callStaticObjectMethod(
        "java/io/File", "<init>", "(Ljava/lang/String;)V",
        QJniObject::fromString(destPath).object<jstring>());
    if (!fileObj.isValid())
        return true; // file copied, just not scanned

    QJniObject fileUri = QJniObject::callStaticObjectMethod(
        "android/net/Uri", "fromFile",
        "(Ljava/io/File;)Landroid/net/Uri;",
        fileObj.object<jobject>());

    QJniObject intent("android/content/Intent",
        "(Ljava/lang/String;)V",
        QJniObject::fromString("android.intent.action.MEDIA_SCANNER_SCAN_FILE").object<jstring>());
    intent.callObjectMethod("setData",
        "(Landroid/net/Uri;)Landroid/content/Intent;",
        fileUri.object<jobject>());

    jobject ctx = QNativeInterface::QAndroidApplication::context();
    QJniObject context(ctx);
    context.callMethod<void>("sendBroadcast",
        "(Landroid/content/Intent;)V",
        intent.object<jobject>());

    return true;
}
#endif

void ReportService::onImageGenerated(const QString &imagePath)
{
    const int reportId = m_pendingExportReportId;
    m_pendingExportReportId = -1;

    if (!imagePath.isEmpty()) {
        emit exportCompleted(reportId, imagePath);
        return;
    }

#ifdef Q_OS_ANDROID
    // Use QPainter-based rendering on Android (no Python runtime)
    const QString cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    QDir().mkpath(cacheDir);
    const QString outputPath = cacheDir + QStringLiteral("/report_%1.png").arg(reportId);

    QJsonDocument doc = QJsonDocument::fromJson(m_pendingExportJson.toUtf8());
    m_pendingExportJson.clear();
    if (doc.isObject()) {
        QImage image = renderReportToImage(doc.object());
        if (!image.isNull() && image.save(outputPath)) {
            if (saveToAndroidPictures(outputPath, QStringLiteral("LifeBalance_report_%1.png").arg(reportId))) {
                const QString downloadsDir = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
                emit exportCompleted(reportId, downloadsDir + QStringLiteral("/LifeBalance_report_%1.png").arg(reportId));
            } else {
                emit reportError(-1, QStringLiteral("导出失败: 无法保存到相册"));
            }
        } else {
            emit reportError(-1, QStringLiteral("导出失败: 无法生成报告图片"));
        }
    } else {
        emit reportError(-1, QStringLiteral("导出失败: 报告数据无效"));
    }
    return;
#else
    // Desktop: use Python script for high-quality rendering

    const QString appDir = QCoreApplication::applicationDirPath();
    const QString outputPath = appDir + QStringLiteral("/report_%1.png").arg(reportId);

    QString scriptPath;
    const QStringList scriptCandidates = {
        appDir + QStringLiteral("/resources/export_report.py"),
        appDir + QStringLiteral("/../resources/export_report.py"),
        appDir + QStringLiteral("/../../resources/export_report.py"),
        appDir + QStringLiteral("/../../../resources/export_report.py"),
        QDir::current().absoluteFilePath(QStringLiteral("resources/export_report.py")),
        QDir::current().absoluteFilePath(QStringLiteral("LifeBalanceAI_demo/resources/export_report.py"))
    };
    for (const QString &candidate : scriptCandidates) {
        const QFileInfo candidateInfo(QDir::cleanPath(candidate));
        if (candidateInfo.exists() && candidateInfo.isFile()) {
            scriptPath = candidateInfo.absoluteFilePath();
            break;
        }
    }
    if (scriptPath.isEmpty()) {
        emit reportError(-1, QStringLiteral("导出失败: 未找到 export_report.py 脚本"));
        return;
    }

    auto resolveExecutable = [](const QString &candidate) -> QString {
        const QString trimmed = candidate.trimmed();
        if (trimmed.isEmpty())
            return QString();

        const QFileInfo info(trimmed);
        if (info.exists() && info.isFile())
            return info.absoluteFilePath();

        return QStandardPaths::findExecutable(trimmed);
    };

    QString pythonExe = resolveExecutable(QString::fromLocal8Bit(qgetenv("PYTHON")));
    if (pythonExe.isEmpty()) {
        const QStringList pythonCandidates = {
            appDir + QStringLiteral("/python.exe"),
            appDir + QStringLiteral("/python/python.exe"),
            appDir + QStringLiteral("/runtime/python/python.exe"),
            QStringLiteral("python"),
            QStringLiteral("python3")
        };
        for (const QString &candidate : pythonCandidates) {
            pythonExe = resolveExecutable(candidate);
            if (!pythonExe.isEmpty())
                break;
        }
    }
    if (pythonExe.isEmpty()) {
        emit reportError(-1, QStringLiteral("导出失败: 未找到 Python 运行时"));
        return;
    }

    const QString jsonFile = outputPath + QStringLiteral(".json");
    {
        QFile jf(jsonFile);
        if (!jf.open(QFile::WriteOnly | QFile::Text)) {
            emit reportError(-1, QStringLiteral("导出失败: 无法写入临时数据文件"));
            return;
        }
        jf.write(m_pendingExportJson.toUtf8());
    }
    m_pendingExportJson.clear();

    QProcess proc;
    proc.start(pythonExe, {scriptPath, jsonFile, outputPath, QStringLiteral("png")});
    if (!proc.waitForStarted(5000)) {
        QFile::remove(jsonFile);
        emit reportError(-1, QStringLiteral("导出失败: 无法启动 Python: ") + proc.errorString());
        return;
    }

    if (!proc.waitForFinished(30000)) {
        proc.kill();
        proc.waitForFinished(1000);
        QFile::remove(jsonFile);
        emit reportError(-1, QStringLiteral("导出超时: Python 脚本未在 30 秒内完成"));
        return;
    }

    QFile::remove(jsonFile);
    if (proc.exitCode() == 0 && QFileInfo::exists(outputPath)) {
        emit exportCompleted(reportId, outputPath);
        return;
    }

    const QString err = QString::fromUtf8(proc.readAllStandardError()).trimmed();
    emit reportError(-1, QStringLiteral("导出失败: ") + (err.isEmpty() ? proc.errorString() : err));
#endif
}

QVector<Models::ReportData> ReportService::getReportHistory(int userId)
{
    if (userId > 0)
        return DatabaseManager::instance().getReportHistory(userId);

    return {};
}

void ReportService::onReportAiResponse(const QString &jsonResult)
{
    const int userId = m_pendingUserId;
    m_pendingUserId = -1;

    if (jsonResult.isEmpty()) {
        emit reportError(userId, QStringLiteral("AI 服务超时或 API Key 未配置"));
        return;
    }

    const auto parsed = AiResponseParser::parseReportSummary(jsonResult);
    if (!parsed.ok) {
        qCritical() << "ReportService: Failed to parse AI response" << parsed.errorMessage;
        emit reportError(userId, QStringLiteral("AI 返回数据格式异常"));
        return;
    }

    Models::ReportData report;
    report.uid = userId;
    report.type = m_pendingType;
    report.startDate = QDate::currentDate().addDays(-7).toString(QStringLiteral("yyyy-MM-dd"));
    report.endDate = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
    report.aiSummary = QString::fromUtf8(QJsonDocument(parsed.summaryObject).toJson(QJsonDocument::Compact));
    report.createdAt = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));

    if (!DatabaseManager::instance().saveReport(report)) {
        emit reportError(userId, QStringLiteral("报告保存失败"));
        return;
    }

    emit reportGenerated(userId, report);
}

QString ReportService::buildReportPrompt(int userId, const QString &reportType)
{
    auto &db = DatabaseManager::instance();
    const Models::ProfileData profile = db.getProfile(userId);
    const Models::UserInfo userInfo = db.getUserInfo(userId);

    const int planId = db.getLatestPlanId(userId);
    if (planId < 0)
        return QString();

    const QDate today = QDate::currentDate();
    const QDate start = today.addDays(-6);

    QStringList daySummaries;
    int totalSlots = 0;
    int doneSlots = 0;
    for (int d = 0; d < 7; ++d) {
        const QDate date = start.addDays(d);
        const auto items = db.getItemsForDate(planId, date);
        if (items.isEmpty())
            continue;

        int dayDone = 0;
        QStringList dayItemNames;
        for (const auto &item : items) {
            ++totalSlots;
            if (item.isDone) {
                ++doneSlots;
                ++dayDone;
            }
            dayItemNames.append(item.content + (item.isDone ? QStringLiteral("(已完成)") : QStringLiteral("(未完成)")));
        }

        const QString daySummary = date.toString(QStringLiteral("MM-dd"))
            + QStringLiteral(": ") + dayItemNames.join(QStringLiteral(", "))
            + QStringLiteral(" [%1/%2]").arg(QString::number(dayDone), QString::number(items.size()));
        daySummaries.append(daySummary);
    }

    const int completionRate = totalSlots > 0 ? (doneSlots * 100 / totalSlots) : 0;
    const QString periodLabel = reportType == QStringLiteral("monthly") ? QStringLiteral("月度") : QStringLiteral("周度");

    QString prompt = QStringLiteral(
        "=== 用户画像 ===\n"
        "年龄: %1 | 身高: %2 cm | 体重: %3 kg | 性别: %4\n"
        "目标: %5\n"
        "饮食偏好: %6\n"
        "运动偏好: %7\n"
        "连续打卡: %8 天 | 累计打卡: %9 次\n\n"
        "=== 近 7 天执行统计 ===\n"
        "总时段数: %10 | 已完成: %11 | 完成率: %12%%\n\n"
        "请根据以上数据生成一份 %13 健康报告。")
        .arg(QString::number(profile.age),
             QString::number(profile.height, 'f', 1),
             QString::number(profile.weight, 'f', 1),
             profile.gender,
             profile.goal,
             profile.dietPref,
             profile.sportPref,
             QString::number(userInfo.streakDays),
             QString::number(userInfo.totalCheckins),
             QString::number(totalSlots),
             QString::number(doneSlots),
             QString::number(completionRate),
             periodLabel);

    if (!daySummaries.isEmpty()) {
        prompt += QStringLiteral("\n=== 每日详情 ===\n");
        for (const QString &daySummary : daySummaries)
            prompt += daySummary + QStringLiteral("\n");
    }

    return prompt;
}

} // namespace Services
} // namespace LifeBalanceAI
