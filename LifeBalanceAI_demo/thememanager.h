#ifndef THEMEMANAGER_H
#define THEMEMANAGER_H

#include <QString>
#include <QObject>
#include <QMap>

class ThemeManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString themeName READ themeName WRITE setTheme NOTIFY themeChanged)

public:
    enum Theme {
        Light,
        Dark
    };

    static ThemeManager &instance();

    QString themeName() const;
    void setTheme(const QString &themeName);
    void setTheme(Theme theme);

    // Theme-aware color getters
    QString bgWindow();
    QString bgCard();
    QString bgSoft();
    QString textTitle();
    QString textBody();
    QString textMuted();
    QString textTertiary();
    QString border();
    QString borderLight();
    QString borderStrong();

    // Primary colors (same across themes)
    QString primary();
    QString primaryLight();
    QString primaryDark();

    // Semantic colors (adaptive)
    QString success();
    QString warning();
    QString error();
    QString info();

    // Get current theme
    Theme currentTheme() const { return m_currentTheme; }
    bool isDarkMode() const { return m_currentTheme == Dark; }

    // Theme CSS stylesheet
    QString getStylesheet();

    // Persistence
    void loadThemePreference();
    void saveThemePreference();

signals:
    void themeChanged(const QString &themeName);

private:
    explicit ThemeManager();

    QString getDarkColor(const QString &lightColor);
    QString getLightColor(const QString &darkColor);

    Theme m_currentTheme = Light;
    QMap<QString, QString> m_lightColors;
    QMap<QString, QString> m_darkColors;

    void initializeColorMaps();
};

#endif // THEMEMANAGER_H
