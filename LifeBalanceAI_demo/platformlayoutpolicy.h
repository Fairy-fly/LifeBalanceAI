#ifndef PLATFORMLAYOUTPOLICY_H
#define PLATFORMLAYOUTPOLICY_H

#include <QSize>

class QWidget;

namespace LifeBalanceAI {
namespace Ui {

class PlatformLayoutPolicy
{
public:
    static bool isMobileRuntime();
    static QSize availableScreenSize();
    static int bottomSafeAreaInset();
    static int bottomNavContentHeight();
    static int bottomNavHeight();
    static int bottomNavPagePadding();
    static void applyMobileScrollPolicy(QWidget *root);
    static void normalizeHorizontalPosition(QWidget *root);
};

} // namespace Ui
} // namespace LifeBalanceAI

#endif // PLATFORMLAYOUTPOLICY_H
