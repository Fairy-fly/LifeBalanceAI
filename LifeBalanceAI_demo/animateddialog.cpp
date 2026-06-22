#include "animateddialog.h"
#include "designtokens.h"
#include "platformlayoutpolicy.h"

#include <QApplication>
#include <QEventLoop>
#include <QGuiApplication>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QParallelAnimationGroup>
#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>
#include <QScrollBar>
#include <QScreen>
#include <QSizePolicy>
#include <QStyle>
#include <QTimer>

namespace {

void repolish(QWidget *widget)
{
    if (!widget)
        return;
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
    widget->update();
}

void applyTransparentDialogChrome(QDialog *dialog)
{
    if (!dialog)
        return;

#ifdef Q_OS_ANDROID
    if (dialog->parentWidget()) {
        dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Widget);
        dialog->setWindowModality(Qt::ApplicationModal);
    } else {
        dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    }
#else
    dialog->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
#endif
    dialog->setAttribute(Qt::WA_TranslucentBackground, true);
    dialog->setAttribute(Qt::WA_NoSystemBackground, true);
    dialog->setAutoFillBackground(false);
    dialog->setStyleSheet(QStringLiteral("QDialog{background:transparent;}"));
}

void applyPanelShadow(QFrame *panel)
{
    if (!panel)
        return;

#ifndef Q_OS_ANDROID
    auto *shadow = new QGraphicsDropShadowEffect(panel);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 9);
    shadow->setColor(QColor(35, 45, 40, 58));
    panel->setGraphicsEffect(shadow);
#endif
}

QFrame *createFakeShadowLayer(QWidget *parent, const QString &objectName, int alpha)
{
    auto *layer = new QFrame(parent);
    layer->setObjectName(objectName);
    layer->setAttribute(Qt::WA_StyledBackground, true);
    layer->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    layer->setAutoFillBackground(false);
    layer->setStyleSheet(QStringLiteral(
        "QFrame#%1{background:rgba(30,36,32,%2);border-radius:18px;border:none;}").arg(objectName).arg(alpha));
    layer->hide();
    return layer;
}

void syncFakeShadowLayers(QWidget *root)
{
    if (!root)
        return;

    auto *panel = root->findChild<QFrame *>(QStringLiteral("animatedDialogPanel"), Qt::FindDirectChildrenOnly);
    auto *nearLayer = root->findChild<QFrame *>(QStringLiteral("modalShadowNear"), Qt::FindDirectChildrenOnly);
    auto *farLayer = root->findChild<QFrame *>(QStringLiteral("modalShadowFar"), Qt::FindDirectChildrenOnly);
    if (!panel || !nearLayer || !farLayer || panel->geometry().isEmpty())
        return;

    const QRect panelRect = panel->geometry();
    farLayer->setGeometry(panelRect.adjusted(-1, 0, 5, 4).translated(4, 10));
    nearLayer->setGeometry(panelRect.adjusted(0, 0, 3, 2).translated(2, 5));
    farLayer->show();
    nearLayer->show();
    farLayer->lower();
    nearLayer->raise();
    nearLayer->stackUnder(panel);
    panel->raise();
}

