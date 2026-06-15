#ifndef PLATFORMLAYOUTPOLICY_H
#define PLATFORMLAYOUTPOLICY_H

#include <QRect>
#include <QSize>

class QWidget;

namespace LifeBalanceAI {
namespace Ui {

class PlatformLayoutPolicy
{
public:
    enum class DialogRole {
        Alert,
        Input,
        LargeContent
    };

    static bool isMobileRuntime();
    static QSize availableScreenSize();
    static void applyAndroidEdgeToEdge();
    static int topSafeAreaInset();
    static QRect safeContentRect(int margin = 0);
    static QRect dialogAvailableRect(int margin = 18);
    static QSize dialogSizeForRole(DialogRole role, const QSize &contentHint = QSize());
    static void centerWidgetOnSafeArea(QWidget *widget, int margin = 18);
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
