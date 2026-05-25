#ifndef MOTIONHELPER_H
#define MOTIONHELPER_H

#include <QObject>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QEasingCurve>

class QStackedWidget;
class QWidget;
class QLabel;

namespace MotionTokens {
    // Animation durations for design tokens
    constexpr int DURATION_FAST = 150;      // Button hover, quick feedback
    constexpr int DURATION_BASE = 250;      // Standard animations
    constexpr int DURATION_SLOW = 400;      // Page transitions
    constexpr int DURATION_SLOWER = 600;    // Complex sequences
}

namespace MotionHelper {

// Basic animations
void fadeRiseIn(QWidget *widget, int distance = 8, int duration = 180);
void animateStackedSwitch(QStackedWidget *stack, int fromIndex, int toIndex, bool forward);

// Enhanced animation helpers
QPropertyAnimation* createHoverLift(QWidget *widget, int liftDistance = 2, int duration = MotionTokens::DURATION_FAST);
QPropertyAnimation* createButtonPulse(QWidget *widget, int duration = MotionTokens::DURATION_BASE);
QPropertyAnimation* createFadeIn(QWidget *widget, int duration = MotionTokens::DURATION_BASE);
QPropertyAnimation* createFadeOut(QWidget *widget, int duration = MotionTokens::DURATION_BASE);
QPropertyAnimation* createSlideIn(QWidget *widget, const QString &direction, int distance, int duration = MotionTokens::DURATION_BASE);

// Micro-interaction animations
void animateCheckboxToggle(QWidget *checkboxWidget, int duration = MotionTokens::DURATION_FAST);
void animateButtonClick(QWidget *button, int duration = MotionTokens::DURATION_FAST);
void animateCompletionFeedback(QWidget *parent, const QString &message);

} // namespace MotionHelper

#endif // MOTIONHELPER_H
