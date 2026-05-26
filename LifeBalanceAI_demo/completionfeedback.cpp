#include "completionfeedback.h"
#include <QPainter>
#include <QFont>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>
#include <QRadialGradient>

CompletionFeedback::CompletionFeedback(QWidget *parent)
    : QWidget(parent), m_scale(0.8), m_opacity(1.0)
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

void CompletionFeedback::showCompletionFeedback(const QString &message)
{
    m_type = Completion;
    m_message = message;
    m_scale = 0.65;
    m_opacity = 0;
    show();
    playCompletionAnimation();
}

void CompletionFeedback::showAchievementUnlocked(const QString &title, const QString &description)
{
    m_type = Achievement;
    m_achievementTitle = title;
    m_achievementDescription = description;
    m_scale = 0.5;
    m_opacity = 0;
    show();
    playAchievementAnimation();
}

void CompletionFeedback::setScale(qreal scale)
{
    m_scale = scale;
    update();
}

void CompletionFeedback::setOpacity(qreal op)
{
    m_opacity = op;
    update();
}

void CompletionFeedback::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity(m_opacity);

    // Apply scale transform (center)
    painter.translate(width() / 2, height() / 2);
    painter.scale(m_scale, m_scale);
    painter.translate(-width() / 2, -height() / 2);

    if (m_type == Completion) {
        drawCompletionContent(painter);
    } else if (m_type == Achievement) {
        drawAchievementContent(painter);
    }
}

void CompletionFeedback::hideEvent(QHideEvent *event)
{
    m_type = None;
    QWidget::hideEvent(event);
}

void CompletionFeedback::playCompletionAnimation()
{
    if (m_animationGroup) {
        m_animationGroup->stop();
        delete m_animationGroup;
    }

    m_animationGroup = new QSequentialAnimationGroup(this);

    QPropertyAnimation *scaleIn = new QPropertyAnimation(this, "scale");
    scaleIn->setDuration(220);
    scaleIn->setStartValue(0.65);
    scaleIn->setEndValue(1.0);
    scaleIn->setEasingCurve(QEasingCurve::OutCubic);

    QPropertyAnimation *opacityIn = new QPropertyAnimation(this, "opacity");
    opacityIn->setDuration(180);
    opacityIn->setStartValue(0.0);
    opacityIn->setEndValue(1.0);
    opacityIn->setEasingCurve(QEasingCurve::OutCubic);

    QParallelAnimationGroup *group1 = new QParallelAnimationGroup();
    group1->addAnimation(scaleIn);
    group1->addAnimation(opacityIn);

    m_animationGroup->addAnimation(group1);

    QPropertyAnimation *pause = new QPropertyAnimation(this, "scale");
    pause->setDuration(620);

    m_animationGroup->addAnimation(pause);

    QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "opacity");
    fadeOut->setDuration(240);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InOutQuad);

    QPropertyAnimation *scaleOut = new QPropertyAnimation(this, "scale");
    scaleOut->setDuration(240);
    scaleOut->setStartValue(1.0);
    scaleOut->setEndValue(0.92);
    scaleOut->setEasingCurve(QEasingCurve::InOutQuad);

    QParallelAnimationGroup *group2 = new QParallelAnimationGroup();
    group2->addAnimation(fadeOut);
    group2->addAnimation(scaleOut);

    m_animationGroup->addAnimation(group2);

    connect(m_animationGroup, &QSequentialAnimationGroup::finished, this, &QWidget::hide);
    m_animationGroup->start();
}

