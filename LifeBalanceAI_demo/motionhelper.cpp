#include "motionhelper.h"
#include "completionfeedback.h"
#include "designtokens.h"

#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QStackedWidget>
#include <QTimer>
#include <QWidget>

namespace MotionHelper {

void fadeRiseIn(QWidget *widget, int distance, int duration)
{
    if (!widget)
        return;

#ifdef Q_OS_ANDROID
    Q_UNUSED(distance)
    Q_UNUSED(duration)
    widget->show();
    return;
#else
    QPoint endPos = widget->pos();
    QPoint startPos = endPos + QPoint(0, distance);
    widget->move(startPos);
    widget->setWindowOpacity(1.0);

    auto *effect = qobject_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
    }
    effect->setOpacity(0.0);

    auto *group = new QParallelAnimationGroup(widget);
    auto *move = new QPropertyAnimation(widget, "pos", group);
    move->setDuration(duration);
    move->setStartValue(startPos);
    move->setEndValue(endPos);
    move->setEasingCurve(QEasingCurve::OutCubic);

    auto *fade = new QPropertyAnimation(effect, "opacity", group);
    fade->setDuration(duration);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    fade->setEasingCurve(QEasingCurve::OutCubic);

    QObject::connect(group, &QParallelAnimationGroup::finished, widget, [widget]() {
        widget->setGraphicsEffect(nullptr);
    });
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}

void animateStackedSwitch(QStackedWidget *stack, int fromIndex, int toIndex, bool forward)
{
    Q_UNUSED(forward)
    if (!stack || fromIndex == toIndex || toIndex < 0 || toIndex >= stack->count()) {
        if (stack && toIndex >= 0 && toIndex < stack->count())
            stack->setCurrentIndex(toIndex);
        return;
    }

#ifdef Q_OS_ANDROID
    QWidget *from = stack->widget(fromIndex);
    QWidget *to = stack->widget(toIndex);
    if (!from || !to) {
        stack->setCurrentIndex(toIndex);
        return;
    }

    const QRect area = stack->rect();
    const int offset = qMin(22, qMax(12, area.width() / 24));
    const QPoint basePos = from->pos();
    const QPoint fromEnd = basePos + QPoint(forward ? -offset : offset, 0);
    const QPoint toStart = basePos + QPoint(forward ? offset : -offset, 0);

    to->setGeometry(area);
    to->move(toStart);
    to->show();
    to->raise();

    auto *group = new QParallelAnimationGroup(stack);
    auto *fromMove = new QPropertyAnimation(from, "pos", group);
    fromMove->setDuration(180);
    fromMove->setStartValue(basePos);
    fromMove->setEndValue(fromEnd);
    fromMove->setEasingCurve(QEasingCurve::OutCubic);

    auto *toMove = new QPropertyAnimation(to, "pos", group);
    toMove->setDuration(180);
    toMove->setStartValue(toStart);
    toMove->setEndValue(basePos);
    toMove->setEasingCurve(QEasingCurve::OutCubic);

    QObject::connect(group, &QParallelAnimationGroup::finished, stack, [=]() {
        stack->setCurrentIndex(toIndex);
        from->move(basePos);
        to->move(basePos);
    });
    group->start(QAbstractAnimation::DeleteWhenStopped);
    return;
#else
    QWidget *from = stack->widget(fromIndex);
    QWidget *to = stack->widget(toIndex);
    if (!from || !to) {
        stack->setCurrentIndex(toIndex);
        return;
    }

    QRect area = stack->rect();
    const int offset = qMin(28, qMax(14, area.width() / 18));
    QPoint fromStart = from->pos();
    QPoint fromEnd = fromStart + QPoint(forward ? -offset : offset, 0);
    QPoint toEnd = fromStart;
    QPoint toStart = fromStart + QPoint(forward ? offset : -offset, 0);

    auto *fromEffect = new QGraphicsOpacityEffect(from);
    auto *toEffect = new QGraphicsOpacityEffect(to);
    from->setGraphicsEffect(fromEffect);
    to->setGraphicsEffect(toEffect);
    fromEffect->setOpacity(1.0);
    toEffect->setOpacity(0.0);

    to->setGeometry(area);
    to->move(toStart);
    to->show();
    to->raise();

    auto *group = new QParallelAnimationGroup(stack);
    auto *fromMove = new QPropertyAnimation(from, "pos", group);
    fromMove->setDuration(DesignTokens::MotionBase);
    fromMove->setStartValue(fromStart);
    fromMove->setEndValue(fromEnd);
    fromMove->setEasingCurve(QEasingCurve::OutCubic);

    auto *toMove = new QPropertyAnimation(to, "pos", group);
    toMove->setDuration(DesignTokens::MotionBase);
    toMove->setStartValue(toStart);
    toMove->setEndValue(toEnd);
    toMove->setEasingCurve(QEasingCurve::OutCubic);

    auto *fromFade = new QPropertyAnimation(fromEffect, "opacity", group);
    fromFade->setDuration(DesignTokens::MotionBase);
    fromFade->setStartValue(1.0);
    fromFade->setEndValue(0.0);

    auto *toFade = new QPropertyAnimation(toEffect, "opacity", group);
    toFade->setDuration(DesignTokens::MotionBase);
    toFade->setStartValue(0.0);
    toFade->setEndValue(1.0);

    QObject::connect(group, &QParallelAnimationGroup::finished, stack, [=]() {
        stack->setCurrentIndex(toIndex);
        from->move(fromStart);
        to->move(toEnd);
        from->setGraphicsEffect(nullptr);
        to->setGraphicsEffect(nullptr);
    });

    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}

