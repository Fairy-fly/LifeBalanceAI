#ifndef DESIGNTOKENS_H
#define DESIGNTOKENS_H

#include <QColor>
#include <QString>

namespace DesignTokens {

inline constexpr int RadiusXs = 4;
inline constexpr int RadiusSm = 6;
inline constexpr int RadiusMd = 8;
inline constexpr int RadiusLg = 12;
inline constexpr int RadiusXl = 16;

inline constexpr int SpaceXs = 4;
inline constexpr int SpaceSm = 8;
inline constexpr int SpaceMd = 12;
inline constexpr int SpaceLg = 16;
inline constexpr int SpaceXl = 24;
inline constexpr int SpaceXxl = 32;

inline constexpr int MotionFast = 150;
inline constexpr int MotionBase = 250;
inline constexpr int MotionSlow = 400;
inline constexpr int MotionSlower = 600;

inline QString color(const char *hex) { return QString::fromLatin1(hex); }

inline QString primaryDark() { return color("#2D7A5E"); }
inline QString primary() { return color("#4CAF7F"); }
inline QString primaryLight() { return color("#6BC48F"); }
inline QString primaryLighter() { return color("#A8E6C3"); }
inline QString primaryLightest() { return color("#E8F8F2"); }
inline QString primaryHover() { return primaryLight(); }
inline QString primaryPressed() { return primaryDark(); }

inline QString accentDark() { return color("#D97E3A"); }
inline QString accent() { return color("#F4A261"); }
inline QString accentLight() { return color("#F8B88D"); }
inline QString accentSoft() { return color("#FFF1E6"); }
inline QString accentLightest() { return accentSoft(); }

inline QString blue() { return color("#6BA3D4"); }
inline QString blueLight() { return color("#A8C8E1"); }
inline QString blueSoft() { return color("#EEF4F9"); }

inline QString success() { return color("#52C41A"); }
inline QString successBg() { return color("#F6FFED"); }
inline QString successBorder() { return color("#B7EB8F"); }
inline QString active() { return color("#FFB84D"); }
inline QString activeBg() { return color("#FFFBE6"); }
inline QString activeBorder() { return color("#FFD591"); }
inline QString rest() { return color("#A0AEC0"); }
inline QString restBg() { return color("#F2F5FB"); }
inline QString restBorder() { return color("#D1D9E8"); }
inline QString warning() { return color("#FF7875"); }
inline QString warningBg() { return color("#FFF1F0"); }
inline QString warningBorder() { return color("#FFCCCB"); }
inline QString disabled() { return color("#BFBFBF"); }
inline QString disabledBg() { return color("#F5F5F5"); }

inline QString textTitle() { return color("#1A1A1A"); }
inline QString textBody() { return color("#333333"); }
inline QString textMuted() { return color("#666666"); }
inline QString textTertiary() { return color("#999999"); }
inline QString textDisabled() { return color("#CCCCCC"); }

inline QString bgWindow() { return color("#FBF6EF"); }
inline QString bgPage() { return color("#FBF6EF"); }
inline QString bgCard() { return color("#FEFEFE"); }
inline QString bgSoft() { return color("#F5F5F5"); }
inline QString bgLighter() { return color("#EFEFEF"); }

inline QString border() { return color("#E8E8E8"); }
inline QString borderLight() { return color("#F0F0F0"); }
inline QString borderStrong() { return color("#D9D9D9"); }
inline QString divider() { return borderLight(); }
inline QString danger() { return warning(); }

inline QString dietColor() { return color("#FF6B6B"); }
inline QString exerciseColor() { return color("#4ECDC4"); }
inline QString sleepColor() { return color("#6C5CE7"); }
inline QString mindfulnessColor() { return color("#A29BFE"); }
inline QString socialColor() { return color("#FD79A8"); }
inline QString workColor() { return color("#74B9FF"); }

// Extended color palette for semantic meanings
inline QString errorDark() { return color("#C41D3D"); }
inline QString error() { return warning(); }
inline QString errorLight() { return color("#FFB3BA"); }
inline QString errorLighter() { return color("#FFF1F0"); }

inline QString infoDark() { return color("#0066CC"); }
inline QString info() { return blue(); }
inline QString infoLight() { return blueLight(); }
inline QString infoLighter() { return blueSoft(); }

// Health dimension extended palette
inline QString dietDark() { return color("#D63031"); }
inline QString dietLight() { return color("#FF8C92"); }
inline QString dietLighter() { return color("#FFB3BA"); }
inline QString dietLightest() { return color("#FFF0F1"); }

inline QString exerciseDark() { return color("#00A896"); }
inline QString exerciseLight() { return color("#76D7D7"); }
inline QString exerciseLighter() { return color("#B0E0E0"); }
inline QString exerciseLightest() { return color("#E0F7F7"); }

inline QString sleepDark() { return color("#5849A0"); }
inline QString sleepLight() { return color("#8B7FC0"); }
inline QString sleepLighter() { return color("#B3A8D8"); }
inline QString sleepLightest() { return color("#E8E5F5"); }

inline QString mindfulnessDark() { return color("#7654A6"); }
inline QString mindfulnessLight() { return color("#C4AADB"); }
inline QString mindfulnessLighter() { return color("#D9CCEB"); }
inline QString mindfulnessLightest() { return color("#F0E8F8"); }

inline QString socialDark() { return color("#E64980"); }
inline QString socialLight() { return color("#F5A3C9"); }
inline QString socialLighter() { return color("#F9C8E0"); }
inline QString socialLightest() { return color("#FEE8F3"); }

inline QString workDark() { return color("#0084FF"); }
inline QString workLight() { return color("#66AAFF"); }
inline QString workLighter() { return color("#99CCFF"); }
inline QString workLightest() { return color("#E6F3FF"); }

// Semantic status colors with extended range
inline QString successDark() { return color("#2E7D32"); }
inline QString successLight() { return color("#66BB6A"); }
inline QString successLighter() { return color("#A5D6A7"); }
inline QString successLightest() { return color("#E8F5E9"); }

inline QString activeDark() { return color("#FF8C00"); }
inline QString activeLight() { return color("#FFD54F"); }
inline QString activeLighter() { return color("#FFE082"); }
inline QString activeLightest() { return color("#FFFBEA"); }

inline QString warningDark() { return color("#D32F2F"); }
inline QString warningLight() { return color("#EF9A9A"); }
inline QString warningLighter() { return color("#FFCDD2"); }
inline QString warningLightest() { return color("#FFEBEE"); }

inline QString restDark() { return color("#607D8B"); }
inline QString restLight() { return color("#CFD8DC"); }
inline QString restLighter() { return color("#ECEFF1"); }
inline QString restLightest() { return color("#F5F7F9"); }

// Neutral grayscale palette for flexible styling
inline QString gray900() { return color("#0F1419"); }
inline QString gray800() { return color("#1A1A1A"); }
inline QString gray700() { return color("#333333"); }
inline QString gray600() { return color("#666666"); }
inline QString gray500() { return color("#999999"); }
inline QString gray400() { return color("#D9D9D9"); }
inline QString gray300() { return color("#E8E8E8"); }
inline QString gray200() { return color("#F0F0F0"); }
inline QString gray100() { return color("#F5F5F5"); }
inline QString gray50() { return color("#FAFAFA"); }

// Primary color extended palette
inline QString primaryXdark() { return color("#1B5542"); }
inline QString primaryDarker() { return color("#245A4B"); }
// primaryDark is already defined
// primary is already defined
// primaryLight is already defined
// primaryLighter is already defined
inline QString primaryVeryLight() { return color("#C8F0E8"); }
inline QString primaryExtraLight() { return color("#E8F8F2"); }
inline QString primaryUltraLight() { return color("#F0FCFA"); }

// Shadow and overlay colors for depth
inline QString shadowXs() { return color("rgba(0, 0, 0, 0.08)"); }
inline QString shadowSm() { return color("rgba(0, 0, 0, 0.12)"); }
inline QString shadowMd() { return color("rgba(0, 0, 0, 0.16)"); }
inline QString shadowLg() { return color("rgba(0, 0, 0, 0.20)"); }
inline QString shadowXl() { return color("rgba(0, 0, 0, 0.24)"); }

// Overlay for modals and drawers
inline QString overlayDim() { return color("rgba(251, 246, 239, 0.6)"); }
inline QString overlayDark() { return color("rgba(0, 0, 0, 0.45)"); }

// Badge and achievement colors
inline QString badgeGold() { return color("#FFD700"); }
inline QString badgeSilver() { return color("#C0C0C0"); }
inline QString badgeBronze() { return color("#CD7F32"); }

inline QString bodyFontStack()
{
    return QStringLiteral("\"MiSans\", \"MiSans-Regular\", \"MiSans Regular\", \"Noto Sans SC\", \"Microsoft YaHei UI\", \"PingFang SC\", \"Segoe UI\", sans-serif");
}

inline QString mediumFontStack()
{
    return QStringLiteral("\"MiSans Medium\", \"MiSans\", \"Noto Sans SC\", \"Microsoft YaHei UI\", \"PingFang SC\", sans-serif");
}

inline QString boldFontStack()
{
    return QStringLiteral("\"MiSans Bold\", \"MiSans\", \"Noto Sans SC\", \"Microsoft YaHei UI\", \"PingFang SC\", sans-serif");
}

inline QString brandFontStack()
{
    return QStringLiteral("\"Smiley Sans Oblique\", \"Smiley Sans\", \"MiSans Bold\", \"MiSans\", \"Noto Sans SC\", sans-serif");
}

// ============ DARK MODE THEME ============
namespace DarkMode {

// Dark background palette
inline QString bgWindow() { return color("#0F1419"); }
inline QString bgPage() { return color("#0F1419"); }
inline QString bgCard() { return color("#1A1D26"); }
inline QString bgSoft() { return color("#252A33"); }
inline QString bgLighter() { return color("#2A2D35"); }

// Dark text palette
inline QString textTitle() { return color("#F5F5F5"); }
inline QString textBody() { return color("#E0E0E0"); }
inline QString textMuted() { return color("#A0A0A0"); }
inline QString textTertiary() { return color("#707070"); }
inline QString textDisabled() { return color("#505050"); }

// Dark borders
inline QString border() { return color("#3A3F48"); }
inline QString borderLight() { return color("#2A2D35"); }
inline QString borderStrong() { return color("#4A5058"); }
inline QString divider() { return borderLight(); }

// Primary colors (brightened for dark mode)
inline QString primaryDark() { return color("#2D7A5E"); }
inline QString primary() { return color("#5EC9A1"); }
inline QString primaryLight() { return color("#7EDAB2"); }
inline QString primaryLighter() { return color("#9DE5C8"); }
inline QString primaryLightest() { return color("#1B4033"); }

// Semantic colors (adapted for dark mode)
inline QString success() { return color("#85CE61"); }
inline QString successBg() { return color("#274916"); }
inline QString successBorder() { return color("#3D6B2F"); }

inline QString active() { return color("#FFC069"); }
inline QString activeBg() { return color("#3D3409"); }
inline QString activeBorder() { return color("#664D1A"); }

inline QString warning() { return color("#FF9C6E"); }
inline QString warningBg() { return color("#3D2416"); }
inline QString warningBorder() { return color("#664033"); }

inline QString error() { return color("#FF7875"); }
inline QString errorBg() { return color("#3D1F1F"); }

inline QString rest() { return color("#91D5FF"); }
inline QString restBg() { return color("#1A3A4D"); }
inline QString restBorder() { return color("#2F5A73"); }

// Health dimensions (brightened for dark mode)
inline QString dietColor() { return color("#FF7A7A"); }
inline QString dietLight() { return color("#FF9999"); }

inline QString exerciseColor() { return color("#5FE3D0"); }
inline QString exerciseLight() { return color("#7FEAE2"); }

inline QString sleepColor() { return color("#9277F8"); }
inline QString sleepLight() { return color("#AE94FF"); }

inline QString mindfulnessColor() { return color("#B8A3F8"); }
inline QString socialColor() { return color("#FF89B6"); }
inline QString workColor() { return color("#85D1FF"); }

// Accent colors
inline QString accent() { return color("#FFB84D"); }
inline QString accentLight() { return color("#FFC869"); }

inline QString disabled() { return color("#555555"); }
inline QString disabledBg() { return color("#262B32"); }

} // namespace DarkMode

} // namespace DesignTokens

#endif // DESIGNTOKENS_H
