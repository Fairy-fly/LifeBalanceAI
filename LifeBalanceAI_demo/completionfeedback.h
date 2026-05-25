#ifndef COMPLETIONFEEDBACK_H
#define COMPLETIONFEEDBACK_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QString>

class CompletionFeedback : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal scale READ scale WRITE setScale)
    Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
    explicit CompletionFeedback(QWidget *parent = nullptr);

    void showCompletionFeedback(const QString &message = "打卡完成！");
    void showAchievementUnlocked(const QString &title, const QString &description);

    qreal scale() const { return m_scale; }
    void setScale(qreal scale);

    qreal opacity() const { return m_opacity; }
    void setOpacity(qreal op);

protected:
    void paintEvent(QPaintEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private:
    void playCompletionAnimation();
    void playAchievementAnimation();
    void drawCompletionContent(QPainter &painter);
    void drawAchievementContent(QPainter &painter);

    enum FeedbackType {
        None,
        Completion,
        Achievement
    };

    FeedbackType m_type = None;
    QString m_message = "";
    QString m_achievementTitle = "";
    QString m_achievementDescription = "";
    qreal m_scale = 0.8;
    qreal m_opacity = 1.0;

    QSequentialAnimationGroup *m_animationGroup = nullptr;
};

#endif // COMPLETIONFEEDBACK_H
