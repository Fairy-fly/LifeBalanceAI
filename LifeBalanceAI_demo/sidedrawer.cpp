#include "sidedrawer.h"
#include "designtokens.h"
#include "platformlayoutpolicy.h"

#include <QApplication>
#ifndef Q_OS_ANDROID
#include <QGraphicsDropShadowEffect>
#endif
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QSizePolicy>

SideDrawer::SideDrawer(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    hide();

    m_overlay = new QWidget(this);
    m_overlay->setStyleSheet(QStringLiteral("background:rgba(26,26,26,56);"));
    m_overlay->installEventFilter(this);
    m_overlay->hide();

    m_panel = new QWidget(this);
    m_panel->setObjectName(QStringLiteral("drawerPanel"));
    m_panel->setStyleSheet(QStringLiteral("#drawerPanel{background:%1;border-right:1px solid %2;}")
                               .arg(DesignTokens::bgCard(), DesignTokens::borderLight()));

#ifndef Q_OS_ANDROID
    auto *shadow = new QGraphicsDropShadowEffect(m_panel);
    shadow->setBlurRadius(24);
    shadow->setOffset(6, 0);
    shadow->setColor(QColor(26, 26, 26, 36));
    m_panel->setGraphicsEffect(shadow);
#endif

    m_panelLayout = new QVBoxLayout(m_panel);
    m_panelLayout->setContentsMargins(0, 0, 0, 0);
    m_panelLayout->setSpacing(0);
    m_panelLayout->setAlignment(Qt::AlignTop);
}

void SideDrawer::addHeader(const QString &nickname, const QString &role)
{
    m_currentGroup = nullptr;
    m_currentGroupLayout = nullptr;
    m_currentGroupItemCount = 0;

    auto *header = new QWidget(m_panel);
    header->setObjectName(QStringLiteral("drawerHeader"));
    header->setStyleSheet(QStringLiteral("#drawerHeader{background:%1;border-bottom:1px solid %2;}")
                              .arg(DesignTokens::primaryLightest(), DesignTokens::borderLight()));
#ifdef Q_OS_ANDROID
    const int topInset = LifeBalanceAI::Ui::PlatformLayoutPolicy::topSafeAreaInset();
#else
    const int topInset = 0;
#endif
    header->setFixedHeight(136 + topInset);

    auto *layout = new QVBoxLayout(header);
    layout->setContentsMargins(20, 10 + topInset, 20, 12);
    layout->setSpacing(7);

    auto *avatar = new QLabel(header);
    avatar->setFixedSize(60, 60);
    avatar->setAlignment(Qt::AlignCenter);
    avatar->setText(nickname.isEmpty() ? QStringLiteral("L") : nickname.left(1));
    avatar->setStyleSheet(QStringLiteral(
        "background:#4EAD7D;color:white;border-radius:30px;"
        "font-family:\"MiSans Medium\",\"MiSans\",\"Microsoft YaHei UI\";"
        "font-size:23px;font-weight:600;"));
    layout->addWidget(avatar);

    m_headerNickname = new QLabel(nickname.isEmpty() ? QStringLiteral("LifeBalance AI") : nickname, header);
    m_headerNickname->setWordWrap(false);
    m_headerNickname->setStyleSheet(QStringLiteral(
        "font-family:\"MiSans Medium\",\"MiSans\",\"Microsoft YaHei UI\";"
        "font-size:18px;font-weight:600;color:#1F2A24;background:transparent;"));
    layout->addWidget(m_headerNickname);

    m_headerRole = new QLabel(role, header);
    m_headerRole->setAlignment(Qt::AlignCenter);
    m_headerRole->setFixedHeight(24);
    m_headerRole->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_headerRole->setContentsMargins(10, 0, 10, 0);
    m_headerRole->setStyleSheet(QStringLiteral(
        "font-family:\"MiSans Medium\",\"MiSans\",\"Microsoft YaHei UI\";"
        "font-size:12px;font-weight:500;color:#2F6F57;"
        "background:#FFFDF9;border:1px solid #BDEDD5;border-radius:12px;"
        "padding:0 10px;"));
    layout->addWidget(m_headerRole);

    m_panelLayout->addWidget(header);
    m_panelLayout->addSpacing(12);
}

void SideDrawer::addSeparator()
{
    m_currentGroup = nullptr;
    m_currentGroupLayout = nullptr;
    m_currentGroupItemCount = 0;
    m_panelLayout->addSpacing(12);
}