void centerOnParent(QWidget *widget)
{
#ifdef Q_OS_ANDROID
    if (widget) {
        if (auto *panel = widget->findChild<QFrame *>(QStringLiteral("animatedDialogPanel"))) {
            const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
            panel->adjustSize();
            const QSize target = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogSizeForRole(
                LifeBalanceAI::Ui::PlatformLayoutPolicy::DialogRole::Alert,
                panel->sizeHint().expandedTo(panel->minimumSizeHint()));
            panel->setFixedSize(target);
            if (auto *layout = widget->layout())
                layout->setAlignment(panel, Qt::AlignCenter);
            if (widget->parentWidget()) {
                QWidget *host = widget->parentWidget()->window();
                widget->setGeometry(host ? host->rect() : QRect(QPoint(0, 0), available.size()));
            } else {
                widget->resize(target);
                widget->move(available.x() + (available.width() - widget->width()) / 2,
                             available.y() + (available.height() - widget->height()) / 2);
            }
            syncFakeShadowLayers(widget);
            QTimer::singleShot(0, widget, [widget]() { syncFakeShadowLayers(widget); });
            return;
        }

        widget->adjustSize();
        const QSize preliminary = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogSizeForRole(
            LifeBalanceAI::Ui::PlatformLayoutPolicy::DialogRole::Alert,
            widget->sizeHint().expandedTo(widget->minimumSizeHint()));
        widget->resize(preliminary.width(), widget->height());
        widget->adjustSize();
        widget->resize(LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogSizeForRole(
            LifeBalanceAI::Ui::PlatformLayoutPolicy::DialogRole::Alert,
            widget->sizeHint().expandedTo(widget->minimumSizeHint())));
    }
    LifeBalanceAI::Ui::PlatformLayoutPolicy::centerWidgetOnSafeArea(widget);
    if (widget) {
        const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
        const int lift = qMin(18, qMax(0, available.height() / 36));
        widget->move(widget->x(), qMax(available.y(), widget->y() - lift));
    }
    return;
#else
    QWidget *parent = widget->parentWidget();
    QRect parentRect = parent ? parent->geometry() : QGuiApplication::primaryScreen()->geometry();
    QPoint topLeft = parent ? parent->mapToGlobal(QPoint(0, 0)) : parentRect.topLeft();
    widget->adjustSize();
    int x = topLeft.x() + (parentRect.width() - widget->width()) / 2;
    int y = topLeft.y() + (parentRect.height() - widget->height()) / 2;
    widget->move(x, y);
#endif
}

void placeInputDialogForKeyboard(QWidget *widget)
{
#ifdef Q_OS_ANDROID
    if (!widget)
        return;

    QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
        if (auto *panel = widget->findChild<QFrame *>(QStringLiteral("animatedDialogPanel"))) {
            panel->adjustSize();
            const QSize targetSize = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogSizeForRole(
                LifeBalanceAI::Ui::PlatformLayoutPolicy::DialogRole::Input,
            panel->sizeHint().expandedTo(panel->minimumSizeHint()));
        panel->setFixedSize(targetSize.boundedTo(available.size()));
        if (auto *layout = widget->layout()) {
            layout->setContentsMargins(widget->parentWidget() ? 18 : 0, 0, widget->parentWidget() ? 18 : 0, 0);
            layout->setAlignment(panel, Qt::AlignCenter);
        }
        if (widget->parentWidget()) {
            QWidget *host = widget->parentWidget()->window();
            widget->setGeometry(host ? host->rect() : QRect(QPoint(0, 0), available.size()));
        } else {
            widget->resize(panel->size());
            const int lift = qMin(18, qMax(0, available.height() / 36));
            widget->move(available.x() + (available.width() - widget->width()) / 2,
                         qMax(available.y(), available.y() + (available.height() - widget->height()) / 2 - lift));
        }
        syncFakeShadowLayers(widget);
        QTimer::singleShot(0, widget, [widget]() { syncFakeShadowLayers(widget); });
        return;
    }

    widget->adjustSize();
    const QSize targetSize = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogSizeForRole(
        LifeBalanceAI::Ui::PlatformLayoutPolicy::DialogRole::Input,
        widget->sizeHint().expandedTo(widget->minimumSizeHint()));
    widget->resize(targetSize.boundedTo(available.size()));

    const int x = available.x() + (available.width() - widget->width()) / 2;
    const int y = available.y() + qMax(24, available.height() / 7);
    widget->move(x, qMin(y, available.bottom() - widget->height()));
#else
    centerOnParent(widget);
#endif
}

} // namespace

AnimatedDialog::AnimatedDialog(QWidget *parent, AnimStyle style)
    : QDialog(parent), m_style(style)
{
    setupUi();
}

