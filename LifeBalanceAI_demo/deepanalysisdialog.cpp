#include "deepanalysisdialog.h"
#include "designtokens.h"
#include "platformlayoutpolicy.h"

#include <QFrame>
#include <QGuiApplication>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QScreen>
#include <QScrollArea>
#include <QSpacerItem>
#include <QTimer>
#include <QVBoxLayout>
#ifdef Q_OS_ANDROID
#include <QScroller>
#include <QScrollerProperties>
#endif

namespace {

QLabel *makeWrappedLabel(const QString &text, const QString &objectName, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setObjectName(objectName);
    label->setWordWrap(true);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return label;
}

void addMajorTitle(QVBoxLayout *layout, QWidget *parent, const QString &title)
{
    auto *label = makeWrappedLabel(title, QStringLiteral("analysisMajorTitle"), parent);
    layout->addWidget(label);
}

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

} // namespace

DeepAnalysisDialog::DeepAnalysisDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void DeepAnalysisDialog::setupUi()
{
    setWindowTitle(QString::fromUtf8("AI 健康洞察报告"));
    setObjectName(QStringLiteral("analysisDialog"));
#ifdef Q_OS_ANDROID
    setWindowFlags(Qt::Widget | Qt::FramelessWindowHint);
    setWindowModality(Qt::ApplicationModal);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAutoFillBackground(false);
#endif
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(QStringLiteral(
        "#analysisDialog{background:transparent;}"
        "QFrame#analysisDialogPanel{"
        "  background:%1;"
        "  border:1px solid %2;"
        "  border-radius:%3px;"
        "}"
).arg(DesignTokens::bgCard(),
          DesignTokens::border(),
          QString::number(DesignTokens::RadiusXl)));

#ifdef Q_OS_ANDROID
    const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
    const QSize preferred(qMin(available.width() * 90 / 100, 420),
                          qMin(available.height() * 72 / 100, 640));
    const QSize target = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogSizeForRole(
        LifeBalanceAI::Ui::PlatformLayoutPolicy::DialogRole::LargeContent,
        preferred);
    QWidget *host = parentWidget() ? parentWidget()->window() : nullptr;
    const QRect hostRect = host ? host->rect()
                                : QRect(QPoint(0, 0),
                                        QGuiApplication::primaryScreen()
                                            ? QGuiApplication::primaryScreen()->geometry().size()
                                            : QSize(390, 844));
    setGeometry(hostRect);
#else
    resize(520, 680);
    setMaximumSize(620, 760);
#endif

    auto *outerLayout = new QVBoxLayout(this);
#ifdef Q_OS_ANDROID
    outerLayout->setContentsMargins(18, 0, 18, 0);
    outerLayout->setSpacing(0);
    m_shadowFar = createModalShadow(this, QStringLiteral("analysisShadowFar"), 9);
    m_shadowNear = createModalShadow(this, QStringLiteral("analysisShadowNear"), 18);
    m_panel = new QFrame(this);
    m_panel->setObjectName(QStringLiteral("analysisDialogPanel"));
    m_panel->setAttribute(Qt::WA_StyledBackground, true);
    m_panel->setFixedSize(target.boundedTo(available.size()));
    outerLayout->addWidget(m_panel, 0, Qt::AlignCenter);
#else
    m_panel = new QFrame(this);
    m_panel->setObjectName(QStringLiteral("analysisDialogPanel"));
    m_panel->setAttribute(Qt::WA_StyledBackground, true);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(m_panel);
#endif

    auto *mainLayout = new QVBoxLayout(m_panel);
#ifdef Q_OS_ANDROID
    mainLayout->setContentsMargins(16, 14, 16, 14);
    mainLayout->setSpacing(10);
#else
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(12);
#endif

    auto *titleLabel = makeWrappedLabel(QString::fromUtf8("AI 健康洞察报告"),
                                        QStringLiteral("analysisDialogTitle"),
                                        this);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    mainLayout->addWidget(titleLabel);

    auto *hintLabel = makeWrappedLabel(QString::fromUtf8("以下内容仅供习惯管理参考，不作为医疗诊断。"),
                                       QStringLiteral("analysisDialogHint"),
                                       this);
    mainLayout->addWidget(hintLabel);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setObjectName(QStringLiteral("analysisDialogScrollArea"));
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#ifdef Q_OS_ANDROID
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (m_scrollArea->viewport()) {
        m_scrollArea->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
        QScroller::grabGesture(m_scrollArea->viewport(), QScroller::TouchGesture);
        QScroller *scroller = QScroller::scroller(m_scrollArea->viewport());
        QScrollerProperties props = scroller->scrollerProperties();
        props.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy,
                              QScrollerProperties::OvershootAlwaysOff);
        props.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy,
                              QScrollerProperties::OvershootAlwaysOff);
        props.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.8);
        props.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.35);
        props.setScrollMetric(QScrollerProperties::DragStartDistance, 0.005);
        props.setScrollMetric(QScrollerProperties::FrameRate,
                              QScrollerProperties::Fps60);
        scroller->setScrollerProperties(props);
    }
