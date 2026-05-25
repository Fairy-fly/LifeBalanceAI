#ifndef BOTTOMNAVBAR_H
#define BOTTOMNAVBAR_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPropertyAnimation>

class BottomNavBar : public QWidget
{
    Q_OBJECT
public:
    explicit BottomNavBar(QWidget *parent = nullptr);

    void setCurrentIndex(int index);
    int currentIndex() const { return m_currentIndex; }

signals:
    void currentChanged(int index);

private:
    struct NavItem {
        QWidget *container = nullptr;
        QPushButton *button = nullptr;
        QLabel *textLabel = nullptr;
        QWidget *indicator = nullptr;
    };

    void setupUi();
    void updateActiveState(int index);
    NavItem addNavItem(int index);
    QRect pillGeometryFor(int index) const;

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QHBoxLayout *m_layout = nullptr;
    QWidget *m_activePill = nullptr;
    QList<NavItem> m_items;
    int m_currentIndex = 0;
    QString m_activeColor = "#2D7A5E";
    QString m_inactiveColor = "#999999";
};

#endif