void AnimatedDialog::setupUi()
{
    applyTransparentDialogChrome(this);
    setModal(true);
#ifdef Q_OS_ANDROID
    setMinimumWidth(0);
#else
    setMinimumWidth(330);
#endif

    auto *outer = new QHBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    m_shadowFar = createFakeShadowLayer(this, QStringLiteral("modalShadowFar"), 9);
    m_shadowNear = createFakeShadowLayer(this, QStringLiteral("modalShadowNear"), 18);

    m_panel = new QFrame(this);
    m_panel->setObjectName(QStringLiteral("animatedDialogPanel"));
    m_panel->setAttribute(Qt::WA_StyledBackground, true);
    m_panel->setStyleSheet(QStringLiteral(
        "QFrame#animatedDialogPanel{"
        "background:#FFFDF9;"
        "border:1px solid #E5D9CC;"
        "border-radius:16px;"
        "}"));
    applyPanelShadow(m_panel);

    auto *panelRow = new QHBoxLayout(m_panel);
    panelRow->setContentsMargins(0, 0, 0, 0);
    panelRow->setSpacing(0);

    m_accentBar = new QWidget(m_panel);
    m_accentBar->setObjectName(QStringLiteral("dialogAccentBar"));
    m_accentBar->setFixedWidth(0);
    panelRow->addWidget(m_accentBar);

    auto *content = new QWidget(m_panel);
    content->setAttribute(Qt::WA_TranslucentBackground, true);
    auto *layout = new QVBoxLayout(content);
#ifdef Q_OS_ANDROID
    layout->setContentsMargins(22, 20, 22, 20);
#else
    layout->setContentsMargins(DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile - 2,
                               DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile - 4);
#endif
    layout->setSpacing(DesignTokens::SpaceSm);

    m_ttl = new QLabel(content);
    m_ttl->setObjectName(QStringLiteral("dialogTitle"));
    layout->addWidget(m_ttl);

    m_msg = new QLabel(content);
    m_msg->setObjectName(QStringLiteral("dialogMessage"));
    m_msg->setWordWrap(true);
    m_msg->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *messageHost = new QWidget(content);
    messageHost->setObjectName(QStringLiteral("dialogMessageHost"));
    auto *messageLayout = new QVBoxLayout(messageHost);
    messageLayout->setContentsMargins(0, 0, 0, 0);
    messageLayout->setSpacing(0);
    messageLayout->addWidget(m_msg);

    m_msgScroll = new QScrollArea(content);
    m_msgScroll->setObjectName(QStringLiteral("dialogMessageScroll"));
    m_msgScroll->setFrameShape(QFrame::NoFrame);
    m_msgScroll->setWidgetResizable(true);
    m_msgScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_msgScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_msgScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_msgScroll->setWidget(messageHost);
    if (QScrollBar *bar = m_msgScroll->verticalScrollBar())
        bar->setFixedWidth(3);
    layout->addWidget(m_msgScroll);

    auto *buttons = new QHBoxLayout();
    buttons->setObjectName(QStringLiteral("buttonLayout"));
    buttons->setContentsMargins(0, 6, 0, 0);
    buttons->setSpacing(DesignTokens::SpaceSm);
    buttons->addStretch();

    m_ok = new QPushButton(QString::fromUtf8("\u786e\u5b9a"), this);
    m_ok->setProperty("class", QStringLiteral("dialogPrimary"));
    m_ok->setMinimumSize(76, 36);
    m_ok->setMaximumHeight(38);
    m_ok->setCursor(Qt::PointingHandCursor);
    buttons->addWidget(m_ok);
    layout->addLayout(buttons);
    panelRow->addWidget(content);
    outer->addWidget(m_panel);

    connect(m_ok, &QPushButton::clicked, this, &QDialog::accept);
}

