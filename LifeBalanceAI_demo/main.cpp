#include "mainwindow.h"
#include "databasemanager.h"
#include "thememanager.h"

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QTextStream>
#include <QDir>
#include <QMessageBox>
#include <QWindow>

int main(int argc, char *argv[])
{
#ifdef Q_OS_ANDROID
    qputenv("ANDROID_OPENSSL_SUFFIX", "_3");
#endif

    QApplication a(argc, argv);

    const QStringList embeddedFontPaths = {
        QStringLiteral(":/fonts/MiSans-Regular.otf"),
        QStringLiteral(":/fonts/MiSans-Medium.otf"),
        QStringLiteral(":/fonts/MiSans-Bold.otf"),
        QStringLiteral(":/fonts/SmileySans-Oblique.otf")
    };
    for (const QString &fontPath : embeddedFontPaths) {
        const int fontId = QFontDatabase::addApplicationFont(fontPath);
        if (fontId < 0) {
            qWarning().noquote() << QStringLiteral("[Font] 加载内置字体失败: %1").arg(fontPath);
            continue;
        }
        qInfo().noquote() << QStringLiteral("[Font] 已加载内置字体 %1 -> %2")
                                  .arg(fontPath, QFontDatabase::applicationFontFamilies(fontId).join(QStringLiteral(", ")));
    }

    auto pickFontFamily = [](const QStringList &candidates) {
        const QStringList families = QFontDatabase::families();
        for (const QString &candidate : candidates) {
            for (const QString &family : families) {
                if (family.compare(candidate, Qt::CaseInsensitive) == 0)
                    return family;
            }
        }
        return QStringLiteral("Microsoft YaHei UI");
    };

    const QString bodyFont = pickFontFamily({
        QStringLiteral("MiSans"),
        QStringLiteral("MiSans-Regular"),
        QStringLiteral("MiSans Regular"),
        QStringLiteral("Noto Sans SC"),
        QStringLiteral("Microsoft YaHei UI"),
        QStringLiteral("PingFang SC"),
        QStringLiteral("Segoe UI")
    });
    QFont appFont(bodyFont, 10);
    appFont.setWeight(QFont::Normal);
    appFont.setStyleStrategy(QFont::PreferAntialias);
    appFont.setHintingPreference(QFont::PreferFullHinting);
    a.setFont(appFont);
    qInfo().noquote() << QStringLiteral("[Font] QApplication body font: %1").arg(bodyFont);

    // ── 加载 QSS 主题（优先资源文件，备用本地文件） ─────────────
    {
        QString qssContent;
        bool loaded = false;

        // 方法1：从 Qt 资源文件加载
        {
            QFile qssFile(QStringLiteral(":/style.qss"));
            if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
                QTextStream stream(&qssFile);
                stream.setEncoding(QStringConverter::Utf8);
                qssContent = stream.readAll();
                qssFile.close();
                qInfo().noquote() << QStringLiteral("[QSS] 从资源文件加载成功，大小=%1 字节").arg(qssContent.size());
                loaded = true;
            } else {
                qWarning().noquote() << "[QSS] 资源文件 :/style.qss 加载失败！";
            }
        }

        // 方法2：兼容另一种资源前缀
        if (!loaded) {
            QFile qssFile(QStringLiteral(":/resources/style.qss"));
            if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
                QTextStream stream(&qssFile);
                stream.setEncoding(QStringConverter::Utf8);
                qssContent = stream.readAll();
                qssFile.close();
                qInfo().noquote() << QStringLiteral("[QSS] 从资源文件 :/resources/style.qss 加载成功，大小=%1 字节").arg(qssContent.size());
                loaded = true;
            } else {
                qWarning().noquote() << "[QSS] 资源文件 :/resources/style.qss 加载失败！";
            }
        }

        // 方法3：从本地文件系统加载（兼容开发运行和安装后运行）
        if (!loaded) {
            const QString appDir = QApplication::applicationDirPath();
            const QStringList localCandidates = {
                appDir + QStringLiteral("/resources/style.qss"),
                QDir(appDir).absoluteFilePath(QStringLiteral("../resources/style.qss")),
                QDir(appDir).absoluteFilePath(QStringLiteral("../../resources/style.qss")),
                QDir::current().absoluteFilePath(QStringLiteral("resources/style.qss")),
                QDir::current().absoluteFilePath(QStringLiteral("LifeBalanceAI_demo/resources/style.qss"))
            };

            for (const QString &candidate : localCandidates) {
                const QString localPath = QDir::cleanPath(candidate);
                qWarning().noquote() << QStringLiteral("[QSS] 尝试本地路径: %1").arg(localPath);
                QFile localFile(localPath);
                if (!localFile.open(QFile::ReadOnly | QFile::Text))
                    continue;

                QTextStream stream(&localFile);
                stream.setEncoding(QStringConverter::Utf8);
                qssContent = stream.readAll();
                localFile.close();
                qInfo().noquote() << QStringLiteral("[QSS] 从本地文件加载成功，大小=%1 字节").arg(qssContent.size());
                loaded = true;
                break;
            }
        }

        if (loaded) {
            ThemeManager &themeManager = ThemeManager::instance();
            const QByteArray envTheme = qgetenv("LIFEBALANCE_THEME");
            if (!envTheme.isEmpty())
                themeManager.setTheme(QString::fromUtf8(envTheme));
            if (themeManager.isDarkMode())
                qssContent += QStringLiteral("\n\n/* Dark theme overlay */\n") + themeManager.getStylesheet();

            // 打印前200个字符用于诊断
            qInfo().noquote() << "[QSS] 内容预览(前200字符):";
            qInfo().noquote() << qssContent.left(200);
            a.setStyleSheet(qssContent);
            qInfo().noquote() << "[QSS] 样式表已成功应用到 QApplication。";
        } else {
            qCritical().noquote() << "[QSS] 所有加载方式均失败！样式表未应用。";
        }
    }

    // Initialize database (Singleton)
    if (!DatabaseManager::instance().initDatabase()) {
        qCritical() << "Database initialization failed! Exiting...";
        return -1;
    }
    qInfo() << "Database connection established successfully.";

    MainWindow w;
#ifdef Q_OS_ANDROID
    w.showMaximized();
#else
    w.show();
#endif
    return a.exec();
}