#else
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif

    m_contentWidget = new QWidget(m_scrollArea);
    m_contentWidget->setObjectName(QStringLiteral("analysisDialogContent"));
    auto *contentLayout = new QVBoxLayout(m_contentWidget);
#ifdef Q_OS_ANDROID
    contentLayout->setContentsMargins(0, 2, 0, 4);
    contentLayout->setSpacing(10);
#else
    contentLayout->setContentsMargins(2, 4, 2, 6);
    contentLayout->setSpacing(12);
#endif

    m_scrollArea->setWidget(m_contentWidget);
    mainLayout->addWidget(m_scrollArea, 1);

    auto *btnClose = new QPushButton(QString::fromUtf8("关闭"), this);
    btnClose->setProperty("class", QStringLiteral("primary"));
#ifdef Q_OS_ANDROID
    btnClose->setMinimumHeight(44);
    btnClose->setMaximumHeight(48);
#else
    btnClose->setMinimumHeight(46);
#endif
    mainLayout->addWidget(btnClose);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);

#ifdef Q_OS_ANDROID
    syncShadowLayers();
    QTimer::singleShot(0, this, [this]() { syncShadowLayers(); });
#else
    if (QScreen *screen = QGuiApplication::primaryScreen()) {
        const QRect avail = screen->availableGeometry();
        move(avail.center() - rect().center());
    }
#endif
}

void DeepAnalysisDialog::syncShadowLayers()
{
#ifdef Q_OS_ANDROID
    if (!m_panel || !m_shadowNear || !m_shadowFar || m_panel->geometry().isEmpty())
        return;

    const QRect panelRect = m_panel->geometry();
    m_shadowFar->setGeometry(panelRect.adjusted(-1, 0, 5, 4).translated(4, 10));
    m_shadowNear->setGeometry(panelRect.adjusted(0, 0, 3, 2).translated(2, 5));
    m_shadowFar->show();
    m_shadowNear->show();
    m_shadowFar->lower();
    m_shadowNear->raise();
    m_shadowNear->stackUnder(m_panel);
    m_panel->raise();
#endif
}

void DeepAnalysisDialog::paintEvent(QPaintEvent *event)
{
#ifdef Q_OS_ANDROID
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), QColor(24, 28, 25, 118));
#else
    QDialog::paintEvent(event);
#endif
}

QLabel *DeepAnalysisDialog::addSection(const QString &title, const QString &content)
{
    auto *layout = qobject_cast<QVBoxLayout *>(m_contentWidget->layout());
    if (!layout)
        return nullptr;

    auto *card = new QFrame(m_contentWidget);
    card->setObjectName(QStringLiteral("analysisSectionCard"));
    auto *cardLayout = new QVBoxLayout(card);
#ifdef Q_OS_ANDROID
    cardLayout->setContentsMargins(12, 10, 12, 10);
    cardLayout->setSpacing(5);
#else
    cardLayout->setContentsMargins(14, 12, 14, 12);
    cardLayout->setSpacing(6);
#endif

    auto *titleLabel = makeWrappedLabel(title, QStringLiteral("analysisSectionTitle"), card);
    cardLayout->addWidget(titleLabel);

    auto *contentLabel = makeWrappedLabel(content, QStringLiteral("analysisSectionBody"), card);
    cardLayout->addWidget(contentLabel);

    layout->addWidget(card);
    return contentLabel;
}

