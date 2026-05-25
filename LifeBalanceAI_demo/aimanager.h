#ifndef AIMANAGER_H
#define AIMANAGER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>

class AIManager : public QObject
{
    Q_OBJECT

public:
    static AIManager &instance();

    /** Full plan generation (initial creation) */
    void generatePlan(const QString &userProfile);

    /**
     * Partial plan update (used for feedback/adjust).
     * @param userProfile       The user's profile string
     * @param currentSlot       The time slot being adjusted (breakfast/lunch/dinner/sports)
     * @param userRequest       The user's adjustment request text
     * @param existingPlanJson  The current plan as a JSON string (for AI context)
     */
    void partialUpdatePlan(const QString &userProfile,
                           const QString &currentSlot,
                           const QString &userRequest,
                           const QString &existingPlanJson,
                           int dayOffset = 0);

    /** Generic AI request: send system+user prompts, result via named signal. */
    void sendRequest(const QString &systemPrompt,
                     const QString &userPrompt,
                     const QString &responseSignal);

    /**
     * Generate an image via OpenAI DALL-E API.
     * @param prompt     Image description prompt
     * @param outputPath Local path to save the generated image
     * Emits imageGenerated(path) on success, imageGenerated("") on failure.
     */
    void generateImage(const QString &prompt, const QString &outputPath);

    // Delete copy/move constructors and assignment operators
    AIManager(const AIManager &) = delete;
    AIManager &operator=(const AIManager &) = delete;
    AIManager(AIManager &&) = delete;
    AIManager &operator=(AIManager &&) = delete;

signals:
    void planGenerated(const QString &jsonResult);
    void partialUpdateGenerated(const QString &jsonResult);
    void analysisGenerated(const QString &jsonResult);
    void reportGenerated(const QString &jsonResult);
    void imageGenerated(const QString &imagePath);
    void requestError(const QString &errorMessage);

private:
    explicit AIManager(QObject *parent = nullptr);
    ~AIManager() override;

    /** Shared: sends a single chat completion request and emits the given signal */
    void sendChatRequest(const QJsonObject &body,
                         const QString &signalName);

    QNetworkAccessManager *m_networkManager;
};

#endif // AIMANAGER_H
