#include "animateddialog.h"
#include "designtokens.h"
#include "platformlayoutpolicy.h"

#include <QApplication>
#include <QEventLoop>
#include <QGuiApplication>
#include <QFrame>
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

void centerOnParent(QWidget *widget)
{
#ifdef Q_OS_ANDROID
    if (widget) {
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
#ifndef Q_OS_ANDROID
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
#endif
    setModal(true);
#ifdef Q_OS_ANDROID
    setMinimumWidth(0);
#else
    setMinimumWidth(330);
#endif

    auto *outer = new QHBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    m_accentBar = new QWidget(this);
    m_accentBar->setObjectName(QStringLiteral("dialogAccentBar"));
    m_accentBar->setFixedWidth(5);
    outer->addWidget(m_accentBar);

    auto *content = new QWidget(this);
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile - 2,
                               DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile - 4);
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
    buttons->setContentsMargins(0, DesignTokens::SpaceSm, 0, 0);
    buttons->setSpacing(DesignTokens::SpaceSm);
    buttons->addStretch();

    m_ok = new QPushButton(QString::fromUtf8("\u786e\u5b9a"), this);
    m_ok->setProperty("class", QStringLiteral("dialogPrimary"));
    m_ok->setMinimumSize(76, 36);
    m_ok->setMaximumHeight(38);
    m_ok->setCursor(Qt::PointingHandCursor);
    buttons->addWidget(m_ok);
    layout->addLayout(buttons);
    outer->addWidget(content);

    connect(m_ok, &QPushButton::clicked, this, &QDialog::accept);

#ifndef Q_OS_ANDROID
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 10);
    shadow->setColor(QColor(26, 26, 26, 36));
    setGraphicsEffect(shadow);
#endif
}

void AnimatedDialog::refreshMessageScrollLimit()
{
    if (!m_msgScroll)
        return;

#ifdef Q_OS_ANDROID
    const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
    const int maxMessageHeight = qBound(92, available.height() * 46 / 100, 320);
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
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), DesignTokens::RadiusLg, DesignTokens::RadiusLg);
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
            "#confirmDialogOverlay{background:rgba(28,24,20,92);}"));

        auto *panel = new QFrame(overlay);
        panel->setObjectName(QStringLiteral("confirmDialogPanel"));
        panel->setAttribute(Qt::WA_StyledBackground, true);
        panel->setStyleSheet(QStringLiteral(
            "QFrame#confirmDialogPanel{"
            "background:#FFFDF9;border:1px solid #E8DED2;border-radius:14px;"
            "}"
            "QLabel#confirmDialogTitle{"
            "color:#B86A3F;font-size:17px;font-weight:600;"
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
        layout->setContentsMargins(24, 22, 24, 20);
        layout->setSpacing(14);

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
        buttons->setContentsMargins(0, 8, 0, 0);
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

        auto *shadow = new QGraphicsDropShadowEffect(panel);
        shadow->setBlurRadius(24);
        shadow->setOffset(0, 8);
        shadow->setColor(QColor(26, 26, 26, 42));
        panel->setGraphicsEffect(shadow);

        const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect(22);
        const int targetWidth = qBound(300, available.width() * 86 / 100, available.width());
        panel->setFixedWidth(targetWidth);
        panel->adjustSize();
        panel->move(available.x() + (available.width() - panel->width()) / 2,
                    available.y() + (available.height() - panel->height()) / 2);

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
#ifndef Q_OS_ANDROID
    setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);
    setAttribute(Qt::WA_TranslucentBackground);
#endif
    setModal(true);
#ifdef Q_OS_ANDROID
    setMinimumWidth(0);
#else
    setMinimumWidth(350);
#endif

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile,
                               DesignTokens::DialogPaddingMobile - 2);
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
    buttons->setContentsMargins(0, DesignTokens::SpaceSm, 0, 0);
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

#ifndef Q_OS_ANDROID
    auto *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 10);
    shadow->setColor(QColor(26, 26, 26, 36));
    setGraphicsEffect(shadow);
#endif
}

void AnimatedInputDialog::setTitle(const QString &title) { m_ttl->setText(title); }
void AnimatedInputDialog::setPrompt(const QString &prompt) { m_pmt->setText(prompt); }
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
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), DesignTokens::RadiusLg, DesignTokens::RadiusLg);
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
