#include "circularprogressbar.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <QPropertyAnimation>
#include <cmath>

CircularProgressBar::CircularProgressBar(QWidget *parent)
    : QWidget(parent), m_value(0.0), m_displayValue(0.0), m_lineWidth(12)
{
#ifdef Q_OS_ANDROID
    setMinimumSize(92, 92);
    setMaximumSize(132, 132);
#else
    setMinimumSize(120, 120);
    setMaximumSize(300, 300);
#endif
}

void CircularProgressBar::setValue(qreal val)
{
    val = qBound(0.0, val, 100.0);
    if (qFuzzyCompare(m_value + 1.0, val + 1.0))
        return;

    m_value = val;

    if (m_animation) {
        m_animation->stop();
        m_animation->deleteLater();
    }

    m_animation = new QPropertyAnimation(this, "displayValue", this);
    m_animation->setDuration(500);
    m_animation->setStartValue(m_displayValue);
    m_animation->setEndValue(val);
    m_animation->setEasingCurve(QEasingCurve::OutCubic);
    connect(m_animation, &QPropertyAnimation::finished, this, [this]() {
        m_animation = nullptr;
    });
    m_animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void CircularProgressBar::setDisplayValue(qreal val)
{
    m_displayValue = qBound(0.0, val, 100.0);
    update();
}

void CircularProgressBar::setLineWidth(int width)
{
    if (m_lineWidth != width) {
        m_lineWidth = width;
        update();
    }
}

void CircularProgressBar::setColors(const QString &background, const QString &progress)
{
    m_bgColor = background;
    m_progressColor = progress;
    update();
}

void CircularProgressBar::setText(const QString &text)
{
    m_displayText = text;
    update();
}

void CircularProgressBar::setAnimationDuration(int ms)
{
    if (m_animation) {
        m_animation->setDuration(ms);
    }
}

QSize CircularProgressBar::sizeHint() const
{
#ifdef Q_OS_ANDROID
    return QSize(116, 116);
#else
    return QSize(200, 200);
#endif
}

QSize CircularProgressBar::minimumSizeHint() const
{
#ifdef Q_OS_ANDROID
    return QSize(92, 92);
#else
    return QSize(120, 120);
#endif
}

void CircularProgressBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    int side = qMin(width(), height());
    int margin = m_lineWidth / 2 + 2;
    QRectF rect(margin, margin, side - 2 * margin, side - 2 * margin);

    drawBackground(painter, rect);
    drawProgress(painter, rect);
    drawText(painter, rect);
}

void CircularProgressBar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

void CircularProgressBar::drawBackground(QPainter &painter, const QRectF &rect)
{
    painter.setPen(QPen(QColor(m_bgColor), m_lineWidth, Qt::SolidLine, Qt::RoundCap));
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(rect);
}

void CircularProgressBar::drawProgress(QPainter &painter, const QRectF &rect)
{
    if (m_displayValue <= 0) return;

    QPen pen(QColor(m_progressColor), m_lineWidth, Qt::SolidLine, Qt::RoundCap);
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);

    // Draw arc from -90 degrees (top) to progress angle
    qreal angle = (m_displayValue / 100.0) * 360.0;
    painter.drawArc(rect, 90 * 16, -static_cast<int>(angle * 16));
}

void CircularProgressBar::drawText(QPainter &painter, const QRectF &rect)
{
    QFont font = this->font();
    int fontSize = qMax(12, static_cast<int>(rect.width() / 6));
    font.setPointSize(fontSize);
    font.setBold(true);
    painter.setFont(font);

    QString displayText;
    if (!m_displayText.isEmpty()) {
        displayText = m_displayText;
    } else {
        displayText = QString::number(static_cast<int>(m_displayValue)) + "%";
    }

    painter.setPen(QColor("#1A1A1A"));
    painter.drawText(rect, Qt::AlignCenter, displayText);
}
