#ifndef SIDEDRAWER_H
#define SIDEDRAWER_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <functional>

class SideDrawer : public QWidget
{
    Q_OBJECT
public:
    explicit SideDrawer(QWidget *parent = nullptr);

    void addHeader(const QString &nickname, const QString &role);
    void addItem(const QString &icon, const QString &text, std::function<void()> callback);
    void addSeparator();
    void showAnimated();
    void hideAnimated();
    bool isVisible() const { return m_visible; }

signals:
    void closed();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QWidget *m_overlay = nullptr;
    QWidget *m_panel = nullptr;
    QVBoxLayout *m_panelLayout = nullptr;
    QFrame *m_currentGroup = nullptr;
    QVBoxLayout *m_currentGroupLayout = nullptr;
    int m_currentGroupItemCount = 0;
    QPropertyAnimation *m_slideAnim = nullptr;
    QPropertyAnimation *m_fadeAnim = nullptr;
    bool m_visible = false;
    int m_panelWidth = 0;
    QLabel *m_headerNickname = nullptr;
    QLabel *m_headerRole = nullptr;
};

#endif
