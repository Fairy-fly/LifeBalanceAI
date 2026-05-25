#include "uifactory.h"
#include "designtokens.h"

#include <QComboBox>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFrame>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QStyle>
#include <QStringList>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHeaderView>

namespace UiFactory {

void setClass(QWidget *widget, const QString &className)
{
    if (!widget)
        return;
    widget->setProperty("class", className);
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}

QPixmap loadPixmap(const QString &resourcePath)
{
    QPixmap pix(resourcePath);
    if (!pix.isNull())
        return pix;

    QString relative = resourcePath;
    if (relative.startsWith(QStringLiteral(":/")))
        relative.remove(0, 2);
    while (relative.startsWith(QLatin1Char('/')))
        relative.remove(0, 1);

    const QString appDir = QCoreApplication::applicationDirPath();
    const QStringList candidates = {
        QDir(appDir).filePath(QStringLiteral("resources/") + relative),
        QDir(appDir).filePath(QStringLiteral("../resources/") + relative),
        QDir(appDir).filePath(QStringLiteral("../../resources/") + relative),
        QDir::current().filePath(QStringLiteral("resources/") + relative),
        QDir::current().filePath(QStringLiteral("LifeBalanceAI_demo/resources/") + relative)
    };

    for (const QString &candidate : candidates) {
        QFileInfo info(candidate);
        if (!info.exists() || !info.isFile())
            continue;
        pix.load(info.absoluteFilePath());
        if (!pix.isNull())
            return pix;
    }

    return pix;
}

QLabel *assetLabel(const QString &resourcePath, int minHeight, QWidget *parent)
{
    auto *label = new QLabel(parent);
    label->setObjectName(QStringLiteral("assetLabel"));
    label->setAlignment(Qt::AlignCenter);
    label->setMinimumHeight(minHeight);
#ifdef Q_OS_ANDROID
    label->setMinimumWidth(0);
    label->setMaximumWidth(QWIDGETSIZE_MAX);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
#else
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
#endif
    QPixmap pix = loadPixmap(resourcePath);
    if (!pix.isNull()) {
#ifdef Q_OS_ANDROID
        label->setPixmap(pix.scaled(QSize(320, minHeight), Qt::KeepAspectRatio, Qt::SmoothTransformation));
#else
        label->setPixmap(pix.scaledToHeight(minHeight, Qt::SmoothTransformation));
#endif
    }
    return label;
}

QWidget *emptyState(const QString &assetPath, const QString &title, const QString &body, QWidget *parent)
{
    auto *box = new QFrame(parent);
    box->setObjectName(QStringLiteral("emptyState"));
    setClass(box, QStringLiteral("warmCard"));

    auto *layout = new QVBoxLayout(box);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(8);

    layout->addWidget(assetLabel(assetPath, 108, box));

    auto *titleLabel = new QLabel(title, box);
    titleLabel->setObjectName(QStringLiteral("emptyTitle"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setWordWrap(true);
    layout->addWidget(titleLabel);

    auto *bodyLabel = new QLabel(body, box);
    bodyLabel->setObjectName(QStringLiteral("emptyBody"));
    bodyLabel->setAlignment(Qt::AlignCenter);
    bodyLabel->setWordWrap(true);
    layout->addWidget(bodyLabel);

    return box;
}

QPushButton *primaryButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    setClass(button, QStringLiteral("primary"));
    button->setMinimumHeight(44);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QPushButton *secondaryButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    setClass(button, QStringLiteral("secondary"));
    button->setMinimumHeight(40);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QPushButton *ghostButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    setClass(button, QStringLiteral("ghost"));
    button->setMinimumHeight(36);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QPushButton *dangerButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    setClass(button, QStringLiteral("danger"));
    button->setMinimumHeight(40);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QPushButton *smallButton(const QString &text, QWidget *parent)
{
    auto *button = new QPushButton(text, parent);
    button->setMinimumHeight(32);
    button->setCursor(Qt::PointingHandCursor);
    return button;
}

QLineEdit *textInput(const QString &placeholder, QWidget *parent)
{
    auto *input = new QLineEdit(parent);
    input->setPlaceholderText(placeholder);
    input->setMinimumHeight(40);
    return input;
}

QWidget *infoCard(const QString &title, const QString &body, QWidget *parent)
{
    auto *card = new QFrame(parent);
    card->setObjectName(QStringLiteral("infoCard"));
    card->setProperty("class", QStringLiteral("warmCard"));
    card->setMinimumWidth(0);
#ifdef Q_OS_ANDROID
    card->setMaximumWidth(QWIDGETSIZE_MAX);
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
#else
    card->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
#endif
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(8);

    auto *titleLabel = new QLabel(title, card);
    titleLabel->setObjectName(QStringLiteral("emptyTitle"));
    titleLabel->setWordWrap(true);
    titleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(titleLabel);

    auto *bodyLabel = new QLabel(body, card);
    bodyLabel->setObjectName(QStringLiteral("emptyBody"));
    bodyLabel->setWordWrap(true);
    bodyLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(bodyLabel);
    return card;
}

QWidget *healthCard(const QString &title, const QString &body, const QString &accentColor, QWidget *parent)
{
    auto *card = qobject_cast<QFrame *>(infoCard(title, body, parent));
    if (!card)
        return nullptr;

    card->setStyleSheet(QStringLiteral(
        "QFrame{background:%1;border:1px solid %2;border-left:4px solid %3;border-radius:%4px;}")
        .arg(DesignTokens::bgCard(), DesignTokens::border(), accentColor)
        .arg(DesignTokens::RadiusMd));
    return card;
}

void applyWarmPolish(QWidget *root)
{
    if (!root)
        return;

    for (auto *group : root->findChildren<QGroupBox *>()) {
        group->setProperty("class", QStringLiteral("warmCard"));
    }
    for (auto *button : root->findChildren<QPushButton *>()) {
        if (button->minimumHeight() < 32)
            button->setMinimumHeight(32);
        button->setCursor(Qt::PointingHandCursor);
    }
    for (auto *edit : root->findChildren<QLineEdit *>()) {
        if (edit->minimumHeight() < 40)
            edit->setMinimumHeight(40);
    }
    for (auto *combo : root->findChildren<QComboBox *>()) {
        if (combo->minimumHeight() < 40)
            combo->setMinimumHeight(40);
    }
    for (auto *text : root->findChildren<QTextEdit *>()) {
        text->setMinimumHeight(qMax(text->minimumHeight(), 68));
    }
    for (auto *table : root->findChildren<QTableWidget *>()) {
        table->setAlternatingRowColors(true);
        table->verticalHeader()->setDefaultSectionSize(34);
        table->horizontalHeader()->setStretchLastSection(true);
    }

    const auto children = root->findChildren<QWidget *>();
    for (auto *child : children) {
        if (qobject_cast<QLabel *>(child) ||
            qobject_cast<QPushButton *>(child) ||
            qobject_cast<QLineEdit *>(child) ||
            qobject_cast<QTextEdit *>(child) ||
            qobject_cast<QComboBox *>(child) ||
            qobject_cast<QGroupBox *>(child) ||
            qobject_cast<QTableWidget *>(child)) {
            child->setStyleSheet(QString());
        }
        child->style()->unpolish(child);
        child->style()->polish(child);
    }
}

} // namespace UiFactory
