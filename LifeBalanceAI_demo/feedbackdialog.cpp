#include "feedbackdialog.h"

#include <QFileDialog>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QScreen>
#include <QSizePolicy>
#include <QStyle>
#include <QVBoxLayout>

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
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(QStringLiteral(
        "QDialog#feedbackDialog{background:#FEFEFE;border:1px solid #E8E8E8;border-radius:12px;}"
        "QLabel#feedbackPrompt{background:transparent;color:#333333;font-size:14px;"
        "font-family:\"MiSans Medium\",\"MiSans\",\"Noto Sans SC\",\"Microsoft YaHei UI\";font-weight:500;}"
        "QLabel#feedbackImagePreview{background:#FFFFFF;border:1px dashed #D9D9D9;"
        "border-radius:8px;color:#999999;font-size:13px;}"
    ));

#ifdef Q_OS_ANDROID
    QRect available(0, 0, 390, 760);
    if (QScreen *screen = QGuiApplication::primaryScreen())
        available = screen->availableGeometry();

    const int dialogWidth = qMax(280, qMin(available.width() - 72, 320));
    const int dialogHeight = qMax(320, qMin(available.height() - 320, 360));
    setMinimumSize(0, 0);
    setFixedSize(dialogWidth, dialogHeight);
#else
    setMinimumSize(380, 380);
    resize(420, 430);
#endif

    auto *layout = new QVBoxLayout(this);
#ifdef Q_OS_ANDROID
    layout->setContentsMargins(12, 12, 12, 12);
    layout->setSpacing(8);
#else
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);
#endif

    auto *label = new QLabel(
        QStringLiteral("请描述您对「%1」计划的执行感受或改进建议：").arg(slotTitle),
        this);
    label->setObjectName(QStringLiteral("feedbackPrompt"));
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    layout->addWidget(label);

    m_textEdit = new QTextEdit(this);
#ifdef Q_OS_ANDROID
    m_textEdit->setMinimumHeight(92);
    m_textEdit->setMaximumHeight(104);
#else
    m_textEdit->setMinimumHeight(150);
#endif
    m_textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    if (initialText.isEmpty())
        m_textEdit->setPlaceholderText(QStringLiteral("请输入反馈内容..."));
    else
        m_textEdit->setPlainText(initialText);
    layout->addWidget(m_textEdit);

    auto *imgRow = new QHBoxLayout;
#ifdef Q_OS_ANDROID
    imgRow->setSpacing(8);
#else
    imgRow->setSpacing(12);
#endif

    m_btnSelectImage = new QPushButton(QStringLiteral("选择图片（可选）"), this);
    m_btnSelectImage->setProperty("class", QStringLiteral("secondary"));
#ifdef Q_OS_ANDROID
    m_btnSelectImage->setMinimumSize(138, 44);
    m_btnSelectImage->setMaximumHeight(46);
#else
    m_btnSelectImage->setMinimumSize(180, 58);
    m_btnSelectImage->setMaximumHeight(58);
#endif
    m_btnSelectImage->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    imgRow->addWidget(m_btnSelectImage);

    m_lblImagePreview = new QLabel(this);
    m_lblImagePreview->setObjectName(QStringLiteral("feedbackImagePreview"));
#ifdef Q_OS_ANDROID
    m_lblImagePreview->setFixedSize(92, 44);
#else
    m_lblImagePreview->setFixedSize(118, 58);
#endif
    m_lblImagePreview->setAlignment(Qt::AlignCenter);
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

    m_btnCancel = new QPushButton(QString::fromUtf8("取消"), this);
    m_btnCancel->setProperty("class", QStringLiteral("secondary"));
#ifdef Q_OS_ANDROID
    m_btnCancel->setMinimumHeight(42);
    m_btnCancel->setMaximumHeight(46);
#else
    m_btnCancel->setMinimumHeight(44);
#endif
    m_btnCancel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    actionRow->addWidget(m_btnCancel);

    m_btnConfirm = new QPushButton(QStringLiteral("确认提交"), this);
    m_btnConfirm->setProperty("class", QStringLiteral("primary"));
#ifdef Q_OS_ANDROID
    m_btnConfirm->setMinimumHeight(42);
    m_btnConfirm->setMaximumHeight(46);
#else
    m_btnConfirm->setMinimumHeight(44);
#endif
    m_btnConfirm->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    actionRow->addWidget(m_btnConfirm);
    layout->addLayout(actionRow);

    connect(m_btnSelectImage, &QPushButton::clicked, this, &FeedbackDialog::onSelectImage);
    connect(m_btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_btnConfirm, &QPushButton::clicked, this, &QDialog::accept);
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

QString FeedbackDialog::feedbackText() const
{
    return m_textEdit->toPlainText().trimmed();
}

QString FeedbackDialog::imagePath() const
{
    return m_imagePath;
}
