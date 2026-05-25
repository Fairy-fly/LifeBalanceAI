#include "animateddialog.h"
#include "designtokens.h"

#include <QApplication>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QParallelAnimationGroup>
#include <QPainter>
#include <QPainterPath>
#include <QScreen>
#include <QStyle>

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
    QWidget *parent = widget->parentWidget();
    QRect parentRect = parent ? parent->geometry() : QGuiApplication::primaryScreen()->geometry();
    QPoint topLeft = parent ? parent->mapToGlobal(QPoint(0, 0)) : parentRect.topLeft();
    widget->adjustSize();
    int x = topLeft.x() + (parentRect.width() - widget->width()) / 2;
    int y = topLeft.y() + (parentRect.height() - widget->height()) / 2;
    widget->move(x, y);
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
    setMinimumWidth(330);

    auto *outer = new QHBoxLayout(this);
    outer->setContentsMargins(0, 0, 0, 0);
    outer->setSpacing(0);

    m_accentBar = new QWidget(this);
    m_accentBar->setObjectName(QStringLiteral("dialogAccentBar"));
    m_accentBar->setFixedWidth(5);
    outer->addWidget(m_accentBar);

    auto *content = new QWidget(this);
    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(18, 16, 18, 14);
    layout->setSpacing(8);

    m_ttl = new QLabel(content);
    m_ttl->setObjectName(QStringLiteral("dialogTitle"));
    layout->addWidget(m_ttl);

    m_msg = new QLabel(content);
    m_msg->setObjectName(QStringLiteral("dialogMessage"));
    m_msg->setWordWrap(true);
    layout->addWidget(m_msg);

    auto *buttons = new QHBoxLayout();
    buttons->setObjectName(QStringLiteral("buttonLayout"));
    buttons->setContentsMargins(0, 8, 0, 0);
    buttons->setSpacing(8);
    buttons->addStretch();

    m_ok = new QPushButton(QString::fromUtf8("\u786e\u5b9a"), this);
    m_ok->setProperty("class", QStringLiteral("dialogPrimary"));
    m_ok->setFixedSize(84, 34);
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

void AnimatedDialog::setMessage(const QString &text) { m_msg->setText(text); }
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
    centerOnParent(this);
#ifdef Q_OS_ANDROID
    show();
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
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), 12, 12);
    painter.fillPath(path, QColor(QStringLiteral("#FEFEFE")));
    painter.setPen(QPen(QColor(QStringLiteral("#E8E8E8")), 1));
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
    auto *dialog = new AnimatedDialog(parent, AnimStyle::RiseFromBottom);
    dialog->setTitle(title);
    dialog->setMessage(message);
    dialog->setType(QStringLiteral("warn"));
    dialog->m_ok->setText(QString::fromUtf8("\u786e\u8ba4"));

    auto *cancel = new QPushButton(QString::fromUtf8("\u53d6\u6d88"), dialog);
    cancel->setProperty("class", QStringLiteral("dialogSecondary"));
    cancel->setFixedSize(78, 34);
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
    second->setFixedSize(88, 34);
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
    setMinimumWidth(350);

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(20, 18, 20, 16);
    layout->setSpacing(10);

    m_ttl = new QLabel(this);
    m_ttl->setObjectName(QStringLiteral("dialogTitle"));
    layout->addWidget(m_ttl);

    m_pmt = new QLabel(this);
    m_pmt->setObjectName(QStringLiteral("dialogPrompt"));
    m_pmt->setWordWrap(true);
    layout->addWidget(m_pmt);

    m_inp = new QLineEdit(this);
    m_inp->setMinimumHeight(40);
    layout->addWidget(m_inp);

    auto *buttons = new QHBoxLayout();
    buttons->setContentsMargins(0, 8, 0, 0);
    buttons->setSpacing(8);
    buttons->addStretch();

    m_ccl = new QPushButton(QString::fromUtf8("\u53d6\u6d88"), this);
    m_ccl->setProperty("class", QStringLiteral("dialogSecondary"));
    m_ccl->setFixedSize(78, 34);
    m_ccl->setCursor(Qt::PointingHandCursor);
    buttons->addWidget(m_ccl);

    m_ok = new QPushButton(QString::fromUtf8("\u786e\u5b9a"), this);
    m_ok->setProperty("class", QStringLiteral("dialogPrimary"));
    m_ok->setFixedSize(78, 34);
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
    centerOnParent(this);
#ifdef Q_OS_ANDROID
    show();
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
    group->start(QAbstractAnimation::DeleteWhenStopped);
#endif
}

void AnimatedInputDialog::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QPainterPath path;
    path.addRoundedRect(rect().adjusted(1, 1, -1, -1), 12, 12);
    painter.fillPath(path, QColor(QStringLiteral("#FEFEFE")));
    painter.setPen(QPen(QColor(QStringLiteral("#E8E8E8")), 1));
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