QPropertyAnimation *createHoverLift(QWidget *widget, int liftDistance, int duration)
{
    if (!widget)
        return nullptr;
    auto *anim = new QPropertyAnimation(widget, "pos", widget);
    anim->setDuration(duration);
    anim->setStartValue(widget->pos());
    anim->setEndValue(widget->pos() + QPoint(0, -qMax(0, liftDistance)));
    anim->setEasingCurve(QEasingCurve::OutCubic);
    return anim;
}

QPropertyAnimation *createButtonPulse(QWidget *widget, int duration)
{
    if (!widget)
        return nullptr;
    auto *effect = qobject_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
    }
    auto *anim = new QPropertyAnimation(effect, "opacity", widget);
    anim->setDuration(duration);
    anim->setStartValue(0.72);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    return anim;
}

QPropertyAnimation *createFadeIn(QWidget *widget, int duration)
{
    if (!widget)
        return nullptr;
    auto *effect = qobject_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
    }
    effect->setOpacity(0.0);
    auto *anim = new QPropertyAnimation(effect, "opacity", widget);
    anim->setDuration(duration);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    QObject::connect(anim, &QPropertyAnimation::finished, widget, [widget]() {
        widget->setGraphicsEffect(nullptr);
    });
    return anim;
}

QPropertyAnimation *createFadeOut(QWidget *widget, int duration)
{
    if (!widget)
        return nullptr;
    auto *effect = qobject_cast<QGraphicsOpacityEffect *>(widget->graphicsEffect());
    if (!effect) {
        effect = new QGraphicsOpacityEffect(widget);
        widget->setGraphicsEffect(effect);
    }
    effect->setOpacity(1.0);
    auto *anim = new QPropertyAnimation(effect, "opacity", widget);
    anim->setDuration(duration);
    anim->setStartValue(1.0);
    anim->setEndValue(0.0);
    anim->setEasingCurve(QEasingCurve::InCubic);
    return anim;
}

QPropertyAnimation *createSlideIn(QWidget *widget, const QString &direction, int distance, int duration)
{
    if (!widget)
        return nullptr;

    const QPoint end = widget->pos();
    QPoint start = end;
    if (direction.compare(QStringLiteral("left"), Qt::CaseInsensitive) == 0)
        start.rx() -= distance;
    else if (direction.compare(QStringLiteral("right"), Qt::CaseInsensitive) == 0)
        start.rx() += distance;
    else if (direction.compare(QStringLiteral("up"), Qt::CaseInsensitive) == 0)
        start.ry() -= distance;
    else
        start.ry() += distance;

    widget->move(start);
    auto *anim = new QPropertyAnimation(widget, "pos", widget);
    anim->setDuration(duration);
    anim->setStartValue(start);
    anim->setEndValue(end);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    return anim;
}

void animateCheckboxToggle(QWidget *checkboxWidget, int duration)
{
    auto *anim = createButtonPulse(checkboxWidget, duration);
    if (anim)
        anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void animateButtonClick(QWidget *button, int duration)
{
    if (!button)
        return;
    const QPoint original = button->pos();
    auto *down = new QPropertyAnimation(button, "pos", button);
    down->setDuration(qMax(60, duration / 2));
    down->setStartValue(original);
    down->setEndValue(original + QPoint(0, 1));
    down->setEasingCurve(QEasingCurve::OutCubic);

    auto *up = new QPropertyAnimation(button, "pos", button);
    up->setDuration(qMax(60, duration / 2));
    up->setStartValue(original + QPoint(0, 1));
    up->setEndValue(original);
    up->setEasingCurve(QEasingCurve::OutCubic);

    auto *sequence = new QSequentialAnimationGroup(button);
    sequence->addAnimation(down);
    sequence->addAnimation(up);
    sequence->start(QAbstractAnimation::DeleteWhenStopped);
}

void animateCompletionFeedback(QWidget *parent, const QString &message)
{
    if (!parent)
        return;
    auto *overlay = new CompletionFeedback(parent);
    overlay->setAttribute(Qt::WA_DeleteOnClose);
    overlay->setGeometry(parent->rect());
    overlay->raise();
    overlay->showCompletionFeedback(message);
    QTimer::singleShot(1100, overlay, &QWidget::close);
}

} // namespace MotionHelper
