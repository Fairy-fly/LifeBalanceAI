#include "deepanalysisservice.h"
#include "aimanager.h"
#include "databasemanager.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace LifeBalanceAI {
namespace Services {

DeepAnalysisService::DeepAnalysisService(QObject *parent)
    : QObject(parent)
{
    connect(&AIManager::instance(), &AIManager::analysisGenerated,
            this, &DeepAnalysisService::onAnalysisResponse);
}

void DeepAnalysisService::requestAnalysis(int userId)
{
    m_pendingUserId = userId;

    QString systemPrompt = QStringLiteral(
        "你是一位专业的健康与营养分析师。请根据用户画像、近期饮食和运动数据，"
        "生成一份深度分析报告。必须严格输出JSON 格式，不得包含Markdown 代码块标记。\n\n"
        "输出格式:\n"
        "{\n"
        "  \"nutrition_protein\": \"蛋白质摄入分析(一段话)\n"
        "  \"nutrition_carbs\": \"碳水摄入分析(一段话)\n"
        "  \"nutrition_fat\": \"脂肪摄入分析(一段话)\n"
        "  \"nutrition_advice\": \"综合营养建议(一段话)\n"
        "  \"exercise_change\": \"运动调整建议(一段话)\n"
        "  \"exercise_confidence\": \"运动完成信心评估(一段话)\n"
        "  \"recipes\": [{\"name\":\"菜名\",\"reason\":\"推荐原因\",\"calories\":300}],\n"
        "  \"lazy_slot\": \"最常偷懒的时段\",\n"
        "  \"lazy_advice\": \"克服偷懒的建议(一段话)\n"
        "}\n\n"
        "recipes 数组至少包含 5 道推荐食谱。"
    );

    QString userPrompt = buildAnalysisPrompt(userId);
    if (userPrompt.isEmpty()) {
        emit analysisError(userId, QStringLiteral("无法获取用户数据"));
        return;
    }

    AIManager::instance().sendRequest(
        systemPrompt, userPrompt, QStringLiteral("analysisGenerated"));
}

bool DeepAnalysisService::hasAnalysisThisWeek(int userId)
{
    // Check if there is an analysis from the last 7 days in the Reports table
    auto history = DatabaseManager::instance().getReportHistory(userId);
    QDate weekAgo = QDate::currentDate().addDays(-7);
    for (const auto &r : history) {
        if (r.type == QStringLiteral("deep_analysis")) {
            QDate reportDate = QDate::fromString(r.createdAt.left(10), QStringLiteral("yyyy-MM-dd"));
            if (reportDate.isValid() && reportDate >= weekAgo)
                return true;
        }
    }
    return false;
}

Models::DeepAnalysisResult DeepAnalysisService::getLatestAnalysis(int userId)
{
    Models::DeepAnalysisResult result;
    auto history = DatabaseManager::instance().getReportHistory(userId);
    for (const auto &r : history) {
        if (r.type == QStringLiteral("deep_analysis") && !r.aiSummary.isEmpty()) {
            QJsonDocument doc = QJsonDocument::fromJson(r.aiSummary.toUtf8());
            if (doc.isObject()) {
                result = parseAnalysisResponse(doc.object());
                break;
            }
        }
    }
    return result;
}

void DeepAnalysisService::onAnalysisResponse(const QString &jsonResult)
{
    if (m_pendingUserId < 0) return;
    int userId = m_pendingUserId;
    m_pendingUserId = -1;

    if (jsonResult.isEmpty()) {
        emit analysisError(userId, QStringLiteral("AI 服务超时或 API Key 未配置"));
        return;
    }

    QString cleaned = jsonResult;
    cleaned.remove(QStringLiteral("`json"));
    cleaned.remove(QStringLiteral("`"));
    cleaned = cleaned.trimmed();

    QJsonDocument doc = QJsonDocument::fromJson(cleaned.toUtf8());
    if (doc.isNull() || !doc.isObject()) {
        qCritical() << "DeepAnalysisService: Failed to parse AI response";
        emit analysisError(userId, QStringLiteral("AI 返回数据格式异常"));
        return;
    }

    Models::DeepAnalysisResult result = parseAnalysisResponse(doc.object());

    // Save to Reports table as a cache
    Models::ReportData report;
    report.uid       = userId;
    report.type      = QStringLiteral("deep_analysis");
    report.startDate = QDate::currentDate().addDays(-7).toString(QStringLiteral("yyyy-MM-dd"));
    report.endDate   = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
    report.aiSummary = QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    report.createdAt = QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss"));
    DatabaseManager::instance().saveReport(report);

    emit analysisReady(userId, result);
}

Models::DeepAnalysisResult DeepAnalysisService::parseAnalysisResponse(const QJsonObject &obj)
{
    Models::DeepAnalysisResult result;
    result.nutritionProtein  = obj[QStringLiteral("nutrition_protein")].toString();
    result.nutritionCarbs    = obj[QStringLiteral("nutrition_carbs")].toString();
    result.nutritionFat      = obj[QStringLiteral("nutrition_fat")].toString();
    result.nutritionAdvice   = obj[QStringLiteral("nutrition_advice")].toString();
    result.exerciseChange     = obj[QStringLiteral("exercise_change")].toString();
    result.exerciseConfidence = obj[QStringLiteral("exercise_confidence")].toString();
    result.lazySlot           = obj[QStringLiteral("lazy_slot")].toString();
    result.lazyAdvice         = obj[QStringLiteral("lazy_advice")].toString();

    QJsonArray recipes = obj[QStringLiteral("recipes")].toArray();
    for (const auto &r : recipes) {
        QJsonObject ro = r.toObject();
        Models::RecipeItem item;
        item.name     = ro[QStringLiteral("name")].toString();
        item.reason   = ro[QStringLiteral("reason")].toString();
        item.calories = ro[QStringLiteral("calories")].toInt();
        result.recipes.append(item);
    }
    return result;
}

QString DeepAnalysisService::buildAnalysisPrompt(int userId)
{
    auto &db = DatabaseManager::instance();
    Models::ProfileData profile = db.getProfile(userId);
    Models::UserInfo userInfo = db.getUserInfo(userId);
    if (userId <= 0) return QString();

    // Collect recent 7 days of items
    int planId = db.getLatestPlanId(userId);
    if (planId < 0)
        return QString();

    QDate today = QDate::currentDate();
    QDate start = today.addDays(-6);

    QStringList daySummaries;
    for (int d = 0; d < 7; ++d) {
        QDate date = start.addDays(d);
        auto items = db.getItemsForDate(planId, date);
        if (items.isEmpty()) continue;

        QStringList slotItems;
        int doneCount = 0, totalCount = 0;
        for (const auto &item : items) {
            totalCount++;
            if (item.isDone) doneCount++;
            QString slotName;
            if (item.timeSlot == QStringLiteral("breakfast")) slotName = QStringLiteral("早餐");
            else if (item.timeSlot == QStringLiteral("lunch")) slotName = QStringLiteral("午餐");
            else if (item.timeSlot == QStringLiteral("dinner")) slotName = QStringLiteral("晚餐");
            else if (item.timeSlot == QStringLiteral("sports")) slotName = QStringLiteral("运动");
            slotItems << QStringLiteral("%1: %2 [%3]")
                         .arg(slotName, item.content,
                              item.isDone ? QStringLiteral("已完成") : QStringLiteral("未完成"));
        }
        daySummaries << QStringLiteral("%1 (完成%2/%3):\n%4")
                            .arg(date.toString(QStringLiteral("MM-dd")),
                                 QString::number(doneCount),
                                 QString::number(totalCount),
                                 slotItems.join(QStringLiteral("\n")));
    }

    QString prompt = QStringLiteral(
        "=== 用户画像 ===\n"
        "年龄: %1 岁| 身高: %2 cm | 体重: %3 kg | 性别: %4\n"
        "目标: %5 | 过敏源: %6\n"
        "饮食偏好: %7\n"
        "运动偏好: %8\n"
        "连续打卡: %9 天| 累计打卡: %10 次\n\n"
        "=== 7天规划与执行情况 ===\n"
        "%11\n\n"
        "请根据以上数据进行深度分析。"
    ).arg(QString::number(profile.age),
          QString::number(profile.height, 'f', 1),
          QString::number(profile.weight, 'f', 1),
          profile.gender,
          profile.goal,
          profile.allergy,
          profile.dietPref,
          profile.sportPref,
          QString::number(userInfo.streakDays),
          QString::number(userInfo.totalCheckins),
          daySummaries.join(QStringLiteral("\n")));

    return prompt;
}

} // namespace Services
} // namespace LifeBalanceAI