void CompletionFeedback::playAchievementAnimation()
{
    if (m_animationGroup) {
        m_animationGroup->stop();
        delete m_animationGroup;
    }

    m_animationGroup = new QSequentialAnimationGroup(this);

    // Bounce-in animation
    QPropertyAnimation *bounceIn = new QPropertyAnimation(this, "scale");
    bounceIn->setDuration(600);
    bounceIn->setStartValue(0.5);
    bounceIn->setEndValue(1.0);
    bounceIn->setEasingCurve(QEasingCurve::OutBounce);

    QPropertyAnimation *opacityIn = new QPropertyAnimation(this, "opacity");
    opacityIn->setDuration(600);
    opacityIn->setStartValue(0.0);
    opacityIn->setEndValue(1.0);
    opacityIn->setEasingCurve(QEasingCurve::InOutQuad);

    QParallelAnimationGroup *group1 = new QParallelAnimationGroup();
    group1->addAnimation(bounceIn);
    group1->addAnimation(opacityIn);

    m_animationGroup->addAnimation(group1);

    // Pause longer for achievement
    QPropertyAnimation *pause = new QPropertyAnimation(this, "scale");
    pause->setDuration(2000);

    m_animationGroup->addAnimation(pause);

    // Fade out
    QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "opacity");
    fadeOut->setDuration(400);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InQuad);

    m_animationGroup->addAnimation(fadeOut);

    connect(m_animationGroup, &QSequentialAnimationGroup::finished, this, &QWidget::hide);
    m_animationGroup->start();
}

void CompletionFeedback::drawCompletionContent(QPainter &painter)
{
    const int size = 138;
    const int x = (width() - size) / 2;
    const int y = (height() - size) / 2 - 18;
    const QPoint center(x + size / 2, y + size / 2);

    QRadialGradient glow(center, size * 0.82);
    glow.setColorAt(0.0, QColor(255, 255, 255, 245));
    glow.setColorAt(0.65, QColor(232, 248, 242, 236));
    glow.setColorAt(1.0, QColor(232, 248, 242, 0));

    painter.setPen(Qt::NoPen);
    painter.setBrush(glow);
    painter.drawEllipse(QRect(x - 18, y - 18, size + 36, size + 36));

    painter.setBrush(QColor(255, 255, 255, 232));
    painter.setPen(QPen(QColor("#4CAF7F"), 3));
    painter.drawEllipse(QRect(x, y, size, size));

    painter.setPen(QPen(QColor("#4CAF7F"), 7, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    const int cx = width() / 2;
    const int cy = y + size / 2;
    painter.drawLine(cx - 34, cy + 4, cx - 10, cy + 28);
    painter.drawLine(cx - 10, cy + 28, cx + 38, cy - 24);

    QFont font = this->font();
    font.setPointSize(15);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor("#2D7A5E"));

    QRect textRect(x - 30, y + size + 12, size + 60, 38);
    painter.drawText(textRect, Qt::AlignCenter | Qt::TextWordWrap, m_message);
}

void CompletionFeedback::drawAchievementContent(QPainter &painter)
{
    // Draw card background with glow
    int cardW = 280;
    int cardH = 200;
    int x = (width() - cardW) / 2;
    int y = (height() - cardH) / 2;

    painter.fillRect(QRect(x, y, cardW, cardH), QColor("#FEFEFE"));
    painter.setPen(QPen(QColor("#FFD700"), 3));
    painter.drawRect(x, y, cardW, cardH);

    // Draw medal/badge symbol
    int medalSize = 80;
    int medalX = x + (cardW - medalSize) / 2;
    int medalY = y + 20;

    painter.fillRect(QRect(medalX, medalY, medalSize, medalSize), QColor("#FFD700"));
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#FFF8DC"));
    painter.drawEllipse(medalX + 15, medalY + 15, 50, 50);

    // Draw title
    QFont titleFont = this->font();
    titleFont.setPointSize(14);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.setPen(QColor("#1A1A1A"));

    QRect titleRect(x + 20, y + 110, cardW - 40, 30);
    painter.drawText(titleRect, Qt::AlignCenter, m_achievementTitle);

    // Draw description
    QFont descFont = this->font();
    descFont.setPointSize(11);
    painter.setFont(descFont);
    painter.setPen(QColor("#666666"));

    QRect descRect(x + 20, y + 140, cardW - 40, 50);
    painter.drawText(descRect, Qt::AlignCenter | Qt::TextWordWrap, m_achievementDescription);
}
