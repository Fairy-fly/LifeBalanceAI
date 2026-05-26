#include "calendargridview.h"
#include <QPainter>
#include <QFont>
#include <QMouseEvent>
#include <QDate>

CalendarGridView::CalendarGridView(QWidget *parent)
    : QWidget(parent)
{
    QDate today = QDate::currentDate();
    m_month = today.month();
    m_year = today.year();
#ifdef Q_OS_ANDROID
    setMinimumSize(0, 286);
#else
    setMinimumSize(280, 350);
#endif
}

void CalendarGridView::setCheckInData(const QMap<QDate, CheckInStatus> &data)
{
    m_checkInData = data;
    update();
}

void CalendarGridView::addCheckIn(const QDate &date, CheckInStatus status)
{
    m_checkInData[date] = status;
    update();
}

void CalendarGridView::setTitle(const QString &title)
{
    m_title = title;
    update();
}

void CalendarGridView::setMonth(int month, int year)
{
    if (month >= 1 && month <= 12) {
        m_month = month;
        m_year = year;
        update();
    }
}

CalendarGridView::CheckInStatus CalendarGridView::status(const QDate &date) const
{
    return m_checkInData.value(date, NoData);
}

QSize CalendarGridView::sizeHint() const
{
#ifdef Q_OS_ANDROID
    return QSize(280, 300);
#else
    return QSize(320, 380);
#endif
}

QSize CalendarGridView::minimumSizeHint() const
{
#ifdef Q_OS_ANDROID
    return QSize(0, 286);
#else
    return QSize(280, 350);
#endif
}

void CalendarGridView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // Fill background
    painter.fillRect(rect(), QColor("#FEFEFE"));

    int topMargin = 40;
    QRectF gridArea(10, topMargin, width() - 20, height() - topMargin - 10);

    drawTitle(painter);
    drawDayLabels(painter, gridArea);
    drawCells(painter, gridArea);
}

void CalendarGridView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    m_cellSize = qMax(30, (width() - 30) / 7);
    update();
}

void CalendarGridView::mousePressEvent(QMouseEvent *event)
{
    int topMargin = 40 + 30;  // title + day labels
    QRectF gridArea(10, topMargin, width() - 20, height() - topMargin - 10);

    int col = (event->pos().x() - gridArea.left()) / m_cellSize;
    int row = (event->pos().y() - gridArea.top()) / m_cellSize;

    if (col >= 0 && col < 7 && row >= 0) {
        int dayOfWeek = firstDayOfMonth(m_month, m_year);
        int dayNumber = row * 7 + col - dayOfWeek + 1;
        int daysCount = daysInMonth(m_month, m_year);

        if (dayNumber >= 1 && dayNumber <= daysCount) {
            QDate date(m_year, m_month, dayNumber);
            emit dateClicked(date);
        }
    }
}

void CalendarGridView::drawTitle(QPainter &painter)
{
    QFont font = this->font();
    font.setPointSize(14);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor("#1A1A1A"));

    QString title = m_title.isEmpty()
        ? QStringLiteral("%1年%2月").arg(m_year).arg(m_month)
        : QStringLiteral("%1 · %2年%3月").arg(m_title).arg(m_year).arg(m_month);
    painter.drawText(QRectF(0, 5, width(), 30), Qt::AlignCenter, title);
}

void CalendarGridView::drawDayLabels(QPainter &painter, const QRectF &gridArea)
{
    QStringList dayLabels = {"日", "一", "二", "三", "四", "五", "六"};

    QFont font = this->font();
    font.setPointSize(10);
    font.setBold(true);
    painter.setFont(font);
    painter.setPen(QColor("#666666"));

    for (int i = 0; i < 7; ++i) {
        QRectF cellRect(gridArea.left() + i * m_cellSize, gridArea.top(), m_cellSize, 25);
        painter.drawText(cellRect, Qt::AlignCenter, dayLabels[i]);
    }
}

void CalendarGridView::drawCells(QPainter &painter, const QRectF &gridArea)
{
    int dayOfWeek = firstDayOfMonth(m_month, m_year);
    int daysCount = daysInMonth(m_month, m_year);

    int day = 1;
    for (int row = 0; row < 6; ++row) {
        for (int col = 0; col < 7; ++col) {
            if ((row == 0 && col < dayOfWeek) || day > daysCount) {
                continue;
            }

            QRectF cellRect(
                gridArea.left() + col * m_cellSize,
                gridArea.top() + 25 + row * m_cellSize,
                m_cellSize - 2,
                m_cellSize - 2
            );

            QDate date(m_year, m_month, day);
            drawCell(painter, row, col, date, cellRect);
            day++;
        }
    }
}

void CalendarGridView::drawCell(QPainter &painter, int row, int col, const QDate &date, const QRectF &cellRect)
{
    Q_UNUSED(row);
    Q_UNUSED(col);

    CheckInStatus checkStatus = status(date);
    QString bgColor = getColorForStatus(checkStatus);
    QString borderColor = "#E8E8E8";
    QString textColor = "#333333";

    // Draw background
    if (checkStatus != NoData) {
        painter.fillRect(cellRect, QColor(bgColor));
        painter.setPen(QPen(QColor(bgColor), 1));
    } else {
        painter.fillRect(cellRect, QColor("#FFFFFF"));
        painter.setPen(QPen(QColor(borderColor), 1));
    }

    painter.drawRect(cellRect);

    // Draw day number
    QFont font = this->font();
    font.setPointSize(11);
    if (checkStatus == Excellent) {
        font.setBold(true);
    }
    painter.setFont(font);
    painter.setPen(QColor(textColor));

    painter.drawText(cellRect, Qt::AlignCenter, QString::number(date.day()));
}

QString CalendarGridView::getColorForStatus(CheckInStatus status) const
{
    switch (status) {
    case NoData:
        return "#FFFFFF";
    case Incomplete:
        return "#FFE8D6";  // Light orange
    case Complete:
        return "#D4EDDA";  // Light green
    case Excellent:
        return "#B7EB8F";  // Bright green
    default:
        return "#FFFFFF";
    }
}

int CalendarGridView::daysInMonth(int month, int year) const
{
    return QDate(year, month, 1).daysInMonth();
}

int CalendarGridView::firstDayOfMonth(int month, int year) const
{
    return QDate(year, month, 1).dayOfWeek() % 7;
}
