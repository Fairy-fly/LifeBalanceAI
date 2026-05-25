#ifndef DEEPANALYSISDIALOG_H
#define DEEPANALYSISDIALOG_H

#include <QDialog>
#include "models/dto.h"

class QLabel;
class QScrollArea;

class DeepAnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeepAnalysisDialog(QWidget *parent = nullptr);

    void setAnalysisResult(const LifeBalanceAI::Models::DeepAnalysisResult &result);

private:
    void setupUi();
    QLabel *addSection(const QString &title, const QString &content);

    QScrollArea *m_scrollArea;
    QWidget     *m_contentWidget;
};

#endif // DEEPANALYSISDIALOG_H
