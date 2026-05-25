#include "thememanager.h"
#include <QSettings>
#include <QApplication>
#include <QStyleFactory>

ThemeManager &ThemeManager::instance()
{
    static ThemeManager s_instance;
    return s_instance;
}

ThemeManager::ThemeManager()
{
    initializeColorMaps();
    loadThemePreference();
}

QString ThemeManager::themeName() const
{
    return m_currentTheme == Light ? "Light" : "Dark";
}

void ThemeManager::setTheme(const QString &themeName)
{
    if (themeName == "Dark") {
        setTheme(Dark);
    } else {
        setTheme(Light);
    }
}

void ThemeManager::setTheme(Theme theme)
{
    if (m_currentTheme != theme) {
        m_currentTheme = theme;
        saveThemePreference();
        emit themeChanged(themeName());

        // Update application stylesheet
        qApp->setStyle(QStyleFactory::create("Fusion"));
    }
}

QString ThemeManager::bgWindow()
{
    return m_currentTheme == Light ? m_lightColors["bgWindow"] : m_darkColors["bgWindow"];
}

QString ThemeManager::bgCard()
{
    return m_currentTheme == Light ? m_lightColors["bgCard"] : m_darkColors["bgCard"];
}

QString ThemeManager::bgSoft()
{
    return m_currentTheme == Light ? m_lightColors["bgSoft"] : m_darkColors["bgSoft"];
}

QString ThemeManager::textTitle()
{
    return m_currentTheme == Light ? m_lightColors["textTitle"] : m_darkColors["textTitle"];
}

QString ThemeManager::textBody()
{
    return m_currentTheme == Light ? m_lightColors["textBody"] : m_darkColors["textBody"];
}

QString ThemeManager::textMuted()
{
    return m_currentTheme == Light ? m_lightColors["textMuted"] : m_darkColors["textMuted"];
}

QString ThemeManager::textTertiary()
{
    return m_currentTheme == Light ? m_lightColors["textTertiary"] : m_darkColors["textTertiary"];
}

QString ThemeManager::border()
{
    return m_currentTheme == Light ? m_lightColors["border"] : m_darkColors["border"];
}

QString ThemeManager::borderLight()
{
    return m_currentTheme == Light ? m_lightColors["borderLight"] : m_darkColors["borderLight"];
}

QString ThemeManager::borderStrong()
{
    return m_currentTheme == Light ? m_lightColors["borderStrong"] : m_darkColors["borderStrong"];
}

QString ThemeManager::primary()
{
    // Primary color stays consistent
    return "#4CAF7F";
}

QString ThemeManager::primaryLight()
{
    return "#6BC48F";
}

QString ThemeManager::primaryDark()
{
    return "#2D7A5E";
}

QString ThemeManager::success()
{
    return m_currentTheme == Light ? "#52C41A" : "#85CE61";
}

QString ThemeManager::warning()
{
    return m_currentTheme == Light ? "#FF7875" : "#FF9C6E";
}

QString ThemeManager::error()
{
    return m_currentTheme == Light ? "#FF7875" : "#FF9C6E";
}

QString ThemeManager::info()
{
    return m_currentTheme == Light ? "#6BA3D4" : "#91D5FF";
}

QString ThemeManager::getStylesheet()
{
    if (m_currentTheme == Light) {
        // Light theme uses existing style.qss
        return "";
    } else {
        // Dark mode stylesheet
        return QString(
            "QWidget { background: %1; color: %2; }"
            "QMainWindow { background: %1; }"
            "QDialog { background: %1; }"
            "QFrame, QGroupBox { background: %3; border: 1px solid %4; }"
            "QLabel { color: %2; }"
            "QLineEdit, QTextEdit, QPlainTextEdit, QSpinBox, QComboBox {"
            "  background: %3; color: %2; border: 1px solid %4; }"
            "QPushButton { background: %3; color: %2; border: 1px solid %4; }"
            "QPushButton:hover { background: %5; }"
            "QTabBar::tab { background: %5; color: %6; }"
            "QTabBar::tab:selected { background: %3; color: %2; }"
            "QProgressBar { background: %5; }"
            "QProgressBar::chunk { background: #4CAF7F; }"
            "QScrollBar { background: %5; }"
            "QTableWidget { background: %3; color: %2; gridline-color: %4; }"
        ).arg(m_darkColors["bgWindow"], m_darkColors["textBody"],
              m_darkColors["bgCard"], m_darkColors["border"],
              m_darkColors["bgSoft"], m_darkColors["textMuted"]);
    }
}

void ThemeManager::loadThemePreference()
{
    QSettings settings("LifeBalanceAI", "LifeBalanceAI");
    QString savedTheme = settings.value("theme", "Light").toString();
    setTheme(savedTheme);
}

void ThemeManager::saveThemePreference()
{
    QSettings settings("LifeBalanceAI", "LifeBalanceAI");
    settings.setValue("theme", themeName());
}

void ThemeManager::initializeColorMaps()
{
    // Light theme colors
    m_lightColors["bgWindow"] = "#FBF6EF";
    m_lightColors["bgCard"] = "#FEFEFE";
    m_lightColors["bgSoft"] = "#F5F5F5";
    m_lightColors["textTitle"] = "#1A1A1A";
    m_lightColors["textBody"] = "#333333";
    m_lightColors["textMuted"] = "#666666";
    m_lightColors["textTertiary"] = "#999999";
    m_lightColors["border"] = "#E8E8E8";
    m_lightColors["borderLight"] = "#F0F0F0";
    m_lightColors["borderStrong"] = "#D9D9D9";

    // Dark theme colors - carefully chosen for readability
    m_darkColors["bgWindow"] = "#0F1419";      // Very dark background
    m_darkColors["bgCard"] = "#1A1D26";        // Dark card background
    m_darkColors["bgSoft"] = "#252A33";        // Slightly lighter for contrast
    m_darkColors["textTitle"] = "#F5F5F5";     // Near white for titles
    m_darkColors["textBody"] = "#E0E0E0";      // Light gray for body text
    m_darkColors["textMuted"] = "#A0A0A0";     // Medium gray for muted text
    m_darkColors["textTertiary"] = "#707070";  // Darker gray for tertiary text
    m_darkColors["border"] = "#3A3F48";        // Dark border
    m_darkColors["borderLight"] = "#2A2D35";   // Lighter dark border
    m_darkColors["borderStrong"] = "#4A5058";  // Stronger dark border
}

QString ThemeManager::getDarkColor(const QString &lightColor)
{
    // Helper to convert light colors to dark theme equivalents
    // This is a utility function for custom component styling
    if (lightColor == "#FBF6EF") return m_darkColors["bgWindow"];
    if (lightColor == "#FEFEFE") return m_darkColors["bgCard"];
    if (lightColor == "#F5F5F5") return m_darkColors["bgSoft"];
    return lightColor;  // Default to original if not mapped
}

QString ThemeManager::getLightColor(const QString &darkColor)
{
    // Helper to convert dark colors to light theme equivalents
    if (darkColor == "#0F1419") return m_lightColors["bgWindow"];
    if (darkColor == "#1A1D26") return m_lightColors["bgCard"];
    if (darkColor == "#252A33") return m_lightColors["bgSoft"];
    return darkColor;
}
