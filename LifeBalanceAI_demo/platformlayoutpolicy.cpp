#include "platformlayoutpolicy.h"

#include <QAbstractScrollArea>
#include <QGuiApplication>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QScrollBar>
#include <QScroller>
#include <QScrollerProperties>
#include <QSizePolicy>
#include <QTableWidget>
#include <QTextEdit>
#include <QWidget>

namespace LifeBalanceAI {
namespace Ui {

bool PlatformLayoutPolicy::isMobileRuntime()
{
#ifdef Q_OS_ANDROID
    return true;
#else
    return false;
#endif
}

QSize PlatformLayoutPolicy::availableScreenSize()
{
    if (QScreen *screen = QGuiApplication::primaryScreen())
        return screen->availableGeometry().size();
    return {};
}

int PlatformLayoutPolicy::bottomNavHeight()
{
#ifdef Q_OS_ANDROID
    return 64 + bottomSafeAreaInset();
#else
    return 60;
#endif
}

int PlatformLayoutPolicy::bottomSafeAreaInset()
{
#ifdef Q_OS_ANDROID
    return 48;
#else
    return 0;
#endif
}

void PlatformLayoutPolicy::applyMobileScrollPolicy(QWidget *root)
{
    if (!root || !isMobileRuntime())
        return;

    root->setMinimumWidth(0);
    if (root->maximumWidth() < QWIDGETSIZE_MAX)
        root->setMaximumWidth(QWIDGETSIZE_MAX);

    const auto widgets = root->findChildren<QWidget *>();
    for (QWidget *widget : widgets) {
        if (!widget)
            continue;

        widget->setMinimumWidth(0);
        if (widget->maximumWidth() < QWIDGETSIZE_MAX &&
            !qobject_cast<QPushButton *>(widget)) {
            widget->setMaximumWidth(QWIDGETSIZE_MAX);
        }

        QSizePolicy policy = widget->sizePolicy();
        if (!qobject_cast<QLabel *>(widget) &&
            !qobject_cast<QPushButton *>(widget) &&
            !qobject_cast<QScrollBar *>(widget)) {
            policy.setHorizontalPolicy(QSizePolicy::Expanding);
            widget->setSizePolicy(policy);
        }
    }

    const auto scrollAreas = root->findChildren<QAbstractScrollArea *>();
    for (QAbstractScrollArea *area : scrollAreas) {
        if (!area || !area->viewport())
            continue;

        area->setAttribute(Qt::WA_AcceptTouchEvents, true);
        area->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        if (!qobject_cast<QTextEdit *>(area) &&
            !qobject_cast<QPlainTextEdit *>(area) &&
            !qobject_cast<QTableWidget *>(area)) {
            area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }

        if (QScrollBar *bar = area->verticalScrollBar()) {
            bar->setFixedWidth(0);
            bar->hide();
        }

        QScroller::grabGesture(area->viewport(), QScroller::TouchGesture);
        QScroller *scroller = QScroller::scroller(area->viewport());
        QScrollerProperties properties = scroller->scrollerProperties();
        properties.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.16);
        properties.setScrollMetric(QScrollerProperties::DecelerationFactor, 1.65);
        properties.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.18);
        properties.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
        properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy,
                                   QScrollerProperties::OvershootAlwaysOff);
        properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy,
                                   QScrollerProperties::OvershootAlwaysOff);
        scroller->setScrollerProperties(properties);
    }
}

void PlatformLayoutPolicy::normalizeHorizontalPosition(QWidget *root)
{
    if (!root || !isMobileRuntime())
        return;

    const auto areas = root->findChildren<QAbstractScrollArea *>();
    for (QAbstractScrollArea *area : areas) {
        if (!area || qobject_cast<QTableWidget *>(area))
            continue;

        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if (QScrollBar *bar = area->horizontalScrollBar())
            bar->setValue(0);

        auto *scrollArea = qobject_cast<QScrollArea *>(area);
        if (!scrollArea || !scrollArea->widget() || !scrollArea->viewport())
            continue;

        const int viewportWidth = scrollArea->viewport()->width();
        if (viewportWidth <= 0)
            continue;

        QWidget *content = scrollArea->widget();
        content->setMinimumWidth(0);
        content->setMaximumWidth(viewportWidth);
        content->resize(viewportWidth, qMax(content->height(), content->sizeHint().height()));
    }
}

} // namespace Ui
} // namespace LifeBalanceAI
