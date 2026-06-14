#include "deepanalysisdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QPushButton>
#include <QTableWidget>
#include <QHeaderView>
#include <QFont>
#include <QGuiApplication>
#include <QScreen>
#ifdef Q_OS_ANDROID
#include <QScroller>
#include <QScrollerProperties>
#endif

DeepAnalysisDialog::DeepAnalysisDialog(QWidget *parent)
    : QDialog(parent)
{
    setupUi();
}

void DeepAnalysisDialog::setupUi()
{
    setWindowTitle(tr("AI 深度分析报告"));
    setObjectName(QStringLiteral("analysisDialog"));
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(QStringLiteral(
        "#analysisDialog{"
        "  background:#FFFFFF;"
        "  border:2px solid #4CAF7F;"
        "  border-radius:8px;"
        "}"
    ));
#ifdef Q_OS_ANDROID
    const int screenW = QGuiApplication::primaryScreen()->availableGeometry().width();
    setMinimumSize(qMin(screenW - 36, 540), 400);
    resize(qMin(screenW - 24, 540), 680);
#else
    setMinimumSize(480, 600);
    resize(520, 680);
#endif

    // Center on screen (Qt dialogs don't always auto-center on Android)
    {
        const QRect avail = QGuiApplication::primaryScreen()->availableGeometry();
        move(avail.center() - rect().center());
    }

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(18, 18, 18, 18);
    mainLayout->setSpacing(12);

    // Title
    auto *titleLabel = new QLabel(tr("🔬 AI 深度分析报告"), this);
    titleLabel->setObjectName(QStringLiteral("analysisDialogTitle"));
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    // Scroll area for content
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
#ifdef Q_OS_ANDROID
    if (m_scrollArea->viewport()) {
        m_scrollArea->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
        QScroller::grabGesture(m_scrollArea->viewport(), QScroller::TouchGesture);
        QScroller *s = QScroller::scroller(m_scrollArea->viewport());
        QScrollerProperties sp = s->scrollerProperties();
        sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy,
                           QScrollerProperties::OvershootAlwaysOff);
        sp.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy,
                           QScrollerProperties::OvershootAlwaysOff);
        sp.setScrollMetric(QScrollerProperties::DecelerationFactor, 0.8);
        sp.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.3);
        sp.setScrollMetric(QScrollerProperties::DragStartDistance, 0.005);
        sp.setScrollMetric(QScrollerProperties::FrameRate,
                           QScrollerProperties::Fps60);
        s->setScrollerProperties(sp);
    }
#endif

    m_contentWidget = new QWidget;
    m_contentWidget->setLayout(new QVBoxLayout);
    m_contentWidget->layout()->setContentsMargins(6, 6, 6, 6);
    m_contentWidget->layout()->setSpacing(12);

    m_scrollArea->setWidget(m_contentWidget);
    mainLayout->addWidget(m_scrollArea);

    // Close button
    auto *btnClose = new QPushButton(tr("关闭"), this);
    btnClose->setProperty("class", QStringLiteral("primary"));
    btnClose->setMinimumHeight(44);
    mainLayout->addWidget(btnClose);
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
}

QLabel *DeepAnalysisDialog::addSection(const QString &title, const QString &content)
{
    auto *layout = qobject_cast<QVBoxLayout *>(m_contentWidget->layout());

    auto *titleLabel = new QLabel(title, m_contentWidget);
    titleLabel->setObjectName(QStringLiteral("analysisSectionTitle"));
    QFont f = titleLabel->font();
    f.setPointSize(11);
    f.setBold(true);
    titleLabel->setFont(f);
    layout->addWidget(titleLabel);

    auto *contentLabel = new QLabel(content, m_contentWidget);
    contentLabel->setObjectName(QStringLiteral("analysisSectionCard"));
    contentLabel->setWordWrap(true);
    layout->addWidget(contentLabel);

    return contentLabel;
}

void DeepAnalysisDialog::setAnalysisResult(const LifeBalanceAI::Models::DeepAnalysisResult &result)
{
    // Clear existing content
    QLayout *layout = m_contentWidget->layout();
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget())
            child->widget()->deleteLater();
        delete child;
    }

    // ── Nutrition Analysis ──
    auto *nutriTitle = new QLabel(tr("📊 营养分析"), m_contentWidget);
    nutriTitle->setObjectName(QStringLiteral("analysisMajorTitle"));
    QFont f = nutriTitle->font();
    f.setPointSize(13);
    f.setBold(true);
    nutriTitle->setFont(f);
    layout->addWidget(nutriTitle);

    if (!result.nutritionProtein.isEmpty())
        addSection(tr("🥩 蛋白质"), result.nutritionProtein);
    if (!result.nutritionCarbs.isEmpty())
        addSection(tr("🍚 碳水化合物"), result.nutritionCarbs);
    if (!result.nutritionFat.isEmpty())
        addSection(tr("🧈 脂肪"), result.nutritionFat);
    if (!result.nutritionAdvice.isEmpty())
        addSection(tr("💡 综合营养建议"), result.nutritionAdvice);

    // ── Exercise Analysis ──
    auto *exTitle = new QLabel(tr("🏃 运动分析"), m_contentWidget);
    exTitle->setObjectName(QStringLiteral("analysisMajorTitle"));
    QFont f2 = exTitle->font();
    f2.setPointSize(13);
    f2.setBold(true);
    exTitle->setFont(f2);
    layout->addWidget(exTitle);

    if (!result.exerciseChange.isEmpty())
        addSection(tr("📈 调整建议"), result.exerciseChange);
    if (!result.exerciseConfidence.isEmpty())
        addSection(tr("🎯 完成信心"), result.exerciseConfidence);

    // ── Recipe Recommendations ──
    if (!result.recipes.isEmpty()) {
        auto *recipeTitle = new QLabel(tr("🍳 推荐食谱"), m_contentWidget);
        recipeTitle->setObjectName(QStringLiteral("analysisMajorTitle"));
        QFont f3 = recipeTitle->font();
        f3.setPointSize(13);
        f3.setBold(true);
        recipeTitle->setFont(f3);
        layout->addWidget(recipeTitle);

        auto *table = new QTableWidget(result.recipes.size(), 3, m_contentWidget);
        table->setHorizontalHeaderLabels({tr("菜名"), tr("推荐原因"), tr("热量(kcal)")});
        table->horizontalHeader()->setStretchLastSection(true);
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setMinimumHeight(80 + result.recipes.size() * 30);

        for (int i = 0; i < result.recipes.size(); ++i) {
            table->setItem(i, 0, new QTableWidgetItem(result.recipes[i].name));
            table->setItem(i, 1, new QTableWidgetItem(result.recipes[i].reason));
            table->setItem(i, 2, new QTableWidgetItem(QString::number(result.recipes[i].calories)));
        }
        layout->addWidget(table);
    }

    // ── Lazy Slot ──
    if (!result.lazySlot.isEmpty() || !result.lazyAdvice.isEmpty()) {
        auto *lazyTitle = new QLabel(tr("😴 偷懒分析"), m_contentWidget);
        lazyTitle->setObjectName(QStringLiteral("analysisMajorTitle"));
        QFont f4 = lazyTitle->font();
        f4.setPointSize(13);
        f4.setBold(true);
        lazyTitle->setFont(f4);
        layout->addWidget(lazyTitle);

        if (!result.lazySlot.isEmpty())
            addSection(tr("🔍 最常偷懒时段"), result.lazySlot);
        if (!result.lazyAdvice.isEmpty())
            addSection(tr("💪 改善建议"), result.lazyAdvice);
    }

    // Add spacer at bottom
    layout->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
}
