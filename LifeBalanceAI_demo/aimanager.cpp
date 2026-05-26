#include "aimanager.h"
#include "databasemanager.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QStandardPaths>
#include <QTextStream>
#include <QUrl>

static QString readEnvValue(const QString &filePath, const QString &name)
{
    QFile envFile(filePath);
    if (!envFile.open(QFile::ReadOnly | QFile::Text))
        return QString();

    QTextStream stream(&envFile);
    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (!line.startsWith(name + QLatin1Char('=')))
            continue;

        QString value = line.mid(name.length() + 1).trimmed();
        if ((value.startsWith(QLatin1Char('"')) && value.endsWith(QLatin1Char('"')))
            || (value.startsWith(QLatin1Char('\'')) && value.endsWith(QLatin1Char('\'')))) {
            value = value.mid(1, value.size() - 2);
        }
        return value;
    }

    return QString();
}

static QString loadEnvKey(const QString &name)
{
    QString key = QString::fromLocal8Bit(qgetenv(name.toLocal8Bit().constData())).trimmed();
    if (!key.isEmpty())
        return key;

    QStringList candidates;
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QString appLocalData = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (!appData.isEmpty())
        candidates << QDir(appData).filePath(QStringLiteral(".env"));
    if (!appLocalData.isEmpty() && appLocalData != appData)
        candidates << QDir(appLocalData).filePath(QStringLiteral(".env"));

    candidates << QCoreApplication::applicationDirPath() + QStringLiteral("/.env")
               << QCoreApplication::applicationDirPath() + QStringLiteral("/../../.env")
               << QStringLiteral(":/config/.env");

    for (const QString &path : candidates) {
        key = readEnvValue(path, name);
        if (!key.isEmpty())
            return key;
    }

    return QString();
}

static const QString &getApiKey()
{
    static const QString key = loadEnvKey(QStringLiteral("DEEPSEEK_API_KEY"));
    return key;
}

static const QString &getOpenAiKey()
{
    static const QString key = loadEnvKey(QStringLiteral("OPENAI_API_KEY"));
    return key;
}

static const QString OPENAI_IMAGE_URL = QStringLiteral("https://api.openai.com/v1/images/generations");
static const QString API_URL = QStringLiteral("https://api.deepseek.com/v1/chat/completions");
static const QString MODEL_NAME = QStringLiteral("deepseek-chat");

AIManager &AIManager::instance()
{
    static AIManager inst;
    return inst;
}

AIManager::AIManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

AIManager::~AIManager() = default;

void AIManager::generateImage(const QString &prompt, const QString &outputPath)
{
    const QString &key = getOpenAiKey();
    if (key.isEmpty()) {
        qWarning() << "AIManager::generateImage: OPENAI_API_KEY is not set, will fall back to local render.";
        emit imageGenerated(QString());
        return;
    }

    QJsonObject body;
    body[QStringLiteral("model")] = QStringLiteral("dall-e-2");
    body[QStringLiteral("prompt")] = prompt;
    body[QStringLiteral("n")] = 1;
    body[QStringLiteral("size")] = QStringLiteral("1024x1024");

    QNetworkRequest request{QUrl(OPENAI_IMAGE_URL)};
    request.setTransferTimeout(60000);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QStringLiteral("Bearer %1").arg(key).toUtf8());

    QNetworkReply *reply = m_networkManager->post(request, QJsonDocument(body).toJson(QJsonDocument::Compact));
    connect(reply, &QNetworkReply::finished, this, [this, reply, outputPath]() {
        reply->deleteLater();

        if (reply->error() != QNetworkReply::NoError) {
            qCritical() << "AIManager::generateImage: Network error -" << reply->errorString();
            emit imageGenerated(QString());
            return;
        }

        const QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (!doc.isObject()) {
            qCritical() << "AIManager::generateImage: Failed to parse response.";
            emit imageGenerated(QString());
            return;
        }

        const QJsonArray dataArr = doc.object()[QStringLiteral("data")].toArray();
        const QString imageUrl = dataArr.isEmpty()
            ? QString()
            : dataArr.first().toObject()[QStringLiteral("url")].toString();
        if (imageUrl.isEmpty()) {
            qCritical() << "AIManager::generateImage: No image URL in response.";
            emit imageGenerated(QString());
            return;
        }

        QNetworkReply *imgReply = m_networkManager->get(QNetworkRequest{QUrl(imageUrl)});
        connect(imgReply, &QNetworkReply::finished, this, [this, imgReply, outputPath]() {
            imgReply->deleteLater();
            if (imgReply->error() != QNetworkReply::NoError) {
                qCritical() << "AIManager::generateImage: Failed to download image -" << imgReply->errorString();
                emit imageGenerated(QString());
                return;
            }

            QFile file(outputPath);
            if (!file.open(QFile::WriteOnly)) {
                qCritical() << "AIManager::generateImage: Failed to write file" << outputPath;
                emit imageGenerated(QString());
                return;
            }

            file.write(imgReply->readAll());
            file.close();
            qDebug() << "AIManager::generateImage: Image saved to" << outputPath;
            emit imageGenerated(outputPath);
        });
    });
}

