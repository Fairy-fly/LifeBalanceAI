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
        QString line = stream.readLine().trimmed();
        if (!line.isEmpty() && line.front() == QChar(0xFEFF))
            line.remove(0, 1);
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

namespace {

struct LoadedEnvKey
{
    QString value;
    AiKeyStatus status;
};

QStringList defaultEnvCandidates()
{
    QStringList candidates;
    const QString appData = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    const QString appLocalData = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (!appData.isEmpty())
        candidates << QDir(appData).filePath(QStringLiteral(".env"));
    if (!appLocalData.isEmpty() && appLocalData != appData)
        candidates << QDir(appLocalData).filePath(QStringLiteral(".env"));

    candidates << QCoreApplication::applicationDirPath() + QStringLiteral("/.env")
               << QCoreApplication::applicationDirPath() + QStringLiteral("/../../.env");

    candidates.removeDuplicates();
    return candidates;
}

LoadedEnvKey loadEnvKey(const QString &name,
                        const QStringList &envFiles,
                        bool includeProcessEnvironment,
                        bool includeDefaultPaths)
{
    LoadedEnvKey loaded;
    loaded.status.keyName = name;

    if (includeProcessEnvironment) {
        loaded.status.checkedPaths << QStringLiteral("environment:%1").arg(name);
        loaded.value = QString::fromLocal8Bit(qgetenv(name.toLocal8Bit().constData())).trimmed();
        if (!loaded.value.isEmpty()) {
            loaded.status.configured = true;
            loaded.status.source = QStringLiteral("environment");
            loaded.status.message = QObject::tr("AI service key is configured through the process environment.");
            return loaded;
        }
    }

    QStringList candidates = envFiles;
    if (includeDefaultPaths)
        candidates << defaultEnvCandidates();
    candidates.removeDuplicates();

    for (const QString &path : candidates) {
        loaded.status.checkedPaths << path;
        loaded.value = readEnvValue(path, name);
        if (!loaded.value.isEmpty()) {
            loaded.status.configured = true;
            loaded.status.source = path;
            loaded.status.message = QObject::tr("AI service key is configured through a local config file.");
            return loaded;
        }
    }

    loaded.status.configured = false;
    loaded.status.source = QStringLiteral("not_configured");
    loaded.status.message = QObject::tr("AI service is not configured. Set DEEPSEEK_API_KEY in a local .env file.");
    return loaded;
}

QString getApiKey()
{
    return loadEnvKey(QStringLiteral("DEEPSEEK_API_KEY"), {}, true, true).value;
}

QString getOpenAiKey()
{
    return loadEnvKey(QStringLiteral("OPENAI_API_KEY"), {}, true, true).value;
}

} // namespace

static const QString API_URL = QStringLiteral("https://api.deepseek.com/v1/chat/completions");
static const QString MODEL_NAME = QStringLiteral("deepseek-chat");
static const QString OPENAI_IMAGE_URL = QStringLiteral("https://api.openai.com/v1/images/generations");
void AIManager::generateNickname(const QString &userProfile)
{
    const QString systemPrompt = QStringLiteral(
        "You are a friendly Chinese nickname writer for a health app. "
        "Create one warm Simplified Chinese nickname, 2 to 8 Chinese characters, "
        "related to food, exercise, or healthy living. Return only the nickname, "
        "with no explanation, punctuation, or line breaks.");

    QJsonArray messages;
    messages.append(QJsonObject{{QStringLiteral("role"), QStringLiteral("system")},
                                {QStringLiteral("content"), systemPrompt}});
    messages.append(QJsonObject{{QStringLiteral("role"), QStringLiteral("user")},
                                {QStringLiteral("content"), userProfile}});

    QJsonObject body;
    body[QStringLiteral("model")] = MODEL_NAME;
    body[QStringLiteral("messages")] = messages;

    sendChatRequest(body, QStringLiteral("nicknameGenerated"));
}

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

AiKeyStatus AIManager::chatApiKeyStatus()
{
    return chatApiKeyStatus({}, true, true);
}

AiKeyStatus AIManager::chatApiKeyStatus(const QStringList &envFiles,
                                        bool includeProcessEnvironment,
                                        bool includeDefaultPaths)
{
    return loadEnvKey(QStringLiteral("DEEPSEEK_API_KEY"),
                      envFiles,
                      includeProcessEnvironment,
                      includeDefaultPaths).status;
}

bool AIManager::hasChatApiKey()
{
    return chatApiKeyStatus().configured;
}