void AnimatedDialog::refreshMessageScrollLimit()
{
    if (!m_msgScroll)
        return;

#ifdef Q_OS_ANDROID
    const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
    const int maxMessageHeight = qBound(84, available.height() * 38 / 100, 260);
#else
    const int maxMessageHeight = 420;
#endif
    m_msgScroll->setMinimumHeight(0);
    m_msgScroll->setMaximumHeight(maxMessageHeight);
}

void AnimatedDialog::setMessage(const QString &text)
{
    m_msg->setText(text);
    refreshMessageScrollLimit();
}
void AnimatedDialog::setTitle(const QString &text) { m_ttl->setText(text); }

void AnimatedDialog::setType(const QString &type)
{
    m_type = type;
    const QString accent = (type == QStringLiteral("warn"))
        ? QStringLiteral("#F4A261")
        : QStringLiteral("#4CAF7F");
    Q_UNUSED(accent)
    const QString titleClass = (type == QStringLiteral("warn")) ? QStringLiteral("warn") : QString();
    m_accentBar->setProperty("class", titleClass);
    m_ttl->setProperty("class", titleClass);
    repolish(m_accentBar);
    repolish(m_ttl);
}

void AnimatedDialog::showAnimated()
{
    Q_UNUSED(m_style)
    refreshMessageScrollLimit();
    centerOnParent(this);
#ifdef Q_OS_ANDROID
    show();
    QTimer::singleShot(0, this, [this]() {
        if (isVisible())
            centerOnParent(this);
    });
    QTimer::singleShot(120, this, [this]() {
        if (isVisible())
            centerOnParent(this);
    });
    return;
#else
    QPoint end = pos();
    QPoint start = end + QPoint(0, 8);
    move(start);
    setWindowOpacity(0.0);
    show();

    auto *group = new QParallelAnimationGroup(this);
    auto *moveAnim = new QPropertyAnimation(this, "pos", group);
    moveAnim->setDuration(DesignTokens::MotionFast);
    moveAnim->setStartValue(start);
    moveAnim->setEndValue(end);
    moveAnim->setEasingCurve(QEasingCurve::OutCubic);

    auto *fade = new QPropertyAnimation(this, "windowOpacity", group);
    fade->setDuration(DesignTokens::MotionFast);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    fade->setEasingCurve(QEasingCurve::OutCubic);

    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}

void AnimatedDialog::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);
    if (m_type == QStringLiteral("success")) {
        QTimer::singleShot(2200, this, [this]() {
            if (isVisible())
                accept();
        });
    }
}

void AnimatedDialog::paintEvent(QPaintEvent *)
{
    if (m_panel) {
        QPainter painter(this);
        painter.fillRect(rect(), parentWidget() ? QColor(24, 28, 25, 118) : Qt::transparent);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), DesignTokens::RadiusXl, DesignTokens::RadiusXl);
    painter.fillPath(path, QColor(DesignTokens::bgCard()));
    painter.setPen(QPen(QColor(DesignTokens::border()), 1));
    painter.drawPath(path);
}

void AnimatedDialog::info(QWidget *parent, const QString &title, const QString &message)
{
    auto *dialog = new AnimatedDialog(parent, AnimStyle::RiseFromBottom);
    dialog->setTitle(title);
    dialog->setMessage(message);
    dialog->setType(QStringLiteral("info"));
    dialog->showAnimated();
    dialog->exec();
    dialog->deleteLater();
}

void AnimatedDialog::warn(QWidget *parent, const QString &title, const QString &message)
{
    auto *dialog = new AnimatedDialog(parent, AnimStyle::RiseFromBottom);
    dialog->setTitle(title);
    dialog->setMessage(message);
    dialog->setType(QStringLiteral("warn"));
    dialog->showAnimated();
    dialog->exec();
    dialog->deleteLater();
}