void AIManager::sendRequest(const QString &systemPrompt,
                            const QString &userPrompt,
                            const QString &responseSignal)
{
    if (getApiKey().isEmpty()) {
        qCritical() << "AIManager::sendRequest: DEEPSEEK_API_KEY is not set!";
        if (responseSignal == QStringLiteral("analysisGenerated"))
            emit analysisGenerated(QString());
        else if (responseSignal == QStringLiteral("reportGenerated"))
            emit reportGenerated(QString());
        else if (responseSignal == QStringLiteral("planGenerated"))
            emit planGenerated(QString());
        else if (responseSignal == QStringLiteral("partialUpdateGenerated"))
            emit partialUpdateGenerated(QString());
        emit requestError(tr("AI 服务未配置 API 密钥，请联系管理员。"));
        return;
    }

    QJsonArray messages;
    messages.append(QJsonObject{{QStringLiteral("role"), QStringLiteral("system")},
                                {QStringLiteral("content"), systemPrompt}});
    messages.append(QJsonObject{{QStringLiteral("role"), QStringLiteral("user")},
                                {QStringLiteral("content"), userPrompt}});

    QJsonObject body;
    body[QStringLiteral("model")] = MODEL_NAME;
    body[QStringLiteral("messages")] = messages;

    sendChatRequest(body, responseSignal);
}

void AIManager::generatePlan(const QString &userProfile)
{
    QJsonArray messages;
    messages.append(QJsonObject{
        {QStringLiteral("role"), QStringLiteral("system")},
        {QStringLiteral("content"),
         QStringLiteral("你是一位专业、克制、可靠的健康规划教练。请根据用户画像生成未来 3 天的详细计划（早中晚餐+运动），"
                        "以及周、月、年长期目标。输出必须是合法 JSON，不要包含 Markdown、解释文字或代码块标记。"
                        "字段格式必须严格如下："
                        "{\"long_term\":{\"week\":\"...\",\"month\":\"...\",\"year\":\"...\"},"
                        "\"daily\":["
                        "{\"day\":1,\"breakfast\":\"...\",\"lunch\":\"...\",\"dinner\":\"...\",\"sports\":\"...\"},"
                        "{\"day\":2,\"breakfast\":\"...\",\"lunch\":\"...\",\"dinner\":\"...\",\"sports\":\"...\"},"
                        "{\"day\":3,\"breakfast\":\"...\",\"lunch\":\"...\",\"dinner\":\"...\",\"sports\":\"...\"}"
                        "]}")}
    });
    messages.append(QJsonObject{{QStringLiteral("role"), QStringLiteral("user")},
                                {QStringLiteral("content"), userProfile}});

    QJsonObject body;
    body[QStringLiteral("model")] = MODEL_NAME;
    body[QStringLiteral("messages")] = messages;

    sendChatRequest(body, QStringLiteral("planGenerated"));
}

