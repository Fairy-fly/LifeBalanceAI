#ifndef LOADINGOVERLAY_H
#define LOADINGOVERLAY_H

#include <QWidget>

class QLabel;
class QTimer;

class LoadingOverlay : public QWidget
{
    Q_OBJECT

public:
    explicit LoadingOverlay(QWidget *parent = nullptr);

    void showLoading(const QString &message);
    void hideLoading();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateDots();

    QLabel *m_messageLabel = nullptr;
    QList<QLabel *> m_dots;
    QTimer *m_timer = nullptr;
    int m_frame = 0;
};

#endif // LOADINGOVERLAY_H
