#include "motionhelper.h"
#include "designtokens.h"

#include <QGraphicsOpacityEffect>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QStackedWidget>
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

} // namespace MotionHelper