void AnimatedDialog::success(QWidget *parent, const QString &title, const QString &message)
{
    auto *dialog = new AnimatedDialog(parent, AnimStyle::RiseFromBottom);
    dialog->setTitle(title);
    dialog->setMessage(message);
    dialog->setType(QStringLiteral("success"));
    dialog->showAnimated();
    dialog->exec();
    dialog->deleteLater();
}

bool AnimatedDialog::confirm(QWidget *parent, const QString &title, const QString &message)
{
#ifdef Q_OS_ANDROID
    QWidget *host = parent ? parent->window() : QApplication::activeWindow();
    if (host) {
        auto *overlay = new QWidget(host);
        overlay->setObjectName(QStringLiteral("confirmDialogOverlay"));
        overlay->setAttribute(Qt::WA_StyledBackground, true);
        overlay->setGeometry(host->rect());
        overlay->setStyleSheet(QStringLiteral(
        "#confirmDialogOverlay{background:rgba(24,28,25,118);}"));

        auto *shadowFar = new QFrame(overlay);
        shadowFar->setObjectName(QStringLiteral("confirmShadowFar"));
        shadowFar->setAttribute(Qt::WA_StyledBackground, true);
        shadowFar->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        shadowFar->setStyleSheet(QStringLiteral(
            "QFrame#confirmShadowFar{background:rgba(30,36,32,9);border-radius:18px;border:none;}"));

        auto *shadowNear = new QFrame(overlay);
        shadowNear->setObjectName(QStringLiteral("confirmShadowNear"));
        shadowNear->setAttribute(Qt::WA_StyledBackground, true);
        shadowNear->setAttribute(Qt::WA_TransparentForMouseEvents, true);
        shadowNear->setStyleSheet(QStringLiteral(
            "QFrame#confirmShadowNear{background:rgba(30,36,32,18);border-radius:18px;border:none;}"));

        auto *panel = new QFrame(overlay);
        panel->setObjectName(QStringLiteral("confirmDialogPanel"));
        panel->setAttribute(Qt::WA_StyledBackground, true);
        panel->setStyleSheet(QStringLiteral(
            "QFrame#confirmDialogPanel{"
            "background:#FFFDF9;border:1px solid #E5D9CC;border-radius:16px;"
            "}"
            "QLabel#confirmDialogTitle{"
            "color:#B86A3F;font-size:17px;font-weight:700;"
            "font-family:\"MiSans Medium\",\"MiSans\",\"Noto Sans SC\",\"Microsoft YaHei UI\",sans-serif;"
            "background:transparent;"
            "}"
            "QLabel#confirmDialogMessage{"
            "color:#333333;font-size:14px;font-weight:400;line-height:1.45;"
            "font-family:\"MiSans\",\"Noto Sans SC\",\"Microsoft YaHei UI\",sans-serif;"
            "background:transparent;"
            "}"
            "QPushButton#confirmCancelButton{"
            "min-height:38px;border-radius:12px;background:#FFFFFF;"
            "border:1px solid #E8DED2;color:#333333;font-size:13px;font-weight:500;"
            "font-family:\"MiSans Medium\",\"MiSans\",\"Noto Sans SC\",\"Microsoft YaHei UI\",sans-serif;"
            "}"
            "QPushButton#confirmOkButton{"
            "min-height:38px;border-radius:12px;background:#FFF3F1;"
            "border:1px solid #E9B9B2;color:#B7645C;font-size:13px;font-weight:600;"
            "font-family:\"MiSans Medium\",\"MiSans\",\"Noto Sans SC\",\"Microsoft YaHei UI\",sans-serif;"
            "}"
            "QPushButton#confirmCancelButton:pressed{background:#F6F1EA;}"
            "QPushButton#confirmOkButton:pressed{background:#F8E2DE;border-color:#D48A82;}"));

        auto *layout = new QVBoxLayout(panel);
        layout->setContentsMargins(22, 18, 22, 18);
        layout->setSpacing(12);

        auto *titleLabel = new QLabel(title, panel);
        titleLabel->setObjectName(QStringLiteral("confirmDialogTitle"));
        titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        layout->addWidget(titleLabel);

        auto *messageLabel = new QLabel(message, panel);
        messageLabel->setObjectName(QStringLiteral("confirmDialogMessage"));
        messageLabel->setWordWrap(true);
        messageLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        layout->addWidget(messageLabel);

        auto *buttons = new QHBoxLayout();
        buttons->setContentsMargins(0, 6, 0, 0);
        buttons->setSpacing(10);
        buttons->addStretch();

        auto *cancel = new QPushButton(QString::fromUtf8("\u53d6\u6d88"), panel);
        cancel->setObjectName(QStringLiteral("confirmCancelButton"));
        cancel->setMinimumSize(92, 38);
        cancel->setMaximumHeight(42);
        cancel->setCursor(Qt::PointingHandCursor);
        buttons->addWidget(cancel);

        auto *ok = new QPushButton(QString::fromUtf8("\u786e\u8ba4"), panel);
        ok->setObjectName(QStringLiteral("confirmOkButton"));
        ok->setMinimumSize(92, 38);
        ok->setMaximumHeight(42);
        ok->setCursor(Qt::PointingHandCursor);
        buttons->addWidget(ok);
        layout->addLayout(buttons);

        const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect(22);
        const int targetWidth = qBound(300, available.width() * 86 / 100, available.width());
        panel->setFixedWidth(targetWidth);
        panel->adjustSize();
        const int lift = qMin(18, qMax(0, available.height() / 36));
        panel->move(available.x() + (available.width() - panel->width()) / 2,
                    qMax(available.y(), available.y() + (available.height() - panel->height()) / 2 - lift));
        shadowFar->setGeometry(panel->geometry().adjusted(-1, 0, 5, 4).translated(4, 10));
        shadowNear->setGeometry(panel->geometry().adjusted(0, 0, 3, 2).translated(2, 5));

        bool result = false;
        QEventLoop loop;
        connect(cancel, &QPushButton::clicked, overlay, [&]() {
            result = false;
            loop.quit();
        });
        connect(ok, &QPushButton::clicked, overlay, [&]() {
            result = true;
            loop.quit();
        });

        overlay->show();
        shadowFar->show();
        shadowNear->show();
        shadowFar->lower();
        shadowNear->raise();
        shadowNear->stackUnder(panel);
        panel->raise();
        loop.exec();
        overlay->deleteLater();
        return result;
    }
#endif

    auto *dialog = new AnimatedDialog(parent, AnimStyle::RiseFromBottom);
    dialog->setTitle(title);
    dialog->setMessage(message);
    dialog->setType(QStringLiteral("warn"));
    dialog->m_ok->setText(QString::fromUtf8("\u786e\u8ba4"));

    auto *cancel = new QPushButton(QString::fromUtf8("\u53d6\u6d88"), dialog);
    cancel->setProperty("class", QStringLiteral("dialogSecondary"));
    cancel->setMinimumSize(76, 36);
    cancel->setMaximumHeight(38);
    cancel->setCursor(Qt::PointingHandCursor);

    auto *buttonLayout = dialog->findChild<QHBoxLayout *>(QStringLiteral("buttonLayout"));
    if (buttonLayout)
        buttonLayout->insertWidget(1, cancel);

    bool result = false;
    disconnect(dialog->m_ok, nullptr, dialog, nullptr);
    connect(cancel, &QPushButton::clicked, dialog, [dialog, &result]() {
        result = false;
        dialog->reject();
    });
    connect(dialog->m_ok, &QPushButton::clicked, dialog, [dialog, &result]() {
        result = true;
        dialog->accept();
    });
    dialog->showAnimated();
    dialog->exec();
    dialog->deleteLater();
    return result;
}

