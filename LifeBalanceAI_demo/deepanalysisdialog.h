#ifndef DEEPANALYSISDIALOG_H
#define DEEPANALYSISDIALOG_H

#include <QDialog>
#include "models/dto.h"

class QLabel;
class QFrame;
class QPaintEvent;
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
    void syncShadowLayers();
    void paintEvent(QPaintEvent *event) override;

    QFrame      *m_panel = nullptr;
    QFrame      *m_shadowNear = nullptr;
    QFrame      *m_shadowFar = nullptr;
    QScrollArea *m_scrollArea;
    QWidget     *m_contentWidget;
};

#endif // DEEPANALYSISDIALOG_H
