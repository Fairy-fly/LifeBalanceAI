#ifndef DEEPANALYSISSERVICE_H
#define DEEPANALYSISSERVICE_H

#include <QObject>
#include "models/dto.h"

class AIManager;

namespace LifeBalanceAI {
namespace Services {

class DeepAnalysisService : public QObject
{
    Q_OBJECT

public:
    explicit DeepAnalysisService(QObject *parent = nullptr);

    void requestAnalysis(int userId);
    Models::DeepAnalysisResult getLatestAnalysis(int userId);
    bool hasAnalysisThisWeek(int userId);

signals:
    void analysisReady(int userId, const Models::DeepAnalysisResult &result);
    void analysisError(int userId, const QString &error);

private slots:
    void onAnalysisResponse(const QString &jsonResult);

private:
    QString buildAnalysisPrompt(int userId);

    int m_pendingUserId = -1;
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // DEEPANALYSISSERVICE_H