void DeepAnalysisDialog::setAnalysisResult(const LifeBalanceAI::Models::DeepAnalysisResult &result)
{
    QLayout *layout = m_contentWidget->layout();
    QLayoutItem *child = nullptr;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }

    auto *contentLayout = qobject_cast<QVBoxLayout *>(layout);
    if (!contentLayout)
        return;

    addMajorTitle(contentLayout, m_contentWidget, QString::fromUtf8("营养观察"));
    if (!result.nutritionProtein.isEmpty())
        addSection(QString::fromUtf8("蛋白质"), result.nutritionProtein);
    if (!result.nutritionCarbs.isEmpty())
        addSection(QString::fromUtf8("碳水化合物"), result.nutritionCarbs);
    if (!result.nutritionFat.isEmpty())
        addSection(QString::fromUtf8("脂肪"), result.nutritionFat);
    if (!result.nutritionAdvice.isEmpty())
        addSection(QString::fromUtf8("综合建议"), result.nutritionAdvice);

    addMajorTitle(contentLayout, m_contentWidget, QString::fromUtf8("运动观察"));
    if (!result.exerciseChange.isEmpty())
        addSection(QString::fromUtf8("调整方向"), result.exerciseChange);
    if (!result.exerciseConfidence.isEmpty())
        addSection(QString::fromUtf8("完成信心"), result.exerciseConfidence);

    if (!result.recipes.isEmpty()) {
        addMajorTitle(contentLayout, m_contentWidget, QString::fromUtf8("推荐食谱"));
        for (const auto &recipe : result.recipes) {
            auto *card = new QFrame(m_contentWidget);
            card->setObjectName(QStringLiteral("analysisRecipeCard"));
            auto *cardLayout = new QVBoxLayout(card);
#ifdef Q_OS_ANDROID
            cardLayout->setContentsMargins(12, 10, 12, 10);
            cardLayout->setSpacing(5);
#else
            cardLayout->setContentsMargins(14, 12, 14, 12);
            cardLayout->setSpacing(6);
#endif
            const QString recipeName = recipe.name.trimmed().isEmpty()
                ? QString::fromUtf8("推荐食谱")
                : recipe.name.trimmed();
            cardLayout->addWidget(makeWrappedLabel(recipeName,
                                                   QStringLiteral("analysisRecipeName"),
                                                   card));
            if (!recipe.reason.trimmed().isEmpty()) {
                cardLayout->addWidget(makeWrappedLabel(recipe.reason.trimmed(),
                                                       QStringLiteral("analysisRecipeReason"),
                                                       card));
            }

            const QString calories = recipe.calories > 0
                ? QString::fromUtf8("约 %1 kcal").arg(recipe.calories)
                : QString::fromUtf8("热量待估算");
            cardLayout->addWidget(makeWrappedLabel(calories,
                                                   QStringLiteral("analysisRecipeMeta"),
                                                   card));
            contentLayout->addWidget(card);
        }
    }

    if (!result.lazySlot.isEmpty() || !result.lazyAdvice.isEmpty()) {
        addMajorTitle(contentLayout, m_contentWidget, QString::fromUtf8("执行习惯"));
        if (!result.lazySlot.isEmpty())
            addSection(QString::fromUtf8("容易松动的时段"), result.lazySlot);
        if (!result.lazyAdvice.isEmpty())
            addSection(QString::fromUtf8("改善建议"), result.lazyAdvice);
    }

    contentLayout->addItem(new QSpacerItem(20, 8, QSizePolicy::Minimum, QSizePolicy::Fixed));
}
