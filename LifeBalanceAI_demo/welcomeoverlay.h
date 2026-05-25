#ifndef WELCOMEOVERLAY_H
#define WELCOMEOVERLAY_H

#include <QPixmap>
#include <QWidget>

class QGraphicsOpacityEffect;

class WelcomeOverlay : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal overlayOpacity READ overlayOpacity WRITE setOverlayOpacity)

public:
    explicit WelcomeOverlay(QWidget *parent = nullptr);

    void showWelcome(const QString &message, int holdMs = 1500);
    qreal overlayOpacity() const { return m_overlayOpacity; }
    void setOverlayOpacity(qreal opacity);

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QString m_message;
    QPixmap m_background;
    qreal m_overlayOpacity = 1.0;
};

#endif // WELCOMEOVERLAY_H