int AnimatedDialog::choose(QWidget *parent, const QString &title, const QString &message,
                           const QString &btn1, const QString &btn2)
{
    auto *dialog = new AnimatedDialog(parent, AnimStyle::RiseFromBottom);
    dialog->setTitle(title);
    dialog->setMessage(message);
    dialog->setType(QStringLiteral("info"));
    dialog->m_ok->setText(btn1);

    auto *second = new QPushButton(btn2, dialog);
    second->setProperty("class", QStringLiteral("dialogSecondary"));
    second->setMinimumSize(82, 36);
    second->setMaximumHeight(38);
    second->setCursor(Qt::PointingHandCursor);

    auto *buttonLayout = dialog->findChild<QHBoxLayout *>(QStringLiteral("buttonLayout"));
    if (buttonLayout)
        buttonLayout->addWidget(second);

    int result = -1;
    disconnect(dialog->m_ok, nullptr, dialog, nullptr);
    connect(dialog->m_ok, &QPushButton::clicked, dialog, [dialog, &result]() {
        result = 0;
        dialog->accept();
    });
    connect(second, &QPushButton::clicked, dialog, [dialog, &result]() {
        result = 1;
        dialog->accept();
    });
    connect(dialog, &QDialog::rejected, dialog, [&result]() { result = -1; });
    dialog->showAnimated();
    dialog->exec();
    dialog->deleteLater();
    return result;
}

