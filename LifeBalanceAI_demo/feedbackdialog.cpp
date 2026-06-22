#include "feedbackdialog.h"
#include "designtokens.h"
#include "platformlayoutpolicy.h"

#include <QFileDialog>
#include <QEvent>
#include <QFrame>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QScreen>
#include <QSizePolicy>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace {

QFrame *createModalShadow(QWidget *parent, const QString &name, int alpha)
{
    auto *shadow = new QFrame(parent);
    shadow->setObjectName(name);
    shadow->setAttribute(Qt::WA_StyledBackground, true);
    shadow->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    shadow->setStyleSheet(QStringLiteral(
        "QFrame#%1{background:rgba(30,36,32,%2);border-radius:18px;border:none;}").arg(name).arg(alpha));
    shadow->hide();
    return shadow;
}

void syncModalShadow(QFrame *panel, QFrame *nearLayer, QFrame *farLayer)
{
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

} // namespace

FeedbackDialog::FeedbackDialog(const QString &slotTitle, QWidget *parent)
    : QDialog(parent)
{
    setupUi(slotTitle, QString());
}

FeedbackDialog::FeedbackDialog(const QString &slotTitle,
                               const QString &initialText,
                               QWidget *parent)
    : QDialog(parent)
{
    setupUi(slotTitle, initialText);
}

void FeedbackDialog::setupUi(const QString &slotTitle, const QString &initialText)
{
    setObjectName(QStringLiteral("feedbackDialog"));
    setWindowTitle(QStringLiteral("反馈 - %1").arg(slotTitle));
    setModal(true);
    setWindowTitle(QString::fromUtf8("反馈 - %1").arg(slotTitle));
#ifdef Q_OS_ANDROID
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);
#endif
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(QStringLiteral(
        "QDialog#feedbackDialog{background:rgba(24,28,25,118);}"
        "QFrame#feedbackDialogPanel{background:%1;border:1px solid #E5D9CC;border-radius:16px;}"
        "QLabel#feedbackTitle{background:transparent;color:#1F2A24;font-size:16px;"
        "font-family:\"MiSans Medium\",\"MiSans\",\"Noto Sans SC\",\"Microsoft YaHei UI\";font-weight:700;}"
        "QLabel#feedbackSubtitle{background:transparent;color:#827B72;font-size:12px;"
        "font-family:\"MiSans\",\"Noto Sans SC\",\"Microsoft YaHei UI\";font-weight:400;}"
        "QLabel#feedbackImagePreview{background:#FFFFFF;border:1px dashed %2;"
        "border-radius:10px;color:#999999;font-size:13px;}")
        .arg(DesignTokens::bgCard(),
             DesignTokens::borderStrong()));

#ifndef Q_OS_ANDROID
    setMinimumSize(380, 380);
    resize(420, 430);
#endif

    auto *outer = new QVBoxLayout(this);
    m_outerLayout = outer;
#ifdef Q_OS_ANDROID
    outer->setContentsMargins(18, 0, 18, 48);
#else
    outer->setContentsMargins(0, 0, 0, 0);
#endif
    outer->setSpacing(0);

    auto *shadowFar = createModalShadow(this, QStringLiteral("feedbackShadowFar"), 9);
    auto *shadowNear = createModalShadow(this, QStringLiteral("feedbackShadowNear"), 18);
    m_shadowFar = shadowFar;
    m_shadowNear = shadowNear;

    auto *panel = new QFrame(this);
    m_panel = panel;
    panel->setObjectName(QStringLiteral("feedbackDialogPanel"));
    panel->setAttribute(Qt::WA_StyledBackground, true);
#ifdef Q_OS_ANDROID
    outer->addWidget(panel, 0, Qt::AlignCenter);
#else
    outer->addWidget(panel);
#endif

    auto *layout = new QVBoxLayout(panel);
#ifdef Q_OS_ANDROID
    layout->setContentsMargins(22, 18, 22, 16);
    layout->setSpacing(7);
#else
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);
#endif

    auto *label = new QLabel(
        QStringLiteral("请描述您对「%1」计划的执行感受或改进建议：").arg(slotTitle),
        this);
    label->setObjectName(QStringLiteral("feedbackTitle"));
    label->setText(QString::fromUtf8("%1反馈").arg(slotTitle));
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(label);

    auto *subtitleLabel = new QLabel(QString::fromUtf8("记录执行感受或改进建议"), panel);
    subtitleLabel->setObjectName(QStringLiteral("feedbackSubtitle"));
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(subtitleLabel);

    m_textEdit = new QTextEdit(this);
    m_textEdit->setFocusPolicy(Qt::ClickFocus);
    m_textEdit->installEventFilter(this);
#ifdef Q_OS_ANDROID
    m_textEdit->setFixedHeight(94);
#else
    m_textEdit->setMinimumHeight(150);
#endif
#ifdef Q_OS_ANDROID
    m_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
#else
    m_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#endif
    if (initialText.isEmpty())
        m_textEdit->setPlaceholderText(QStringLiteral("请输入反馈内容..."));
    else
        m_textEdit->setPlainText(initialText);
    if (initialText.isEmpty())
        m_textEdit->setPlaceholderText(QString::fromUtf8("请输入反馈内容..."));
    layout->addWidget(m_textEdit);

    auto *imgRow = new QHBoxLayout;
#ifdef Q_OS_ANDROID
    imgRow->setSpacing(8);
#else
    imgRow->setSpacing(12);
#endif

    m_btnSelectImage = new QPushButton(QStringLiteral("选择图片（可选）"), this);
    m_btnSelectImage->setProperty("class", QStringLiteral("secondary"));
    m_btnSelectImage->setText(QString::fromUtf8("选择图片（可选）"));
