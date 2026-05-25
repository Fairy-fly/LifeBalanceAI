#include "welcomeoverlay.h"

#include <QPainter>
#include <QPropertyAnimation>
#include <QEasingCurve>
#include <QTimer>

WelcomeOverlay::WelcomeOverlay(QWidget *parent)
    : QWidget(parent),
      m_background(QStringLiteral(":/assets/welcome_success.png"))
{
    setObjectName(QStringLiteral("welcomeOverlay"));
    setAttribute(Qt::WA_DeleteOnClose, true);
    setAttribute(Qt::WA_StyledBackground, false);
}

void WelcomeOverlay::showWelcome(const QString &message, int holdMs)
{
    Q_UNUSED(message)
    m_message.clear();
    m_overlayOpacity = 1.0;
    if (parentWidget())
        setGeometry(parentWidget()->rect());

    show();
    raise();
    update();

    QTimer::singleShot(holdMs, this, [this]() {
        auto *fade = new QPropertyAnimation(this, "overlayOpacity", this);
        fade->setDuration(360);
        fade->setStartValue(1.0);
        fade->setEndValue(0.0);
        fade->setEasingCurve(QEasingCurve::OutCubic);
        connect(fade, &QPropertyAnimation::finished, this, &WelcomeOverlay::close);
        fade->start(QAbstractAnimation::DeleteWhenStopped);
    });
}

void WelcomeOverlay::setOverlayOpacity(qreal opacity)
{
    m_overlayOpacity = qBound(0.0, opacity, 1.0);
    update();
}

void WelcomeOverlay::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    painter.setOpacity(m_overlayOpacity);

    if (!m_background.isNull()) {
        const QPixmap scaled = m_background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
        const QPoint topLeft((width() - scaled.width()) / 2, (height() - scaled.height()) / 2);
        painter.drawPixmap(topLeft, scaled);
    } else {
        painter.fillRect(rect(), QColor(QStringLiteral("#FAFAFA")));
    }

}

void WelcomeOverlay::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}
