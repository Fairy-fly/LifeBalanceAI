#include "platformlayoutpolicy.h"

#include <QAbstractScrollArea>
#include <QCoreApplication>
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

#ifdef Q_OS_ANDROID
#include <QJniObject>
#include <QtCore/qcoreapplication_platform.h>
#endif

namespace LifeBalanceAI {
namespace Ui {

namespace {

#ifdef Q_OS_ANDROID
int androidWindowBottomInset()
{
    if (!QNativeInterface::QAndroidApplication::isActivityContext())
        return 0;

    QJniObject activity(QNativeInterface::QAndroidApplication::context());
    if (!activity.isValid())
        return 0;

    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (!window.isValid())
        return 0;

    QJniObject decorView = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
    if (!decorView.isValid())
        return 0;

    QJniObject insets = decorView.callObjectMethod("getRootWindowInsets", "()Landroid/view/WindowInsets;");
    if (!insets.isValid())
        return 0;

    const int stableBottom = insets.callMethod<jint>("getStableInsetBottom", "()I");
    const int systemBottom = insets.callMethod<jint>("getSystemWindowInsetBottom", "()I");
    return qMax(stableBottom, systemBottom);
}

int androidWindowTopInset()
{
    if (!QNativeInterface::QAndroidApplication::isActivityContext())
        return 0;

    QJniObject activity(QNativeInterface::QAndroidApplication::context());
    if (!activity.isValid())
        return 0;

    QJniObject window = activity.callObjectMethod("getWindow", "()Landroid/view/Window;");
    if (!window.isValid())
        return 0;

    QJniObject decorView = window.callObjectMethod("getDecorView", "()Landroid/view/View;");
    if (!decorView.isValid())
        return 0;

    QJniObject insets = decorView.callObjectMethod("getRootWindowInsets", "()Landroid/view/WindowInsets;");
    if (!insets.isValid())
        return 0;

    const int stableTop = insets.callMethod<jint>("getStableInsetTop", "()I");
    const int systemTop = insets.callMethod<jint>("getSystemWindowInsetTop", "()I");
    return qMax(stableTop, systemTop);
}
#endif

QRect screenGeometryRect()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
        return QRect(0, 0, 390, 760);
#ifdef Q_OS_ANDROID
    return screen->geometry();
#else
    return screen->availableGeometry();
#endif
}

int screenGeometryTopInset()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
        return 0;

    const QRect geometry = screen->geometry();
    const QRect available = screen->availableGeometry();
    return qMax(0, available.y() - geometry.y());
}

int screenGeometryBottomInset()
{
    QScreen *screen = QGuiApplication::primaryScreen();
    if (!screen)
        return 0;

    const QRect geometry = screen->geometry();
    const QRect available = screen->availableGeometry();
    return qMax(0, geometry.y() + geometry.height() - (available.y() + available.height()));
}

int topSafeAreaInset()
{
#ifdef Q_OS_ANDROID
    int inset = androidWindowTopInset();
    if (inset <= 0)
        inset = screenGeometryTopInset();
    return qBound(0, inset, 180);
#else
    return 0;
#endif
}

} // namespace

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
    return screenGeometryRect().size();
}

QRect PlatformLayoutPolicy::safeContentRect(int margin)
{
    QRect rect = screenGeometryRect();
    const int safeMargin = qMax(0, margin);
#ifdef Q_OS_ANDROID
    rect.adjust(safeMargin,
                topSafeAreaInset() + safeMargin,
                -safeMargin,
                -(bottomSafeAreaInset() + safeMargin));
#else
    rect.adjust(safeMargin, safeMargin, -safeMargin, -safeMargin);
#endif
    if (rect.width() <= 0 || rect.height() <= 0)
        return screenGeometryRect();
    return rect;
}

QRect PlatformLayoutPolicy::dialogAvailableRect(int margin)
{
    return safeContentRect(margin);
}

void PlatformLayoutPolicy::centerWidgetOnSafeArea(QWidget *widget, int margin)
{
    if (!widget)
        return;

    QRect available = dialogAvailableRect(margin);
    if (widget->width() <= 0 || widget->height() <= 0)
        widget->adjustSize();

    QSize size = widget->size();
    if (size.width() > available.width() || size.height() > available.height()) {
        size.setWidth(qMin(size.width(), available.width()));
        size.setHeight(qMin(size.height(), available.height()));
        widget->resize(size);
    }

    widget->move(available.x() + (available.width() - widget->width()) / 2,
                 available.y() + (available.height() - widget->height()) / 2);
}

int PlatformLayoutPolicy::bottomSafeAreaInset()
{
#ifdef Q_OS_ANDROID
    int inset = androidWindowBottomInset();
    if (inset <= 0)
        inset = screenGeometryBottomInset();
    if (inset <= 0)
        inset = 48;
    return qBound(0, inset, 240);
#else
    return 0;
#endif
}

int PlatformLayoutPolicy::bottomNavContentHeight()
{
#ifdef Q_OS_ANDROID
    return 64;
#else
    return 60;
#endif
}

int PlatformLayoutPolicy::bottomNavHeight()
{
    return bottomNavContentHeight() + bottomSafeAreaInset();
}

int PlatformLayoutPolicy::bottomNavPagePadding()
{
#ifdef Q_OS_ANDROID
    return bottomNavHeight() + 12;
#else
    return bottomNavHeight();
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
