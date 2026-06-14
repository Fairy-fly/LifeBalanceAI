#include "mobileshellcontroller.h"

#include "bottomnavbar.h"
#include "platformlayoutpolicy.h"

#include <QGridLayout>
#include <QStackedWidget>
#include <QWidget>

#include <utility>

MobileShellController::MobileShellController(QObject *parent)
    : QObject(parent)
{
}

void MobileShellController::setShellWidgets(BottomNavBar *bottomNav, QStackedWidget *stack, QWidget *host)
{
    m_bottomNav = bottomNav;
    m_stack = stack;
    m_host = host;
}

void MobileShellController::setRouteIndex(AppRoute route, int pageIndex)
{
    if (pageIndex >= 0)
        m_routeIndexes.insert(static_cast<int>(route), pageIndex);
    else
        m_routeIndexes.remove(static_cast<int>(route));
}

int MobileShellController::routeIndex(AppRoute route) const
{
    return m_routeIndexes.value(static_cast<int>(route), -1);
}

AppRoute MobileShellController::routeForPageIndex(int pageIndex) const
{
    for (auto it = m_routeIndexes.cbegin(); it != m_routeIndexes.cend(); ++it) {
        if (it.value() == pageIndex)
            return static_cast<AppRoute>(it.key());
    }
    return AppRoute::Unknown;
}

AppRoute MobileShellController::routeForBottomTab(int tab) const
{
    switch (tab) {
    case 0:
        return AppRoute::Home;
    case 1:
        return AppRoute::Analysis;
    case 2:
        return AppRoute::Report;
    case 3:
        return AppRoute::Profile;
    default:
        return AppRoute::Unknown;
    }
}

int MobileShellController::bottomTabForRoute(AppRoute route) const
{
    switch (route) {
    case AppRoute::Home:
        return 0;
    case AppRoute::Analysis:
        return 1;
    case AppRoute::Report:
        return 2;
    case AppRoute::Profile:
        return 3;
    default:
        return -1;
    }
}

bool MobileShellController::isBottomNavRoute(AppRoute route) const
{
    return bottomTabForRoute(route) >= 0;
}

void MobileShellController::setRouteHandler(std::function<void(AppRoute, bool)> handler)
{
    m_routeHandler = std::move(handler);
}

void MobileShellController::navigateTo(AppRoute route, bool animate)
{
    if (m_routeHandler)
        m_routeHandler(route, animate);
}

void MobileShellController::updateBottomNavForRoute(AppRoute route)
{
    if (!m_bottomNav)
        return;

    const int activeTab = bottomTabForRoute(route);
    const bool visible = activeTab >= 0;
    m_bottomNav->setVisible(visible);

    positionBottomNav();

    if (!visible)
        return;

    m_bottomNav->blockSignals(true);
    m_bottomNav->setCurrentIndex(activeTab);
    m_bottomNav->blockSignals(false);
    m_bottomNav->raise();
}

void MobileShellController::updateBottomNavForPage(int pageIndex)
{
    updateBottomNavForRoute(routeForPageIndex(pageIndex));
}

void MobileShellController::positionBottomNav()
{
    if (!m_bottomNav)
        return;

    QWidget *host = m_host;
    if (!host && m_stack)
        host = m_stack->parentWidget();
    if (!host)
        return;

#ifdef Q_OS_ANDROID
    const int bottomInset = LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomSafeAreaInset();
    m_bottomNav->setBottomSafeAreaInset(bottomInset);
    const int navHeight = LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavContentHeight() + bottomInset;

    if (auto *grid = qobject_cast<QGridLayout *>(host->layout())) {
        if (grid->indexOf(m_bottomNav) >= 0)
            grid->removeWidget(m_bottomNav);
        grid->setRowStretch(0, 1);
        grid->setRowStretch(1, 0);
        grid->setRowMinimumHeight(1, 0);
        grid->invalidate();
    }

    if (m_bottomNav->parentWidget() != host)
        m_bottomNav->setParent(host);
    m_bottomNav->setGeometry(0,
                             qMax(0, host->height() - navHeight),
                             host->width(),
                             navHeight);

    if (m_bottomNav->isVisible()) {
        m_bottomNav->show();
        m_bottomNav->raise();
    }
    return;
#else
    const int navHeight = LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavHeight();
    if (auto *grid = qobject_cast<QGridLayout *>(host->layout())) {
        if (m_bottomNav->parentWidget() != host)
            m_bottomNav->setParent(host);
        if (grid->indexOf(m_bottomNav) < 0)
            grid->addWidget(m_bottomNav, 1, 0);
        grid->setRowStretch(0, 1);
        grid->setRowStretch(1, 0);
        grid->setRowMinimumHeight(1, m_bottomNav->isVisible() ? navHeight : 0);
        grid->invalidate();
    } else {
        if (m_bottomNav->parentWidget() != host)
            m_bottomNav->setParent(host);
        m_bottomNav->setGeometry(0, qMax(0, host->height() - navHeight), host->width(), navHeight);
    }

    if (m_bottomNav->isVisible()) {
        m_bottomNav->show();
        m_bottomNav->raise();
    }
#endif
}
