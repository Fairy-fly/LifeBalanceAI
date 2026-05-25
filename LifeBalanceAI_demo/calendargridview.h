#ifndef CALENDARGRIDVIEW_H
#define CALENDARGRIDVIEW_H

#include <QWidget>
#include <QMap>
#include <QDate>
#include <QString>

class CalendarGridView : public QWidget
{
    Q_OBJECT

public:
    enum CheckInStatus {
        NoData = 0,
        Incomplete = 1,
        Complete = 2,
        Excellent = 3
    };

    explicit CalendarGridView(QWidget *parent = nullptr);

    void setCheckInData(const QMap<QDate, CheckInStatus> &data);
    void addCheckIn(const QDate &date, CheckInStatus status);
    void setTitle(const QString &title);
    void setMonth(int month, int year);

    CheckInStatus status(const QDate &date) const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void dateClicked(const QDate &date);

private:
    void drawTitle(QPainter &painter);
    void drawDayLabels(QPainter &painter, const QRectF &gridArea);
    void drawCells(QPainter &painter, const QRectF &gridArea);
    void drawCell(QPainter &painter, int row, int col, const QDate &date, const QRectF &cellRect);

    QString getColorForStatus(CheckInStatus status) const;
    int daysInMonth(int month, int year) const;
    int firstDayOfMonth(int month, int year) const;

    QMap<QDate, CheckInStatus> m_checkInData;
    QString m_title = "打卡日历";
    int m_month = 0;  // Current month
    int m_year = 0;   // Current year
    int m_cellSize = 40;
};

#endif // CALENDARGRIDVIEW_H
