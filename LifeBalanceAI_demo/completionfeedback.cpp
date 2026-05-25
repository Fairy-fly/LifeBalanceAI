#include "completionfeedback.h"
#include <QPainter>
#include <QFont>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

CompletionFeedback::CompletionFeedback(QWidget *parent)
    : QWidget(parent), m_scale(0.8), m_opacity(1.0)
{
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint | Qt::TransparentForMouseEvents);
    setAttribute(Qt::WA_TranslucentBackground);
}

void CompletionFeedback::showCompletionFeedback(const QString &message)
{
    m_type = Completion;
    m_message = message;
    m_scale = 0.3;
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

    // Scale-in animation
    QPropertyAnimation *scaleIn = new QPropertyAnimation(this, "scale");
    scaleIn->setDuration(400);
    scaleIn->setStartValue(0.3);
    scaleIn->setEndValue(1.1);
    scaleIn->setEasingCurve(QEasingCurve::OutElastic);

    // Opacity animation (parallel)
    QPropertyAnimation *opacityIn = new QPropertyAnimation(this, "opacity");
    opacityIn->setDuration(400);
    opacityIn->setStartValue(0.0);
    opacityIn->setEndValue(1.0);
    opacityIn->setEasingCurve(QEasingCurve::InOutQuad);

    QParallelAnimationGroup *group1 = new QParallelAnimationGroup();
    group1->addAnimation(scaleIn);
    group1->addAnimation(opacityIn);

    m_animationGroup->addAnimation(group1);

    // Pause
    QPropertyAnimation *pause = new QPropertyAnimation(this, "scale");
    pause->setDuration(1000);

    m_animationGroup->addAnimation(pause);

    // Fade out and shrink
    QPropertyAnimation *fadeOut = new QPropertyAnimation(this, "opacity");
    fadeOut->setDuration(300);
    fadeOut->setStartValue(1.0);
    fadeOut->setEndValue(0.0);
    fadeOut->setEasingCurve(QEasingCurve::InQuad);

    QPropertyAnimation *scaleOut = new QPropertyAnimation(this, "scale");
    scaleOut->setDuration(300);
    scaleOut->setStartValue(1.0);
    scaleOut->setEndValue(0.8);
    scaleOut->setEasingCurve(QEasingCurve::InQuad);

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
    // Draw circular background
    int size = 160;
    int x = (width() - size) / 2;
    int y = (height() - size) / 2;

    painter.fillRect(QRect(x, y, size, size), QColor(255, 255, 255, 230));
    painter.setPen(QPen(QColor("#4CAF7F"), 2));
    painter.drawEllipse(x, y, size, size);

    // Draw checkmark
    painter.setPen(QPen(QColor("#4CAF7F"), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    int cx = width() / 2;
    int cy = height() / 2;
    painter.drawLine(cx - 30, cy + 5, cx - 10, cy + 25);
    painter.drawLine(cx - 10, cy + 25, cx + 30, cy - 15);

    // Draw message text
    QFont font = this->font();
    font.setPointSize(16);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor("#1A1A1A"));

    QRect textRect(x, y + size + 20, size, 50);
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