#ifdef Q_OS_ANDROID
    m_btnSelectImage->setMinimumSize(132, 38);
    m_btnSelectImage->setMaximumHeight(40);
#else
    m_btnSelectImage->setMinimumSize(180, 58);
    m_btnSelectImage->setMaximumHeight(58);
#endif
    m_btnSelectImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    imgRow->addWidget(m_btnSelectImage);

    m_lblImagePreview = new QLabel(this);
    m_lblImagePreview->setObjectName(QStringLiteral("feedbackImagePreview"));
#ifdef Q_OS_ANDROID
    m_lblImagePreview->setFixedSize(88, 38);
#else
    m_lblImagePreview->setFixedSize(118, 58);
#endif
    m_lblImagePreview->setAlignment(Qt::AlignCenter);
    m_lblImagePreview->setText(QString::fromUtf8("预览"));
    m_lblImagePreview->setText(QStringLiteral("预览"));
    imgRow->addWidget(m_lblImagePreview);
    imgRow->addStretch();
    layout->addLayout(imgRow);

    auto *actionRow = new QHBoxLayout;
#ifdef Q_OS_ANDROID
    actionRow->setSpacing(8);
#else
    actionRow->setSpacing(12);
#endif

    m_btnCancel = new QPushButton(QStringLiteral("取消"), this);
    m_btnCancel->setProperty("class", QStringLiteral("secondary"));
    m_btnCancel->setText(QString::fromUtf8("取消"));
#ifdef Q_OS_ANDROID
    m_btnCancel->setMinimumHeight(38);
    m_btnCancel->setMaximumHeight(40);
#else
    m_btnCancel->setMinimumHeight(44);
#endif
    m_btnCancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    actionRow->addWidget(m_btnCancel);

    m_btnConfirm = new QPushButton(QStringLiteral("确认提交"), this);
    m_btnConfirm->setProperty("class", QStringLiteral("primary"));
    m_btnConfirm->setText(QString::fromUtf8("确认提交"));
#ifdef Q_OS_ANDROID
    m_btnConfirm->setMinimumHeight(38);
    m_btnConfirm->setMaximumHeight(40);
#else
    m_btnConfirm->setMinimumHeight(44);
#endif
    m_btnConfirm->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    actionRow->addWidget(m_btnConfirm);
    layout->addLayout(actionRow);

    connect(m_btnSelectImage, &QPushButton::clicked, this, &FeedbackDialog::onSelectImage);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnConfirm, &QPushButton::clicked, this, &QDialog::accept);

#ifdef Q_OS_ANDROID
    setFocusPolicy(Qt::StrongFocus);
    {
        QWidget *host = parentWidget() ? parentWidget()->window() : nullptr;
        const QRect hostRect = host ? host->rect()
                                    : (QGuiApplication::primaryScreen()
                                           ? QGuiApplication::primaryScreen()->geometry()
                                           : QRect(QPoint(0, 0), QSize(390, 844)));
        const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
        const int panelWidth = qBound(292, qRound(available.width() * 0.82), available.width());
        panel->setFixedWidth(panelWidth);
        panel->setMaximumHeight(qMin(available.height() * 62 / 100, 360));
        panel->adjustSize();
        setGeometry(hostRect);
        syncModalShadow(panel, shadowNear, shadowFar);
        QTimer::singleShot(0, this, [panel, shadowNear, shadowFar]() {
            syncModalShadow(panel, shadowNear, shadowFar);
        });
    }
    QTimer::singleShot(0, this, [this]() {
        if (m_textEdit) {
            m_textEdit->clearFocus();
        }
        setFocus(Qt::OtherFocusReason);
    });
#endif
}

void FeedbackDialog::onSelectImage()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("选择反馈图片"),
        QString(),
        QStringLiteral("图片文件 (*.png *.jpg *.jpeg *.bmp);;所有文件 (*)"));

    if (path.isEmpty())
        return;

    m_imagePath = path;
    const QPixmap pix(path);
    if (!pix.isNull()) {
        const QSize previewSize = m_lblImagePreview->size();
        m_lblImagePreview->setPixmap(
            pix.scaled(previewSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        m_lblImagePreview->setProperty("class", QStringLiteral("selected"));
        m_lblImagePreview->style()->unpolish(m_lblImagePreview);
        m_lblImagePreview->style()->polish(m_lblImagePreview);
        m_btnSelectImage->setText(QStringLiteral("已选择图片"));
    }
}

void FeedbackDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
#ifdef Q_OS_ANDROID
    QPainter painter(this);
    painter.fillRect(rect(), QColor(24, 28, 25, 118));
#endif
}

bool FeedbackDialog::eventFilter(QObject *watched, QEvent *event)
{
#ifdef Q_OS_ANDROID
    if (watched == m_textEdit) {
        if (event->type() == QEvent::FocusIn) {
            setKeyboardLifted(true);
        } else if (event->type() == QEvent::FocusOut) {
            setKeyboardLifted(false);
        }
    }
#endif
    return QDialog::eventFilter(watched, event);
}

void FeedbackDialog::setKeyboardLifted(bool lifted)
{
#ifdef Q_OS_ANDROID
    if (!m_outerLayout)
        return;

    m_outerLayout->setContentsMargins(18, 0, 18, lifted ? 140 : 48);
    m_outerLayout->invalidate();
    updateGeometry();
    QTimer::singleShot(0, this, [this]() {
        syncModalShadow(m_panel, m_shadowNear, m_shadowFar);
    });
#else
    Q_UNUSED(lifted)
#endif
}

QString FeedbackDialog::feedbackText() const
{
    return m_textEdit->toPlainText().trimmed();
}

QString FeedbackDialog::imagePath() const
{
    return m_imagePath;
}