AnimatedInputDialog::AnimatedInputDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void AnimatedInputDialog::setupUi()
{
    applyTransparentDialogChrome(this);
    setModal(true);
#ifdef Q_OS_ANDROID
    setMinimumWidth(0);
#else
    setMinimumWidth(350);
#endif

    auto *outer = new QVBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    m_shadowFar = createFakeShadowLayer(this, QStringLiteral("modalShadowFar"), 9);
    m_shadowNear = createFakeShadowLayer(this, QStringLiteral("modalShadowNear"), 18);

    m_panel = new QFrame(this);
    m_panel->setObjectName(QStringLiteral("animatedDialogPanel"));
    m_panel->setAttribute(Qt::WA_StyledBackground, true);
    m_panel->setStyleSheet(QStringLiteral(
        "QFrame#animatedDialogPanel{"
        "background:#FFFDF9;"
        "border:1px solid #E5D9CC;"
        "border-radius:16px;"
        "}"));
    applyPanelShadow(m_panel);
    outer->addWidget(m_panel);

    auto *layout = new QVBoxLayout(m_panel);
#ifdef Q_OS_ANDROID
    layout->setContentsMargins(22, 20, 22, 20);
#else
    layout->setContentsMargins(DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile - 2);
#endif
    layout->setSpacing(DesignTokens::SpaceSm);

    m_ttl = new QLabel(this);
    m_ttl->setObjectName(QStringLiteral("dialogTitle"));
    layout->addWidget(m_ttl);

    m_pmt = new QLabel(this);
    m_pmt->setObjectName(QStringLiteral("dialogPrompt"));
    m_pmt->setWordWrap(true);
    layout->addWidget(m_pmt);

    m_inp = new QLineEdit(this);
    m_inp->setMinimumHeight(DesignTokens::ButtonHeightPrimary);
    layout->addWidget(m_inp);

    auto *buttons = new QHBoxLayout();
    buttons->setContentsMargins(0, 6, 0, 0);
    buttons->setSpacing(DesignTokens::SpaceSm);
    buttons->addStretch();

    m_ccl = new QPushButton(QString::fromUtf8("\u53d6\u6d88"), this);
    m_ccl->setProperty("class", QStringLiteral("dialogSecondary"));
    m_ccl->setMinimumSize(76, 36);
    m_ccl->setMaximumHeight(38);
    m_ccl->setCursor(Qt::PointingHandCursor);
    buttons->addWidget(m_ccl);

    m_ok = new QPushButton(QString::fromUtf8("\u786e\u5b9a"), this);
    m_ok->setProperty("class", QStringLiteral("dialogPrimary"));
    m_ok->setMinimumSize(76, 36);
    m_ok->setMaximumHeight(38);
    m_ok->setCursor(Qt::PointingHandCursor);
    buttons->addWidget(m_ok);
    layout->addLayout(buttons);

    connect(m_ok, &QPushButton::clicked, this, [this]() {
        m_result = m_inp->text();
        accept();
    });
    connect(m_ccl, &QPushButton::clicked, this, &QDialog::reject);
}

