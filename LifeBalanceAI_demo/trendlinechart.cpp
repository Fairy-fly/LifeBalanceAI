#include "trendlinechart.h"
#include <QPainter>
#include <QPainterPath>
#include <QFont>
#include <algorithm>
#include <cmath>

TrendLineChart::TrendLineChart(QWidget *parent)
    : QWidget(parent)
{
#ifdef Q_OS_ANDROID
    setMinimumSize(0, 150);
#else
    setMinimumSize(300, 200);
#endif
}

void TrendLineChart::setData(const QVector<qreal> &values, const QVector<QString> &labels)
{
    m_values = values;
    m_labels = labels;

    if (m_values.isEmpty()) return;

    // Calculate min/max
    m_maxValue = *std::max_element(m_values.begin(), m_values.end());
    m_minValue = *std::min_element(m_values.begin(), m_values.end());

    // Add 10% padding
    qreal padding = (m_maxValue - m_minValue) * 0.1;
    if (padding == 0) padding = 10;
    m_maxValue += padding;
    m_minValue = std::max(0.0, m_minValue - padding);

    // Auto-generate labels if not provided
    if (m_labels.isEmpty()) {
        for (int i = 0; i < m_values.size(); ++i) {
            m_labels.append(QString::number(i + 1));
        }
    }

    update();
}

void TrendLineChart::setChartType(ChartType type)
{
    if (m_chartType != type) {
        m_chartType = type;
        update();
    }
}

void TrendLineChart::setColors(const QString &lineColor, const QString &fillColor, const QString &gridColor)
{
    m_lineColor = lineColor;
    m_fillColor = fillColor;
    m_gridColor = gridColor;
    update();
}

void TrendLineChart::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void TrendLineChart::setYAxisLabel(const QString &label)
{
    m_yAxisLabel = label;
    update();
}

QSize TrendLineChart::sizeHint() const
{
#ifdef Q_OS_ANDROID
    return QSize(280, 170);
#else
    return QSize(400, 250);
#endif
}

QSize TrendLineChart::minimumSizeHint() const
{
#ifdef Q_OS_ANDROID
    return QSize(0, 150);
#else
    return QSize(300, 200);
#endif
}

void TrendLineChart::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (m_values.isEmpty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    // Calculate chart area (leave space for title and labels)
    int topMargin = m_title.isEmpty() ? 10 : 40;
    int bottomMargin = 50;
    int leftMargin = m_yAxisLabel.isEmpty() ? 40 : 60;
    int rightMargin = 20;

    QRectF chartArea(leftMargin, topMargin, width() - leftMargin - rightMargin, height() - topMargin - bottomMargin);

    drawTitle(painter);
    drawGrid(painter, chartArea);
    drawAxes(painter, chartArea);
    drawFill(painter, chartArea);
    drawLine(painter, chartArea);
    drawLabels(painter, chartArea);
}

void TrendLineChart::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    update();
}

void TrendLineChart::calculatePoints(const QRectF &chartArea)
{
    m_points.clear();

    if (m_values.isEmpty()) return;

    qreal range = m_maxValue - m_minValue;
    if (range <= 0) range = 1;

    for (int i = 0; i < m_values.size(); ++i) {
        const qreal x = (m_values.size() == 1)
            ? chartArea.center().x()
            : chartArea.left() + (i * chartArea.width()) / (m_values.size() - 1);
        qreal normalizedValue = (m_values[i] - m_minValue) / range;
        qreal y = chartArea.bottom() - (normalizedValue * chartArea.height());

        Point point;
        point.pos = QPointF(x, y);
        point.value = m_values[i];
        m_points.append(point);
    }
}

void TrendLineChart::drawGrid(QPainter &painter, const QRectF &chartArea)
{
    painter.setPen(QPen(QColor(m_gridColor), 1));

    // Horizontal grid lines
    int gridLines = 4;
    for (int i = 0; i <= gridLines; ++i) {
        qreal y = chartArea.top() + (i * chartArea.height()) / gridLines;
        painter.drawLine(chartArea.left(), y, chartArea.right(), y);
    }
}

void TrendLineChart::drawAxes(QPainter &painter, const QRectF &chartArea)
{
    painter.setPen(QPen(QColor("#D9D9D9"), 1));

    // X axis
    painter.drawLine(chartArea.left(), chartArea.bottom(), chartArea.right(), chartArea.bottom());

    // Y axis
    painter.drawLine(chartArea.left(), chartArea.top(), chartArea.left(), chartArea.bottom());
}

void TrendLineChart::drawLine(QPainter &painter, const QRectF &chartArea)
{
    calculatePoints(chartArea);

    if (m_points.size() < 2) return;

    QPen pen(QColor(m_lineColor), 2.5);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    painter.setPen(pen);

    // Draw line path
    QPainterPath path;
    path.moveTo(m_points[0].pos);
    for (int i = 1; i < m_points.size(); ++i) {
        path.lineTo(m_points[i].pos);
    }
    painter.drawPath(path);

    // Draw data point circles
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(m_lineColor));
    for (const Point &point : m_points) {
        painter.drawEllipse(point.pos, 4, 4);
    }
}

void TrendLineChart::drawFill(QPainter &painter, const QRectF &chartArea)
{
    calculatePoints(chartArea);

    if (m_points.size() < 2) return;

    // Create fill path
    QPainterPath path;
    path.moveTo(m_points[0].pos);
    for (int i = 1; i < m_points.size(); ++i) {
        path.lineTo(m_points[i].pos);
    }
    path.lineTo(m_points.last().pos.x(), chartArea.bottom());
    path.lineTo(m_points[0].pos.x(), chartArea.bottom());
    path.closeSubpath();

    painter.fillPath(path, QColor(m_fillColor));
}

void TrendLineChart::drawLabels(QPainter &painter, const QRectF &chartArea)
{
    calculatePoints(chartArea);

    QFont font = this->font();
    font.setPointSize(10);
    painter.setFont(font);
    painter.setPen(QColor("#666666"));

    // X-axis labels
    for (int i = 0; i < m_points.size(); ++i) {
        QString label = (i < m_labels.size()) ? m_labels[i] : QString::number(i + 1);
        QRectF textRect(m_points[i].pos.x() - 20, chartArea.bottom() + 5, 40, 30);
        painter.drawText(textRect, Qt::AlignCenter, label);
    }

    // Y-axis label
    if (!m_yAxisLabel.isEmpty()) {
        painter.save();
        painter.translate(10, chartArea.center().y());
        painter.rotate(-90);
        QRectF textRect(-chartArea.height() / 2, -20, chartArea.height(), 40);
        painter.drawText(textRect, Qt::AlignCenter, m_yAxisLabel);
        painter.restore();
    }
}

void TrendLineChart::drawTitle(QPainter &painter)
{
    if (m_title.isEmpty()) return;

    QFont font = this->font();
    font.setPointSize(12);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor("#1A1A1A"));

    QRectF titleRect(0, 5, width(), 30);
    painter.drawText(titleRect, Qt::AlignCenter, m_title);
}
