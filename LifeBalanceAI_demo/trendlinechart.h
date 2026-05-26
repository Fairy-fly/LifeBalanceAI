#ifndef TRENDLINECHART_H
#define TRENDLINECHART_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <QPointF>

class QPainter;

class TrendLineChart : public QWidget
{
    Q_OBJECT

public:
    enum ChartType {
        SevenDay,
        ThirtyDay,
        Weekly
    };

    explicit TrendLineChart(QWidget *parent = nullptr);

    void setData(const QVector<qreal> &values, const QVector<QString> &labels = {});
    void setChartType(ChartType type);
    void setColors(const QString &lineColor, const QString &fillColor, const QString &gridColor = "#E8E8E8");
    void setTitle(const QString &title);
    void setYAxisLabel(const QString &label);

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    struct Point {
        QPointF pos;
        qreal value;
    };

    void drawGrid(QPainter &painter, const QRectF &chartArea);
    void drawAxes(QPainter &painter, const QRectF &chartArea);
    void drawLine(QPainter &painter, const QRectF &chartArea);
    void drawFill(QPainter &painter, const QRectF &chartArea);
    void drawLabels(QPainter &painter, const QRectF &chartArea);
    void drawTitle(QPainter &painter);
    void calculatePoints(const QRectF &chartArea);

    QVector<qreal> m_values;
    QVector<QString> m_labels;
    QVector<Point> m_points;
    ChartType m_chartType = SevenDay;
    QString m_lineColor = "#4CAF7F";
    QString m_fillColor = "#E8F8F2";
    QString m_gridColor = "#E8E8E8";
    QString m_title = "";
    QString m_yAxisLabel = "";
    qreal m_maxValue = 100.0;
    qreal m_minValue = 0.0;
};

#endif // TRENDLINECHART_H