void AnimatedInputDialog::setTitle(const QString &title)
{
    const bool looksLikeAdjust =
        title.contains(QString::fromUtf8("调整规划"))
        || title.contains(QStringLiteral("璋冩暣瑙勫垝"));

    if (looksLikeAdjust) {
        m_isAdjustDialog = true;
        QString slotTitle = title.section(QStringLiteral(" - "), 1).trimmed();
        if (slotTitle.isEmpty())
            slotTitle = title.section(QChar('-'), 1).trimmed();
        m_ttl->setText(slotTitle.isEmpty()
                           ? QString::fromUtf8("调整规划")
                           : QString::fromUtf8("调整%1").arg(slotTitle));
        if (m_inp) {
            m_inp->setMinimumHeight(86);
            m_inp->setMaximumHeight(96);
        }
        return;
    }

    m_ttl->setText(title);
}

void AnimatedInputDialog::setPrompt(const QString &prompt)
{
    if (m_isAdjustDialog) {
        m_pmt->setText(QString::fromUtf8("告诉我你想怎么换"));
        if (m_inp)
            m_inp->setPlaceholderText(QString::fromUtf8("例如：换个更简单的，或不想吃这个"));
        return;
    }

    m_pmt->setText(prompt);
}
QString AnimatedInputDialog::textValue() const { return m_result; }

void AnimatedInputDialog::showAnimated()
{
#ifdef Q_OS_ANDROID
    placeInputDialogForKeyboard(this);
    show();
    QTimer::singleShot(160, this, [this]() {
        if (isVisible())
            placeInputDialogForKeyboard(this);
    });
    return;
#else
    centerOnParent(this);
    QPoint end = pos();
    QPoint start = end + QPoint(0, 8);
    move(start);
    setWindowOpacity(0.0);
    show();

    auto *group = new QParallelAnimationGroup(this);
    auto *moveAnim = new QPropertyAnimation(this, "pos", group);
    moveAnim->setDuration(DesignTokens::MotionFast);
    moveAnim->setStartValue(start);
    moveAnim->setEndValue(end);
    moveAnim->setEasingCurve(QEasingCurve::OutCubic);

    auto *fade = new QPropertyAnimation(this, "windowOpacity", group);
    fade->setDuration(DesignTokens::MotionFast);
    fade->setStartValue(0.0);
    fade->setEndValue(1.0);
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}

void AnimatedInputDialog::paintEvent(QPaintEvent *)
{
    if (m_panel) {
        QPainter painter(this);
        painter.fillRect(rect(), parentWidget() ? QColor(24, 28, 25, 118) : Qt::transparent);
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), DesignTokens::RadiusXl, DesignTokens::RadiusXl);
    painter.fillPath(path, QColor(DesignTokens::bgCard()));
    painter.setPen(QPen(QColor(DesignTokens::border()), 1));
    painter.drawPath(path);
}

QString AnimatedInputDialog::getText(QWidget *parent, const QString &title, const QString &prompt,
                                     const QString &def, bool *ok)
{
    auto *dialog = new AnimatedInputDialog(parent);
    dialog->setTitle(title);
    dialog->setPrompt(prompt);
    if (!def.isEmpty())
        dialog->m_inp->setText(def);
    dialog->showAnimated();
    bool accepted = (dialog->exec() == QDialog::Accepted);
    QString result = dialog->textValue();
    dialog->deleteLater();
    if (ok)
        *ok = accepted;
    return accepted ? result : QString();
}
