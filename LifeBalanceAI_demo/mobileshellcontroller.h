#ifndef MOBILESHELLCONTROLLER_H
#define MOBILESHELLCONTROLLER_H

#include "approute.h"

#include <QObject>
#include <QHash>
#include <functional>

class BottomNavBar;
class QStackedWidget;
class QWidget;

class MobileShellController : public QObject
{
    Q_OBJECT

public:
    explicit MobileShellController(QObject *parent = nullptr);

    void setShellWidgets(BottomNavBar *bottomNav, QStackedWidget *stack, QWidget *host);
    void setRouteIndex(AppRoute route, int pageIndex);
    int routeIndex(AppRoute route) const;
    AppRoute routeForPageIndex(int pageIndex) const;
    AppRoute routeForBottomTab(int tab) const;
    int bottomTabForRoute(AppRoute route) const;
    bool isBottomNavRoute(AppRoute route) const;

    void setRouteHandler(std::function<void(AppRoute, bool)> handler);
    void navigateTo(AppRoute route, bool animate);
    void updateBottomNavForRoute(AppRoute route);
    void updateBottomNavForPage(int pageIndex);
    void positionBottomNav();

private:
    BottomNavBar *m_bottomNav = nullptr;
    QStackedWidget *m_stack = nullptr;
    QWidget *m_host = nullptr;
    QHash<int, int> m_routeIndexes;
    std::function<void(AppRoute, bool)> m_routeHandler;
};

#endif // MOBILESHELLCONTROLLER_H