void AIManager::partialUpdatePlan(const QString &userProfile,
                                  const QString &currentSlot,
                                  const QString &userRequest,
                                  const QString &existingPlanJson,
                                  int dayOffset)
{
    QString slotChinese;
    if (currentSlot == QStringLiteral("breakfast"))
        slotChinese = QStringLiteral("早餐");
    else if (currentSlot == QStringLiteral("lunch"))
        slotChinese = QStringLiteral("午餐");
    else if (currentSlot == QStringLiteral("dinner"))
        slotChinese = QStringLiteral("晚餐");
    else if (currentSlot == QStringLiteral("sports"))
        slotChinese = QStringLiteral("运动");
    else
        slotChinese = currentSlot;

    const QString dayScope = dayOffset == 0
        ? QStringLiteral("今天")
        : (dayOffset == 1 ? QStringLiteral("明天") : QStringLiteral("后天"));
    const int responseDay = dayOffset + 1;

    const QString systemPrompt = QString(
        "你是一位专业的健康规划教练。用户想修改%1【%2】的内容，请把用户输入整理成可执行、自然、简洁的饮食/运动计划。\n"
        "请遵守以下规则：\n"
        "1. 已完成（is_done=1）的项目必须保留，不得修改。\n"
        "2. 已手动调整（is_adjusted=1）的项目必须保留，不得修改。\n"
        "3. 更新【%2】内容，并根据变化合理微调%1其他未完成时段。\n"
        "4. 仅调整%1的内容，不得修改其他日期。\n"
        "5. 禁止修改 long_term 字段。\n"
        "6. 只输出需要改动的 daily 条目的 JSON 数组。\n"
        "7. 所有输出条目的 day 必须是 %3。\n"
        "输出示例：\n"
        "[{\"day\":%3,\"time_slot\":\"lunch\",\"content\":\"...\"},"
        "{\"day\":%3,\"time_slot\":\"dinner\",\"content\":\"...\"}]")
        .arg(dayScope, slotChinese, QString::number(responseDay));

    const QString userPrompt = QString(
        "用户画像：%1\n"
        "调整要求：%2\n"
        "当前计划（JSON）：%3\n"
        "请输出仅包含调整项的 JSON 数组，所有调整项的 day 必须是 %4。")
        .arg(userProfile, userRequest, existingPlanJson, QString::number(responseDay));

    QJsonArray messages;
    messages.append(QJsonObject{{QStringLiteral("role"), QStringLiteral("system")},
                                {QStringLiteral("content"), systemPrompt}});
    messages.append(QJsonObject{{QStringLiteral("role"), QStringLiteral("user")},
                                {QStringLiteral("content"), userPrompt}});

    QJsonObject body;
    body[QStringLiteral("model")] = MODEL_NAME;
    body[QStringLiteral("messages")] = messages;

    sendChatRequest(body, QStringLiteral("partialUpdateGenerated"));
}

