#ifndef REPORTSERVICE_H
#define REPORTSERVICE_H

#include <QObject>
#include <QVector>
#include <QJsonObject>
#include "models/dto.h"

class AIManager;

namespace LifeBalanceAI {
namespace Services {

class ReportService : public QObject
{
    Q_OBJECT

public:
    explicit ReportService(QObject *parent = nullptr);

    bool canGenerateReport(int userId, bool adminOverride = false);
    void generateReport(int userId, const QString &reportType = QStringLiteral("weekly"));
    void exportReport(int reportId, const QString &format);
    QVector<Models::ReportData> getReportHistory(int userId);

signals:
    void reportGenerated(int userId, const Models::ReportData &report);
    void exportCompleted(int reportId, const QString &path);
    void reportError(int userId, const QString &error);

private slots:
    void onReportAiResponse(const QString &jsonResult);
    void onImageGenerated(const QString &imagePath);

private:
    QString buildReportPrompt(int userId, const QString &reportType);

    int m_pendingUserId = -1;
    QString m_pendingType;
    int m_pendingExportReportId = -1;
    QString m_pendingExportJson;
};

} // namespace Services
} // namespace LifeBalanceAI

#endif // REPORTSERVICE_H
