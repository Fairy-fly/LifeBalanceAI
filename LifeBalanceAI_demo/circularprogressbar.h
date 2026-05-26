#ifndef CIRCULARPROGRESSBAR_H
#define CIRCULARPROGRESSBAR_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QString>

class CircularProgressBar : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal value READ value WRITE setValue)
    Q_PROPERTY(qreal displayValue READ displayValue WRITE setDisplayValue)
    Q_PROPERTY(int lineWidth READ lineWidth WRITE setLineWidth)

public:
    explicit CircularProgressBar(QWidget *parent = nullptr);

    void setValue(qreal val);
    qreal value() const { return m_value; }
    qreal displayValue() const { return m_displayValue; }
    void setDisplayValue(qreal val);

    void setLineWidth(int width);
    int lineWidth() const { return m_lineWidth; }

    void setColors(const QString &background, const QString &progress);
    void setText(const QString &text);
    void setAnimationDuration(int ms);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void drawBackground(QPainter &painter, const QRectF &rect);
    void drawProgress(QPainter &painter, const QRectF &rect);
    void drawText(QPainter &painter, const QRectF &rect);

    qreal m_value = 0.0;
    qreal m_displayValue = 0.0;  // For animation
    int m_lineWidth = 12;
    QString m_bgColor = "#E8E8E8";
    QString m_progressColor = "#4CAF7F";
    QString m_displayText = "";
    QPropertyAnimation *m_animation = nullptr;
};

#endif // CIRCULARPROGRESSBAR_H