void SideDrawer::addItem(const QString &icon, const QString &text, std::function<void()> callback)
{
    if (!m_currentGroupLayout) {
        auto *groupWrap = new QWidget(m_panel);
        groupWrap->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        auto *wrapLayout = new QHBoxLayout(groupWrap);
        wrapLayout->setContentsMargins(18, 0, 18, 0);
        wrapLayout->setSpacing(0);

        m_currentGroup = new QFrame(groupWrap);
        m_currentGroup->setObjectName(QStringLiteral("drawerItemGroup"));
        m_currentGroup->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        m_currentGroup->setStyleSheet(QStringLiteral(
            "QFrame#drawerItemGroup{"
            "background:#FFFDF9;border:1px solid #EFE4D8;border-radius:14px;"
            "}"));

        m_currentGroupLayout = new QVBoxLayout(m_currentGroup);
        m_currentGroupLayout->setContentsMargins(0, 0, 0, 0);
        m_currentGroupLayout->setSpacing(0);

        wrapLayout->addWidget(m_currentGroup);
        m_panelLayout->addWidget(groupWrap);
        m_currentGroupItemCount = 0;
    }

    if (m_currentGroupItemCount > 0) {
        auto *line = new QWidget(m_currentGroup);
        line->setFixedHeight(1);
        line->setStyleSheet(QStringLiteral("background:#F1E9DF;"));
        m_currentGroupLayout->addWidget(line);
    }

    const bool isLogout = text.contains(QString::fromUtf8("\u9000\u51fa"));
    const bool isStatus = !callback;
    const QString rightMark = isStatus ? QString::fromUtf8("  \u2713") : QString::fromUtf8("  \u203a");
    auto *button = new QPushButton(icon + QStringLiteral("  ") + text + rightMark, m_currentGroup);
    button->setFlat(true);
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedHeight(52);
    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    const QString normalColor = isLogout ? DesignTokens::accentDark() : DesignTokens::textBody();
    const QString hoverBg = isLogout ? DesignTokens::accentLightest() : DesignTokens::primaryLightest();
    const QString pressedBg = isLogout ? DesignTokens::accentLightest() : DesignTokens::primaryLighter();
    button->setStyleSheet(QStringLiteral(
        "QPushButton{"
        "text-align:left;padding:0 16px;"
        "font-family:\"MiSans Medium\",\"MiSans\",\"Microsoft YaHei UI\";"
        "font-size:14px;font-weight:500;color:%1;"
        "border:none;background:transparent;border-radius:0;"
        "}"
        "QPushButton:hover{background:%2;color:#1A1A1A;}"
        "QPushButton:pressed{background:%3;padding-top:1px;}"
        "QPushButton:disabled{color:#999999;background:#FAFAFA;}"
    ).arg(normalColor, hoverBg, pressedBg));

    if (callback) {
        connect(button, &QPushButton::clicked, this, [this, callback]() {
            hideAnimated();
            callback();
        });
    } else {
        button->setEnabled(false);
        button->setCursor(Qt::ArrowCursor);
    }

    m_currentGroupLayout->addWidget(button);
    ++m_currentGroupItemCount;
}

void SideDrawer::showAnimated()
{
    if (m_visible)
        return;
    m_visible = true;

    QWidget *parent = parentWidget();
    if (!parent)
        return;

    setGeometry(parent->rect());
    m_panelWidth = qBound(304, parent->width() * 82 / 100, 340);

    m_overlay->setGeometry(rect());
    m_panel->setGeometry(-m_panelWidth, 0, m_panelWidth, height());
    m_panel->show();
    m_overlay->show();
    show();
    raise();
    m_overlay->lower();
    m_panel->raise();

#ifndef Q_OS_ANDROID
    auto *fadeEffect = new QGraphicsOpacityEffect(m_overlay);
    fadeEffect->setOpacity(0.0);
    m_overlay->setGraphicsEffect(fadeEffect);

    m_fadeAnim = new QPropertyAnimation(fadeEffect, "opacity");
    m_fadeAnim->setDuration(DesignTokens::MotionBase);
    m_fadeAnim->setStartValue(0.0);
    m_fadeAnim->setEndValue(1.0);
    m_fadeAnim->setEasingCurve(QEasingCurve::OutCubic);
#endif

    m_slideAnim = new QPropertyAnimation(m_panel, "pos");
    m_slideAnim->setDuration(DesignTokens::MotionSlow);
    m_slideAnim->setStartValue(QPoint(-m_panelWidth, 0));
    m_slideAnim->setEndValue(QPoint(0, 0));
    m_slideAnim->setEasingCurve(QEasingCurve::OutCubic);

#ifndef Q_OS_ANDROID
    m_fadeAnim->start(QAbstractAnimation::DeleteWhenStopped);
#endif
    m_slideAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

void SideDrawer::hideAnimated()
{
    if (!m_visible)
        return;
    m_visible = false;

#ifndef Q_OS_ANDROID
    auto *fadeEffect = new QGraphicsOpacityEffect(m_overlay);
    fadeEffect->setOpacity(1.0);
    m_overlay->setGraphicsEffect(fadeEffect);

    m_fadeAnim = new QPropertyAnimation(fadeEffect, "opacity");
    m_fadeAnim->setDuration(DesignTokens::MotionFast);
    m_fadeAnim->setStartValue(1.0);
    m_fadeAnim->setEndValue(0.0);
    m_fadeAnim->setEasingCurve(QEasingCurve::OutCubic);
#endif

    m_slideAnim = new QPropertyAnimation(m_panel, "pos");
    m_slideAnim->setDuration(DesignTokens::MotionBase);
    m_slideAnim->setStartValue(QPoint(0, 0));
    m_slideAnim->setEndValue(QPoint(-m_panelWidth, 0));
    m_slideAnim->setEasingCurve(QEasingCurve::InCubic);

    connect(m_slideAnim, &QPropertyAnimation::finished, this, [this]() {
        m_overlay->hide();
        m_panel->hide();
        hide();
#ifndef Q_OS_ANDROID
        m_overlay->setGraphicsEffect(nullptr);
#endif
        emit closed();
    });

#ifndef Q_OS_ANDROID
    m_fadeAnim->start(QAbstractAnimation::DeleteWhenStopped);
#endif
    m_slideAnim->start(QAbstractAnimation::DeleteWhenStopped);
}

bool SideDrawer::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_overlay && event->type() == QEvent::MouseButtonPress) {
        hideAnimated();
        return true;
    }
    return QWidget::eventFilter(obj, event);
}