void AIManager::sendChatRequest(const QJsonObject &body, const QString &signalName)
{
    auto emitEmptyResponse = [this, signalName]() {
        if (signalName == QStringLiteral("planGenerated"))
            emit planGenerated(QString());
        else if (signalName == QStringLiteral("partialUpdateGenerated"))
            emit partialUpdateGenerated(QString());
        else if (signalName == QStringLiteral("analysisGenerated"))
            emit analysisGenerated(QString());
        else if (signalName == QStringLiteral("reportGenerated"))
            emit reportGenerated(QString());
    };

    const QByteArray jsonPayload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    const QString promptForLog = QString::fromUtf8(jsonPayload);
    auto logAiRequest = [signalName, promptForLog](const QString &response, const QString &error) {
        DatabaseManager::instance().logAiRequest(signalName, promptForLog, response, error);
    };

    if (getApiKey().isEmpty()) {
        qCritical() << "AIManager: DEEPSEEK_API_KEY is not set!";
        logAiRequest(QString(), QStringLiteral("DEEPSEEK_API_KEY is not set"));
        emit requestError(tr("AI 服务未配置 API 密钥，请联系管理员。"));
        emitEmptyResponse();
        return;
    }

    QNetworkRequest request{QUrl(API_URL)};
    request.setTransferTimeout(30000);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QStringLiteral("Bearer %1").arg(getApiKey()).toUtf8());

    QNetworkReply *reply = m_networkManager->post(request, jsonPayload);
    connect(reply, &QNetworkReply::finished, this,
            [this, reply, signalName, emitEmptyResponse, logAiRequest]() {
        reply->deleteLater();

        if (reply->error() == QNetworkReply::OperationCanceledError
            || reply->error() == QNetworkReply::TimeoutError) {
            const QString errorMessage = tr("AI 服务请求超时，请稍后重试。");
            qCritical() << "AIManager: Request timed out.";
            logAiRequest(QString(), errorMessage);
            emit requestError(errorMessage);
            emitEmptyResponse();
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            const QByteArray errorBody = reply->readAll();
            qCritical() << "AIManager: Network error -" << reply->errorString();
            qCritical() << "AIManager: HTTP status code -" << httpStatus;
            qCritical() << "AIManager: Response body -" << errorBody;

            QString errorMessage;
            if (httpStatus == 401)
                errorMessage = tr("API 密钥无效（401），请检查密钥配置。");
            else if (httpStatus == 429)
                errorMessage = tr("API 请求过于频繁（429），请稍后重试。");
            else
                errorMessage = tr("网络请求失败：%1").arg(reply->errorString());

            logAiRequest(QString::fromUtf8(errorBody), errorMessage);
            emit requestError(errorMessage);
            emitEmptyResponse();
            return;
        }

        const QByteArray responseData = reply->readAll();
        qDebug() << "AIManager: Response received successfully.";

        const QJsonDocument doc = QJsonDocument::fromJson(responseData);
        if (!doc.isObject()) {
            const QString errorMessage = tr("AI 返回了无效的数据格式，请稍后重试。");
            qCritical() << "AIManager: Failed to parse response JSON.";
            logAiRequest(QString::fromUtf8(responseData), errorMessage);
            emit requestError(errorMessage);
            emitEmptyResponse();
            return;
        }

        const QJsonArray choices = doc.object()[QStringLiteral("choices")].toArray();
        if (choices.isEmpty()) {
            const QString errorMessage = tr("AI 未返回有效结果，请稍后重试。");
            qCritical() << "AIManager: Response contains no choices.";
            logAiRequest(QString::fromUtf8(responseData), errorMessage);
            emit requestError(errorMessage);
            emitEmptyResponse();
            return;
        }

        const QJsonObject messageObj = choices.first().toObject()[QStringLiteral("message")].toObject();
        const QString content = messageObj[QStringLiteral("content")].toString();
        if (content.isEmpty()) {
            const QString errorMessage = tr("AI 返回了空内容，请稍后重试。");
            qCritical() << "AIManager: Assistant content is empty.";
            logAiRequest(QString::fromUtf8(responseData), errorMessage);
            emit requestError(errorMessage);
            emitEmptyResponse();
            return;
        }

        qDebug() << "AIManager: response content received, emitting" << signalName;
        logAiRequest(content, QString());

        if (signalName == QStringLiteral("planGenerated"))
            emit planGenerated(content);
        else if (signalName == QStringLiteral("partialUpdateGenerated"))
            emit partialUpdateGenerated(content);
        else if (signalName == QStringLiteral("analysisGenerated"))
            emit analysisGenerated(content);
        else if (signalName == QStringLiteral("reportGenerated"))
            emit reportGenerated(content);
    });
}