void AIManager::generateImage(const QString &prompt, const QString &outputPath)
{
    const QString key = getOpenAiKey();
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
    const QString apiKey = getApiKey();
    if (apiKey.isEmpty()) {
        qCritical() << "AIManager::sendRequest: DEEPSEEK_API_KEY is not set!";
        if (responseSignal == QStringLiteral("analysisGenerated"))
            emit analysisGenerated(QString());
        else if (responseSignal == QStringLiteral("reportGenerated"))
            emit reportGenerated(QString());
        else if (responseSignal == QStringLiteral("planGenerated"))
            emit planGenerated(QString());
        else if (responseSignal == QStringLiteral("partialUpdateGenerated"))
            emit partialUpdateGenerated(QString());
        emit requestError(QStringLiteral("AI service is not configured. Please set the DeepSeek API key."));
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
         QStringLiteral(
             "You are a careful health planning coach. Generate a detailed 3-day plan "
             "for the user profile. The content values must be written in Simplified Chinese. "
             "Return strict JSON only, without Markdown fences or explanations. "
             "Required schema: "
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
    QString slotLabel;
    if (currentSlot == QStringLiteral("breakfast"))
        slotLabel = QStringLiteral("breakfast");
    else if (currentSlot == QStringLiteral("lunch"))
        slotLabel = QStringLiteral("lunch");
    else if (currentSlot == QStringLiteral("dinner"))
        slotLabel = QStringLiteral("dinner");
    else if (currentSlot == QStringLiteral("sports"))
        slotLabel = QStringLiteral("sports");
    else
        slotLabel = currentSlot;

    const QString dayLabel = dayOffset == 0
        ? QStringLiteral("today")
        : (dayOffset == 1 ? QStringLiteral("tomorrow") : QStringLiteral("the day after tomorrow"));
    const int responseDay = dayOffset + 1;

    const QString systemPrompt = QString(
        "You are a careful health planning coach. The user wants to adjust the %1 item for %2. "
        "Return strict JSON only. Content values must be written in Simplified Chinese. "
        "Rules: preserve completed items with is_done=1; preserve manually adjusted items with is_adjusted=1; "
        "do not modify long_term; only return changed daily items; every returned item must have day=%3. "
        "Output example: [{\"day\":%3,\"time_slot\":\"lunch\",\"content\":\"...\"},"
        "{\"day\":%3,\"time_slot\":\"dinner\",\"content\":\"...\"}]")
        .arg(slotLabel, dayLabel, QString::number(responseDay));

    const QString userPrompt = QString(
        "User profile:\n%1\n"
        "Adjustment request:\n%2\n"
        "Current plan JSON:\n%3\n"
        "Return only a JSON array. All returned items must have day=%4.")
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
        else if (signalName == QStringLiteral("nicknameGenerated"))
            emit nicknameGenerated(QString());
    };

    const QByteArray jsonPayload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    const QString promptForLog = QString::fromUtf8(jsonPayload);
    auto logAiRequest = [signalName, promptForLog](const QString &response, const QString &error) {
        DatabaseManager::instance().logAiRequest(signalName, promptForLog, response, error);
    };

    const QString apiKey = getApiKey();
    if (apiKey.isEmpty()) {
        qCritical() << "AIManager: DEEPSEEK_API_KEY is not set!";
        logAiRequest(QString(), QStringLiteral("DEEPSEEK_API_KEY is not set"));
        emit requestError(QStringLiteral("AI service is not configured. Please set the DeepSeek API key."));
        emitEmptyResponse();
        return;
    }

    QNetworkRequest request{QUrl(API_URL)};
    request.setTransferTimeout(30000);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setRawHeader("Authorization", QStringLiteral("Bearer %1").arg(apiKey).toUtf8());

    QNetworkReply *reply = m_networkManager->post(request, jsonPayload);
    connect(reply, &QNetworkReply::finished, this,
            [this, reply, signalName, emitEmptyResponse, logAiRequest]() {
        reply->deleteLater();

        if (reply->error() == QNetworkReply::OperationCanceledError
            || reply->error() == QNetworkReply::TimeoutError) {
            const QString errorMessage = QStringLiteral("AI request timed out. Please try again later.");
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
                errorMessage = QStringLiteral("API key is invalid (401). Please check the DeepSeek key configuration.");
            else if (httpStatus == 429)
                errorMessage = QStringLiteral("API requests are too frequent (429). Please try again later.");
            else
                errorMessage = QStringLiteral("Network request failed: %1").arg(reply->errorString());

            logAiRequest(QString::fromUtf8(errorBody), errorMessage);
            emit requestError(errorMessage);
            emitEmptyResponse();
            return;
        }

        const QByteArray responseData = reply->readAll();
        qDebug() << "AIManager: Response received successfully.";

        const QJsonDocument doc = QJsonDocument::fromJson(responseData);
        if (!doc.isObject()) {
            const QString errorMessage = QStringLiteral("AI returned an invalid data format. Please try again later.");
            qCritical() << "AIManager: Failed to parse response JSON.";
            logAiRequest(QString::fromUtf8(responseData), errorMessage);
            emit requestError(errorMessage);
            emitEmptyResponse();
            return;
        }

        const QJsonArray choices = doc.object()[QStringLiteral("choices")].toArray();
        if (choices.isEmpty()) {
            const QString errorMessage = QStringLiteral("AI did not return a valid result. Please try again later.");
            qCritical() << "AIManager: Response contains no choices.";
            logAiRequest(QString::fromUtf8(responseData), errorMessage);
            emit requestError(errorMessage);
            emitEmptyResponse();
            return;
        }

        const QJsonObject messageObj = choices.first().toObject()[QStringLiteral("message")].toObject();
        const QString content = messageObj[QStringLiteral("content")].toString();
        if (content.isEmpty()) {
            const QString errorMessage = QStringLiteral("AI returned empty content. Please try again later.");
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
        else if (signalName == QStringLiteral("nicknameGenerated"))
            emit nicknameGenerated(content);
    });
}
