#include "mainwindow.h"



#include "ui_mainwindow.h"



#include "databasemanager.h"



#include "aimanager.h"



#include "services/userservice.h"
#include "services/authflowservice.h"
#include "services/accountflowservice.h"
#include "services/authsessionservice.h"
#include "services/homeplanservice.h"
#include "services/airesponseparser.h"
#include "services/profileflowservice.h"



#include "feedbackdialog.h"



#include "deepanalysisdialog.h"



#include "animateddialog.h"

#include "designtokens.h"

#include "motionhelper.h"

#include <QVariantAnimation>

#include "thememanager.h"

#include "uifactory.h"

#include "loadingoverlay.h"

#include "welcomeoverlay.h"

#include "circularprogressbar.h"

#include "trendlinechart.h"

#include "calendargridview.h"

#include "mobileshellcontroller.h"
#include "platformlayoutpolicy.h"







#include <QDebug>



#include <QCoreApplication>
#include <QMessageBox>



#include <QSqlQuery>



#include <QSqlError>



#include <QAbstractScrollArea>

#include <QScrollArea>

#include <QTabBar>



#include <QCheckBox>



#include <QPushButton>



#include <QGroupBox>



#include <QLineEdit>



#include <QJsonDocument>



#include <QJsonObject>



#include <QJsonArray>



#include <QDate>



#include <QEasingCurve>



#include <QTimer>



#include <QTextEdit>
#include <QPlainTextEdit>
#include <QFile>



#include <QWheelEvent>

#include <QMouseEvent>
#include <QTouchEvent>



#include <QMap>



#include <QPalette>



#include <QStatusBar>



#include <QLabel>



#include <QHBoxLayout>



#include <QFormLayout>



#include <QGridLayout>



#include <QBoxLayout>



#include <QVBoxLayout>



#include <QTableWidget>



#include <QHeaderView>



#include <QListWidget>

#include <QListView>



#include <QStyleFactory>



#include <QComboBox>
#include <QDialog>



#include <QGraphicsOpacityEffect>

#include <QFont>

#include <QFontDatabase>

#include <QGuiApplication>

#include <QPixmap>

#include <QScreen>

#include <QScrollBar>

#include <QScroller>

#include <QScrollerProperties>

#include <QSpinBox>

#include <QStackedWidget>

#include <QFrame>

#include <algorithm>



namespace {

QString numericPrefix(const QString &text)
{
    QString value = text.trimmed();
    value.remove(QStringLiteral("cm"), Qt::CaseInsensitive);
    value.remove(QStringLiteral("kg"), Qt::CaseInsensitive);
    return value.trimmed();
}

QString uiText(const char *text)
{
    return QString::fromUtf8(text);
}

QLabel *createInlineStatusLabel(const QString &objectName, QWidget *parent)
{
    auto *label = new QLabel(parent);
    label->setObjectName(objectName);
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    label->setMinimumHeight(42);
    label->setContentsMargins(14, 10, 14, 10);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    label->setStyleSheet(QStringLiteral(
        "QLabel#%1{"
        "background:%2;"
        "border:1px solid %3;"
        "border-left:4px solid %4;"
        "border-radius:8px;"
        "color:%5;"
        "font-size:14px;"
        "}").arg(objectName,
                 DesignTokens::primaryLightest(),
                 DesignTokens::border(),
                 DesignTokens::primary(),
                 DesignTokens::textBody()));
    label->hide();
    return label;
}

void relaxAndroidWidthConstraints(QWidget *root)
{
#ifdef Q_OS_ANDROID
    LifeBalanceAI::Ui::PlatformLayoutPolicy::applyMobileScrollPolicy(root);
    if (!root)
        return;

    root->setMinimumWidth(0);
    if (root->maximumWidth() < QWIDGETSIZE_MAX)
        root->setMaximumWidth(QWIDGETSIZE_MAX);

    const auto widgets = root->findChildren<QWidget *>();
    for (QWidget *widget : widgets) {
        if (!widget)
            continue;

        widget->setMinimumWidth(0);
        if (widget->maximumWidth() < QWIDGETSIZE_MAX &&
            !qobject_cast<QPushButton *>(widget)) {
            widget->setMaximumWidth(QWIDGETSIZE_MAX);
        }

        QSizePolicy policy = widget->sizePolicy();
        if (!qobject_cast<QLabel *>(widget) &&
            !qobject_cast<QPushButton *>(widget) &&
            !qobject_cast<QScrollBar *>(widget)) {
            policy.setHorizontalPolicy(QSizePolicy::Expanding);
            widget->setSizePolicy(policy);
        }
        if (auto *label = qobject_cast<QLabel *>(widget)) {
            if (label->wordWrap()) {
                policy = label->sizePolicy();
                policy.setHorizontalPolicy(QSizePolicy::Expanding);
                label->setSizePolicy(policy);
            }
        }
    }
#else
    Q_UNUSED(root);
#endif
}

void installKineticVerticalScroll(QWidget *root)
{
    if (!root)
        return;

    const auto scrollAreas = root->findChildren<QAbstractScrollArea *>();
    for (QAbstractScrollArea *area : scrollAreas) {
        if (!area || !area->viewport())
            continue;

        if (qobject_cast<QTextEdit *>(area) || qobject_cast<QPlainTextEdit *>(area)) {
            area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#ifdef Q_OS_ANDROID
            if (area->verticalScrollBar()) {
                area->verticalScrollBar()->setFixedWidth(0);
                area->verticalScrollBar()->hide();
            }
#endif
            area->setAttribute(Qt::WA_AcceptTouchEvents, true);
            area->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
            QScroller::grabGesture(area->viewport(), QScroller::TouchGesture);

            QScroller *scroller = QScroller::scroller(area->viewport());
            QScrollerProperties properties = scroller->scrollerProperties();
            properties.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.16);
            properties.setScrollMetric(QScrollerProperties::DecelerationFactor, 1.65);
            properties.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.18);
            properties.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
            properties.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve(QEasingCurve::OutQuad));
            properties.setScrollMetric(QScrollerProperties::DragStartDistance, 0.012);
            properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
            properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
            properties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 1.0);
            properties.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0.0);
            properties.setScrollMetric(QScrollerProperties::OvershootScrollTime, 0.0);
            scroller->setScrollerProperties(properties);
            continue;
        }

        area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#ifdef Q_OS_ANDROID
        area->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if (area->verticalScrollBar()) {
            area->verticalScrollBar()->setFixedWidth(0);
            area->verticalScrollBar()->hide();
        }
#endif
        if (!qobject_cast<QTableWidget *>(area))
            area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        area->setAttribute(Qt::WA_AcceptTouchEvents, true);
        area->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
        QScroller::grabGesture(area->viewport(), QScroller::TouchGesture);

        QScroller *scroller = QScroller::scroller(area->viewport());
        QScrollerProperties properties = scroller->scrollerProperties();
        properties.setScrollMetric(QScrollerProperties::DragVelocitySmoothingFactor, 0.16);
        properties.setScrollMetric(QScrollerProperties::DecelerationFactor, 1.65);
        properties.setScrollMetric(QScrollerProperties::MaximumVelocity, 0.18);
        properties.setScrollMetric(QScrollerProperties::MinimumVelocity, 0.0);
        properties.setScrollMetric(QScrollerProperties::ScrollingCurve, QEasingCurve(QEasingCurve::OutQuad));
        properties.setScrollMetric(QScrollerProperties::DragStartDistance, 0.012);
        properties.setScrollMetric(QScrollerProperties::HorizontalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
        properties.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
        properties.setScrollMetric(QScrollerProperties::OvershootDragResistanceFactor, 1.0);
        properties.setScrollMetric(QScrollerProperties::OvershootScrollDistanceFactor, 0.0);
        properties.setScrollMetric(QScrollerProperties::OvershootScrollTime, 0.0);
        scroller->setScrollerProperties(properties);
    }
}

void normalizeAndroidHorizontalPosition(QWidget *root)
{
#ifdef Q_OS_ANDROID
    LifeBalanceAI::Ui::PlatformLayoutPolicy::normalizeHorizontalPosition(root);
    if (!root)
        return;

    const auto areas = root->findChildren<QAbstractScrollArea *>();
    for (QAbstractScrollArea *area : areas) {
        if (!area || qobject_cast<QTableWidget *>(area))
            continue;

        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        if (QScrollBar *bar = area->horizontalScrollBar())
            bar->setValue(0);

        auto *scrollArea = qobject_cast<QScrollArea *>(area);
        if (!scrollArea || !scrollArea->widget() || !scrollArea->viewport())
            continue;

        const int viewportWidth = scrollArea->viewport()->width();
        if (viewportWidth <= 0)
            continue;

        QWidget *content = scrollArea->widget();
        content->setMinimumWidth(0);
        content->setMaximumWidth(viewportWidth);
        content->resize(viewportWidth, qMax(content->height(), content->sizeHint().height()));
    }
#else
    Q_UNUSED(root);
#endif
}

void applyAndroidWindowInsets(QWidget *central)
{
#ifdef Q_OS_ANDROID
    if (!central || !central->layout())
        return;

    LifeBalanceAI::Ui::PlatformLayoutPolicy::applyAndroidEdgeToEdge();
    const int topInset = qMin(LifeBalanceAI::Ui::PlatformLayoutPolicy::topSafeAreaInset(), 44);
    central->setAttribute(Qt::WA_StyledBackground, true);
    central->layout()->setContentsMargins(
        0,
        topInset,
        0,
        0);
    central->layout()->invalidate();
#else
    Q_UNUSED(central);
#endif
}

} // namespace

// Force QTextEdit::document()->setTextWidth() from the nearest QScrollArea
// viewport.  Preferring the viewport over wider ancestors like page_4 avoids
// setting a fixed text width that's wider than the actual text area, which
// would break rendering when the QTextEdit widget itself is narrow.
static void forceTextWidth(QTextEdit *edit)
{
    if (!edit) return;

    // Prefer the edit's own viewport width; never fall back to an outer
    // QScrollArea viewport which is screen-wide and breaks wrapping.
    int w = 0;
    if (edit->viewport())
        w = edit->viewport()->width();
    if (w <= 0)
        w = edit->width();

    // Use the nearest QGroupBox as a reasonable width cap
    for (QWidget *p = edit->parentWidget(); p; p = p->parentWidget()) {
        if (auto *group = qobject_cast<QGroupBox *>(p)) {
            if (group->width() > 0 && (w <= 0 || group->width() < w))
                w = group->width();
            break;
        }
    }

    if (w > 0)
        edit->document()->setTextWidth(w - 8);
    edit->setLineWrapMode(QTextEdit::WidgetWidth);
}







MainWindow::MainWindow(QWidget *parent)



    : QMainWindow(parent)



    , ui(new Ui::MainWindow)



{    ui->setupUi(this);



    // Create CheckinService



    m_checkinService = new LifeBalanceAI::Services::CheckinService(this);

    m_authSessionService = new LifeBalanceAI::Services::AuthSessionService(this);







    // Create FreezeCardService and wire to CheckinService



    m_freezeCardService = new LifeBalanceAI::Services::FreezeCardService(this);



    m_checkinService->setFreezeCardService(m_freezeCardService);







    // Create DeepAnalysisService



    m_deepAnalysisService = new LifeBalanceAI::Services::DeepAnalysisService(this);



        connect(&AIManager::instance(), &AIManager::nicknameGenerated,
            this, &MainWindow::onNicknameGenerated);
connect(m_deepAnalysisService, &LifeBalanceAI::Services::DeepAnalysisService::analysisReady,



            this, &MainWindow::onDeepAnalysisReady);



    connect(m_deepAnalysisService, &LifeBalanceAI::Services::DeepAnalysisService::analysisError,



            this, &MainWindow::onDeepAnalysisError);







    // Create ReportService



    m_reportService = new LifeBalanceAI::Services::ReportService(this);    connect(m_reportService, &LifeBalanceAI::Services::ReportService::reportGenerated,



            this, &MainWindow::onReportReady);



    connect(m_reportService, &LifeBalanceAI::Services::ReportService::exportCompleted,



            this, [this](int reportId, const QString &path) {



        hideLoadingBar();
        AnimatedDialog::info(this, tr("导出成功"), tr("报告已保存到:\n%1").arg(path));



    });



    connect(m_reportService, &LifeBalanceAI::Services::ReportService::reportError,



            this, &MainWindow::onReportError);







    // Day-change auto-refresh: check every 60s if date rolled over



    m_lastRefreshDate = QDate();  // invalid date, forces refresh on first loadMainPage call



    auto *dayTimer = new QTimer(this);



    connect(dayTimer, &QTimer::timeout, this, [this]() {



        QDate today = QDate::currentDate();



        if (today != m_lastRefreshDate) {



            m_lastRefreshDate = today;



            const int homeIndex = ui->page_4 ? ui->stackedWidget->indexOf(ui->page_4) : 3;
            if (m_currentUserId > 0 && ui->stackedWidget->currentIndex() == homeIndex) {



                loadMainPage();



                updatePlanUI();



            }



        }



    });



    dayTimer->start(60000);    // Add "杩斿洖" button on page_3 (for users editing from page_5)



    QPushButton *btnBackPage3 = ui->page_3->findChild<QPushButton *>(QStringLiteral("btnSaveProfile"));



    if (btnBackPage3) {



        QPushButton *btnBack3 = new QPushButton(tr("取消修改"), ui->page_3);



        btnBack3->setObjectName(QStringLiteral("btnBackPage3"));



        btnBack3->setMinimumHeight(40);



        btnBack3->setVisible(false);  // hidden by default, shown only when editing from page_5



        connect(btnBack3, &QPushButton::clicked, this, [this, btnBack3]() {



            btnBack3->setVisible(false);



            m_profileEditFromPage5 = false;



            navigateTo(AppRoute::Profile, false);  // back to page_5



        });        // Put both buttons in a container with equal width (50/50)



        QLayout *parentLayout = btnBackPage3->parentWidget() ? btnBackPage3->parentWidget()->layout() : nullptr;



        if (!parentLayout) {



            QWidget *scrollContent = ui->page_3->findChild<QScrollArea *>() ? ui->page_3->findChild<QScrollArea *>()->widget() : nullptr;



            if (scrollContent)



                parentLayout = scrollContent->layout();



        }



        if (parentLayout) {



            QGridLayout *gridLayout = qobject_cast<QGridLayout *>(parentLayout);



            if (gridLayout) {



                int idx = gridLayout->indexOf(btnBackPage3);



                if (idx >= 0) {



                    int row, col, rowSpan, colSpan;



                    gridLayout->getItemPosition(idx, &row, &col, &rowSpan, &colSpan);



                    gridLayout->removeWidget(btnBackPage3);



                    QWidget *btnContainer = new QWidget(ui->page_3);



                    QHBoxLayout *hbox = new QHBoxLayout(btnContainer);



                    hbox->setContentsMargins(0, 0, 0, 0);



                    hbox->setSpacing(8);



                    btnBackPage3->setParent(btnContainer);



                    hbox->addWidget(btnBackPage3, 1);



                    hbox->addWidget(btnBack3, 1);



                    btnContainer->setLayout(hbox);



                    gridLayout->addWidget(btnContainer, row, col, 1, 2);



                }



            } else {



                QBoxLayout *boxLayout = qobject_cast<QBoxLayout *>(parentLayout);



                if (boxLayout) {



                    int idx = boxLayout->indexOf(btnBackPage3);



                    if (idx >= 0) {



                        boxLayout->removeWidget(btnBackPage3);



                        QWidget *btnContainer = new QWidget(ui->page_3);



                        QHBoxLayout *hbox = new QHBoxLayout(btnContainer);



                        hbox->setContentsMargins(0, 0, 0, 0);



                        hbox->setSpacing(8);



                        btnBackPage3->setParent(btnContainer);



                        hbox->addWidget(btnBackPage3, 1);



                        hbox->addWidget(btnBack3, 1);



                        btnContainer->setLayout(hbox);



                        boxLayout->insertWidget(idx, btnContainer);



                    }



                }



            }



        }



    }







    // Explicitly connect page_5 buttons (bypass connectSlotsByName)



    if (ui->btnSaveProfile5)



        connect(ui->btnSaveProfile5, &QPushButton::clicked, this, &MainWindow::on_btnSaveProfile5_clicked);



    if (ui->btnBackToMain)



        connect(ui->btnBackToMain, &QPushButton::clicked, this, &MainWindow::on_btnBackToMain_clicked);







    // Page 6 (Payment) button connections



    if (ui->btnPay6)



        connect(ui->btnPay6, &QPushButton::clicked, this, &MainWindow::on_btnPay6_clicked);



    if (ui->btnBack6)



        connect(ui->btnBack6, &QPushButton::clicked, this, &MainWindow::on_btnBack6_clicked);







    // Setup page_6 copy (attractive pain-point + benefit style)



    if (ui->lblTitle6)



        ui->lblTitle6->setText(tr("成为律行达人"));



    if (ui->lblPrice6)



        ui->lblPrice6->setText(tr("仅需 9.9 元 / 月"));



    if (ui->lblDesc6) {



        ui->lblDesc6->setText(tr(



        "忘记打卡，连续天数功亏一篏？\n"



        "想要AI深度分析你的饮食与运动？\n"



        "想把健康报告导出为精美图片？\n"



        "\n"



        "成为律行者吧！\n"



        "\n"



        "✅ 每月 3 张冻结卡，守护连续打卡\n"



        "✅ AI 深度分析，精准洞察健康趋势\n"



        "✅ 健康周报导出，记录每一步成长"



    ));



        ui->lblDesc6->setWordWrap(true);



    }



    if (ui->btnPay6)



        ui->btnPay6->setText(tr("立即开通"));







    // ============================================================



    // Set placeholder text for all input fields



    // ============================================================



    ui->editPhoneReg->setPlaceholderText(tr("请输入手机号"));



    ui->editPwdReg->setPlaceholderText(tr("请输入密码"));



    ui->editVerifyCode->setPlaceholderText(tr("请输入4位验证码"));







    ui->editPhoneLogin->setPlaceholderText(tr("请输入手机号"));



    ui->editPwdLogin->setPlaceholderText(tr("请输入密码"));







    // ============================================================



    // Set rich text for link labels (plain text in .ui to avoid



    // UIC HTML parsing errors; set HTML here in C++)



    // ============================================================



    ui->lblGoRegister->setText(



        QStringLiteral("<p style=\"margin:0;color:#999999;font-size:16px;\">还没有账号？</p>"



                       "<p style=\"margin:2px 0 0 0;\">"



                       "<a href=\"goto_register\" style=\"color:#2D7A5E;text-decoration:underline;"



                       "font-size:16px;\">立即注册 →</a></p>"));



    ui->lblRegSubtitle->setText(



        QStringLiteral("<span style=\"color:#666666;font-size:16px;font-family:MiSans;\">已有账号？</span> "



                       "<a href=\"goto_login\" style=\"color:#2D7A5E;text-decoration:underline;"



                       "font-size:16px;font-family:MiSans Medium;\">返回登录 →</a>"));







    // ============================================================



    // Set placeholder text for profile page input fields



    // ============================================================



    ui->editHeight->setPlaceholderText(tr("请输入身高"));



    ui->editWeight->setPlaceholderText(tr("请输入体重"));



    ui->editFood->setPlaceholderText(tr("请输入其他饮食偏好"));



    ui->editSports->setPlaceholderText(tr("请输入其他运动偏好"));



    ui->editTarget->setPlaceholderText(tr("请输入其他目标"));







    // ============================================================



    // Wrap page_3 (profile page) inside a QScrollArea



    // ============================================================



    {



        QWidget *profilePage = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_3"));



        if (profilePage) {



            QLayout *origLayout = profilePage->layout();







            QWidget *scrollContent = new QWidget;



            scrollContent->setLayout(origLayout);
            scrollContent->setMinimumWidth(0);
            scrollContent->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);







            QScrollArea *scrollArea = new QScrollArea;



            scrollArea->setWidgetResizable(true);
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);



            scrollArea->setWidget(scrollContent);



            scrollArea->setFrameShape(QFrame::NoFrame);







            QVBoxLayout *newLayout = new QVBoxLayout(profilePage);



            newLayout->setContentsMargins(0, 0, 0, 0);



            newLayout->addWidget(scrollArea);







            // Legal disclaimer



            QLabel *disclaimer3 = new QLabel(tr("本系统不提供医疗诊断，仅供参考"), profilePage);



            disclaimer3->setAlignment(Qt::AlignCenter);



            disclaimer3->setObjectName(QStringLiteral("legalDisclaimer"));



            newLayout->addWidget(disclaimer3);



        }



    }







    // ============================================================



    // Mobile-friendly: set minimum height for all QPushButton and



    // QCheckBox on page_3 to 40 px



    // ============================================================



    {



        QWidget *profilePage = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_3"));



        if (profilePage) {



            const int minH = 40;



            for (QCheckBox *cb : profilePage->findChildren<QCheckBox *>())



                cb->setMinimumHeight(minH);



            for (QPushButton *pb : profilePage->findChildren<QPushButton *>())



                pb->setMinimumHeight(minH);



        }



    }







    // ============================================================



    // 杩炴帴鐧诲綍/娉ㄥ唽椤垫枃瀛楅摼鎺ワ紙QLabel linkActivated 淇″彿锟?

    // ============================================================



    // 鐧诲綍锟?锟?璺宠浆娉ㄥ唽



    if (QLabel *lbl = ui->page_2->findChild<QLabel *>(



            QStringLiteral("lblGoRegister"))) {



        connect(lbl, &QLabel::linkActivated,



                this, [this](const QString &) {



            navigateTo(AppRoute::Register, false);



        });



    }



    // 娉ㄥ唽椤靛壇鏍囬 锟?璺宠浆鐧诲綍



    if (QLabel *lbl = ui->page->findChild<QLabel *>(



            QStringLiteral("lblRegSubtitle"))) {



        connect(lbl, &QLabel::linkActivated,



                this, [this](const QString &) {



            navigateTo(AppRoute::Login, false);



        });



    }







    // ============================================================



    // ============================================================







    // ============================================================



        // ============================================================



// Connect AIManager signals



    // ============================================================



    connect(&AIManager::instance(), &AIManager::planGenerated,



            this, &MainWindow::onPlanGenerated);



    connect(&AIManager::instance(), &AIManager::partialUpdateGenerated,



            this, &MainWindow::onPartialUpdateGenerated);



    connect(&AIManager::instance(), &AIManager::requestError,



            this, [this](const QString &msg) {



        m_isRequestPending = false;
        hideLoadingBar();
        qWarning() << "AI request failed:" << msg;
        const QString message = AIManager::hasChatApiKey()
            ? uiText("\u0041\u0049 \u670d\u52a1\u6682\u65f6\u4e0d\u53ef\u7528\uff0c\u53ef\u7a0d\u540e\u91cd\u8bd5\u3002")
            : uiText("\u0041\u0049 \u670d\u52a1\u672a\u914d\u7f6e\uff0c\u6682\u65f6\u65e0\u6cd5\u751f\u6210\u3002");
        const AppRoute route = ui && ui->stackedWidget
            ? routeForPageIndex(ui->stackedWidget->currentIndex())
            : AppRoute::Home;
        if (route == AppRoute::Analysis)
            showAnalysisStatus(message);
        else if (route == AppRoute::Report)
            showReportStatus(message);
        else
            showHomePlanStatus(message);



    });







    // ============================================================



    // Wire up all 12 feedback buttons on page_4 via a lambda



    // ============================================================



    struct FeedbackBtnInfo {



        QPushButton *button;



        QTextEdit   *textEdit;



        QString      slotTitle;    // e.g. "今天早餐", "明天运动"



    };







    const QList<FeedbackBtnInfo> feedbackBtns = {



        {ui->btnFeedbackTodayMorning,   ui->txtTodayMorning,   tr("今天早餐")},



        {ui->btnFeedbackTodayLunch,     ui->txtTodayLunch,     tr("今天午餐")},



        {ui->btnFeedbackTodayDinner,    ui->txtTodayDinner,    tr("今天晚餐")},



        {ui->btnFeedbackTodayExercise,  ui->txtTodayExercise,  tr("今天运动")},



        {ui->btnFeedbackTomorrowMorning,  ui->txtTomorrowMorning,  tr("明天早餐")},



        {ui->btnFeedbackTomorrowLunch,    ui->txtTomorrowLunch,    tr("明天午餐")},



        {ui->btnFeedbackTomorrowDinner,   ui->txtTomorrowDinner,   tr("明天晚餐")},



        {ui->btnFeedbackTomorrowExercise, ui->txtTomorrowExercise,  tr("明天运动")},



        {ui->btnFeedbackDayAfterMorning,  ui->txtDayAfterMorning,  tr("后天早餐")},



        {ui->btnFeedbackDayAfterLunch,    ui->txtDayAfterLunch,    tr("后天午餐")},



        {ui->btnFeedbackDayAfterDinner,   ui->txtDayAfterDinner,   tr("后天晚餐")},



        {ui->btnFeedbackDayAfterExercise, ui->txtDayAfterExercise, tr("后天运动")},



    };







    // Store all feedback buttons for later iteration



    m_allFeedbackButtons.clear();



    for (const auto &info : feedbackBtns) {



        connect(info.button, &QPushButton::clicked, this, [this, info]() {



            onFeedbackButtonClicked(info.textEdit, info.slotTitle);



        });



        info.button->setMinimumWidth(112);
        info.button->setMaximumWidth(128);
        info.button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);



        m_allFeedbackButtons.append(info.button);



    }







    // ============================================================



    // 缁熶竴鍚勯〉锟?layout 闂磋窛锛堣ˉ锟?.ui 涓棤娉曠洿鎺ヨ缃殑閮ㄥ垎锟?

    // ============================================================







    // page_3 锟?GroupBox 鍐呴儴 layout 缁熶竴闂磋窛



    {



        QWidget *p3 = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_3"));



        if (p3) {



            for (QGroupBox *gb : p3->findChildren<QGroupBox *>()) {



                if (QLayout *l = gb->layout()) {



                    l->setSpacing(8);



                    l->setContentsMargins(10, 8, 10, 10);



                }



            }



        }



    }







    // 涓鸿祫鏂欓〉 scrollContent 璁剧疆 objectName锛屼娇 QSS 閫夋嫨鍣ㄨ兘绮剧‘鍖归厤



    if (QWidget *p3 = ui->stackedWidget->findChild<QWidget *>(



            QStringLiteral("page_3"))) {



        // 鎵惧埌婊氬姩鍖哄煙鍐呯殑 scrollContent 锟?widget



        for (QScrollArea *sa : p3->findChildren<QScrollArea *>()) {



            if (QWidget *sc = sa->widget()) {



                sc->setObjectName(QStringLiteral("profileScrollContent"));



                // 寮哄埗璧勬枡锟?GroupBox 浣跨敤 Fusion 鏍峰紡锟?

                // 纭繚 Windows 骞冲彴锟?QGroupBox border QSS 鐢熸晥



                for (QGroupBox *gb : sc->findChildren<QGroupBox *>()) {



                    gb->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));



                }



            }



        }



    }







    // page_4 TabWidget 鍐呭悇 GroupBox锛氬唴锟?HBoxLayout 闂磋窛娓呴浂



    // 锛堝弽锟?璋冩暣鎸夐挳绱ц创 QTextEdit锛屽垎闅旂敱 QSS border-left 瀹炵幇锟?

    {



        QWidget *p4 = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_4"));



        if (p4) {



            for (QGroupBox *gb : p4->findChildren<QGroupBox *>()) {



                if (QLayout *l = gb->layout()) {



                    l->setSpacing(0);



                    l->setContentsMargins(0, 0, 0, 0);



                }



            }



            // page_4 锟?VBoxLayout 鏁翠綋闂磋窛



            if (QVBoxLayout *mainL = qobject_cast<QVBoxLayout *>(p4->layout())) {

#ifdef Q_OS_ANDROID
                mainL->setSpacing(4);
                mainL->setContentsMargins(8, 0, 8, 6);
                mainL->setAlignment(Qt::AlignTop);
#else
                mainL->setSpacing(8);
                mainL->setContentsMargins(12, 10, 12, 10);
#endif



            }



        }



    }







    // ============================================================



    // ============================================================



    // Create animated loading label with breathing + typewriter



    // ============================================================



    m_loadingLabel = new QLabel(this);



    m_loadingLabel->setObjectName(QStringLiteral("loadingLabel"));



    m_loadingLabel->hide();



    statusBar()->addPermanentWidget(m_loadingLabel);

    m_loadingOverlay = new LoadingOverlay(ui->centralwidget);
    m_loadingOverlay->hide();







#ifndef Q_OS_ANDROID
    auto *opacityEffect = new QGraphicsOpacityEffect(m_loadingLabel);
    opacityEffect->setOpacity(1.0);
    m_loadingLabel->setGraphicsEffect(opacityEffect);

    m_breathAnim = new QPropertyAnimation(opacityEffect, "opacity");
    m_breathAnim->setDuration(900);
    m_breathAnim->setStartValue(0.62);
    m_breathAnim->setEndValue(1.0);
    m_breathAnim->setLoopCount(-1);
#endif







    m_typewriterTimer = new QTimer(this);



    connect(m_typewriterTimer, &QTimer::timeout, this, [this]() {
        m_typewriterPos = (m_typewriterPos + 1) % 4;
        m_loadingLabel->setText(m_typewriterFullText + QString(m_typewriterPos, QChar('.')));
    });



    statusBar()->showMessage(QString());







    // ============================================================



    // Bottom Navigation Bar



    // ============================================================



    m_bottomNav = new BottomNavBar(centralWidget() ? centralWidget() : this);



    m_sideDrawer = new SideDrawer(centralWidget());



    m_shellController = new MobileShellController(this);
    m_shellController->setShellWidgets(m_bottomNav, ui->stackedWidget, centralWidget());
    m_shellController->setRouteHandler([this](AppRoute route, bool animate) {
        navigateTo(route, animate);
    });
    syncShellRouteIndexes();



    setupSideDrawer();



    m_bottomNav->hide();



    QGridLayout *grid = qobject_cast<QGridLayout *>(centralWidget()->layout());



    if (grid) {
#ifdef Q_OS_ANDROID
        grid->addWidget(m_bottomNav, 1, 0);
        grid->setRowStretch(0, 1);
        grid->setRowMinimumHeight(1, 0);
        grid->setRowStretch(1, 0);
        centralWidget()->installEventFilter(this);
#else
        grid->addWidget(m_bottomNav, 1, 0);
        grid->setRowStretch(0, 1);  // stackedWidget stretches
        grid->setRowMinimumHeight(1, 60);  // nav bar fixed
        grid->setRowStretch(1, 0);
#endif
    }



    connect(m_bottomNav, &BottomNavBar::currentChanged, this, [this](int tab) {
        handleBottomNavTab(tab);
    });

    connect(ui->stackedWidget, &QStackedWidget::currentChanged, this, [this](int index) {
        syncShellRouteIndexes();
        updateBottomNavVisibility(index);
    });











    // ============================================================



    // Programmatically add page_4 widgets (yesterday button,



    // all-done label, 12 adjust-plan buttons)



    // ============================================================



    setupPage4Widgets();







    // Home disclaimer is appended to each plan list instead of being fixed above
    // the bottom navigation. Keeping it in the scroll content saves first-screen
    // space on phones.



    {



        QWidget *p4 = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_4"));



        if (p4) {



            QVBoxLayout *p4Layout = qobject_cast<QVBoxLayout *>(p4->layout());



            if (p4Layout) {
#ifdef Q_OS_ANDROID
                p4Layout->setStretch(2, 1);
                p4Layout->setContentsMargins(14, 8, 14, 8);
                p4Layout->setSpacing(10);
                p4->installEventFilter(this);
#endif



            }



        }



    }



    // ============================================================



    // TabWidget 寮哄埗 Fusion 鏍峰紡锛岀‘锟?QTabBar QSS 锟?Windows 涓嬬敓锟?

    // ============================================================



    ui->tabWidgetDays->setStyle(QStyleFactory::create(QStringLiteral("Fusion")));







    // ============================================================



    // Connect tab widget changes for cross-day restrictions



    // ============================================================



    connect(ui->tabWidgetDays, &QTabWidget::currentChanged,



            this, &MainWindow::onTabChanged);



    applyWarmVisualPolish();
    addRememberLoginControl();

    updateBottomNavVisibility();

#ifdef Q_OS_ANDROID
    QTimer::singleShot(0, this, [this]() {
        applyAndroidWindowInsets(centralWidget());
        if (QScreen *screen = QGuiApplication::primaryScreen())
            resize(screen->geometry().size());
        relaxAndroidWidthConstraints(this);
        if (centralWidget() && centralWidget()->layout())
            centralWidget()->layout()->invalidate();
        if (ui && ui->stackedWidget)
            ui->stackedWidget->updateGeometry();
        updateBottomNavVisibility();
    });
    QTimer::singleShot(250, this, [this]() {
        applyAndroidWindowInsets(centralWidget());
        updateBottomNavVisibility();
    });
#endif

    QTimer::singleShot(0, this, [this]() {
        tryAutoLogin();
    });



}











void MainWindow::addRememberLoginControl()
{
    if (!ui || !ui->page_2 || rememberLoginCheckBox())
        return;

    QFrame *loginCard = ui->page_2->findChild<QFrame *>(QStringLiteral("loginCard"));
    auto *cardLayout = loginCard ? qobject_cast<QVBoxLayout *>(loginCard->layout()) : nullptr;
    if (!cardLayout)
        return;

    auto *remember = new QCheckBox(QString::fromUtf8("\u8bb0\u4f4f\u767b\u5f55\uff0c\u4e0b\u6b21\u81ea\u52a8\u8fdb\u5165"), loginCard);
    remember->setObjectName(QStringLiteral("chkRememberLogin"));
    remember->setChecked(true);
    remember->setCursor(Qt::PointingHandCursor);
    remember->setMinimumHeight(32);
    remember->setStyleSheet(QString());

    int insertIndex = cardLayout->indexOf(ui->btnLogin);
    if (insertIndex < 0)
        insertIndex = cardLayout->count();
    cardLayout->insertWidget(insertIndex, remember);

    // Forgot password link
    QPushButton *btnForgotPwd = new QPushButton(QString::fromUtf8("\u5fd8\u8bb0\u5bc6\u7801\uff1f"), loginCard);
    btnForgotPwd->setFlat(true);
    btnForgotPwd->setCursor(Qt::PointingHandCursor);
    btnForgotPwd->setStyleSheet(QStringLiteral(
        "QPushButton{font-family:\"MiSans Medium\",\"MiSans\",\"Microsoft YaHei UI\";"
        "font-size:13px;color:#999999;border:none;background:transparent;text-align:left;padding:2px 0;}"
        "QPushButton:hover{color:#4CAF7F;}"));
    connect(btnForgotPwd, &QPushButton::clicked, this, &MainWindow::onForgotPassword);
    cardLayout->insertWidget(insertIndex + 2, btnForgotPwd);

    if (ui->editPwdLogin)
        ui->editPwdLogin->setEchoMode(QLineEdit::Password);
}

QCheckBox *MainWindow::rememberLoginCheckBox() const
{
    return ui && ui->page_2
        ? ui->page_2->findChild<QCheckBox *>(QStringLiteral("chkRememberLogin"))
        : nullptr;
}

void MainWindow::saveAutoLoginSessionIfNeeded(int userId, const QString &role)
{
    QCheckBox *remember = rememberLoginCheckBox();
    if (!m_authSessionService)
        return;
    m_authSessionService->saveSession(userId, role, remember && remember->isChecked());
}

void MainWindow::clearAutoLoginSession()
{
    if (m_authSessionService)
        m_authSessionService->clearSession();
}

void MainWindow::logoutToLogin()
{
    clearAutoLoginSession();
    m_currentUserId = -1;
    m_adminUserId = -1;

    if (ui && ui->editPwdLogin)
        ui->editPwdLogin->clear();
    if (rememberLoginCheckBox())
        rememberLoginCheckBox()->setChecked(true);
    if (m_bottomNav)
        m_bottomNav->setVisible(false);

    navigateTo(AppRoute::Login, false);
}

void MainWindow::confirmLogoutToLogin()
{
    if (!AnimatedDialog::confirm(this,
                                 QString::fromUtf8("退出登录"),
                                 QString::fromUtf8("确定要退出当前账号吗？")))
        return;

    logoutToLogin();
}

bool MainWindow::tryAutoLogin()
{
    if (!ui || !ui->stackedWidget || ui->stackedWidget->currentIndex() != 0)
        return false;

    if (!m_authSessionService)
        return false;

    const LifeBalanceAI::Models::AuthFlowResult result = m_authSessionService->tryLoadValidSession();
    if (!result.ok)
        return false;

    if (ui->editPhoneLogin)
        ui->editPhoneLogin->setText(result.phone);
    if (QCheckBox *remember = rememberLoginCheckBox())
        remember->setChecked(true);

    handleAuthenticatedUser(result);
    return true;
}

void MainWindow::handleAuthenticatedUser(const LifeBalanceAI::Models::AuthFlowResult &result)
{
    m_currentUserId = result.userId;

    QString welcomeName = QString::fromUtf8("\u6b22\u8fce\u56de\u6765\n\u4eca\u5929\u4e5f\u4e00\u8d77\u7167\u987e\u597d\u8eab\u4f53");
    if (result.role == QStringLiteral("admin")) {
        welcomeName = QString::fromUtf8("\u6b22\u8fce\u56de\u6765\uff0c\u7ba1\u7406\u5458\n\u4eca\u5929\u4e5f\u4e00\u8d77\u7167\u987e\u597d\u8eab\u4f53");
    } else if (DatabaseManager::instance().hasProfile(m_currentUserId)) {
        LifeBalanceAI::Models::ProfileData prof = DatabaseManager::instance().getProfile(m_currentUserId);
        const QString nickname = prof.nickname.isEmpty() ? QString::fromUtf8("\u4eca\u5929") : prof.nickname;
        welcomeName = QString::fromUtf8("\u6b22\u8fce\u56de\u6765\uff0c") + nickname
                    + QString::fromUtf8("\n\u4eca\u5929\u4e5f\u4e00\u8d77\u7167\u987e\u597d\u8eab\u4f53");
    } else {
        welcomeName = QString::fromUtf8("\u6b22\u8fce\u6765\u5230 LifeBalance AI\n\u4eca\u5929\u4e5f\u4e00\u8d77\u7167\u987e\u597d\u8eab\u4f53");
    }

    qDebug() << "=== Login Success ===";
    qDebug() << "User ID:" << m_currentUserId;
    qDebug() << "Phone:" << result.phone;
    qDebug() << "Role:" << result.role;
    qDebug() << "Auto login:" << result.fromAutoLogin;

    std::function<void()> routeAction = [this, result]() {
        if (result.nextRoute == AppRoute::Admin) {
            qDebug() << "Admin login, navigating to admin panel.";
            navigateTo(AppRoute::Admin, false);
        } else if (result.nextRoute == AppRoute::Home) {
            qDebug() << "Profile exists, navigating to main page (page_4).";
            const bool needsInitialPlan = !DatabaseManager::instance().hasPlan(m_currentUserId);
            if (needsInitialPlan) {
                qDebug() << "No plan found for user" << m_currentUserId
                         << "- scheduling AI plan generation after Home route.";
            }
            loadMainPage();
            // Force synchronous layout of dynamically-created widgets
            // before page_4 becomes visible.  loadMainPage() adds widgets
            // that dirty the layout; without this the resize cascade may
            // not reach QTextEdits inside QScrollAreas before updatePlanUI().
            if (ui->page_4) {
                if (ui->page_4->layout())
                    ui->page_4->layout()->activate();
                QCoreApplication::processEvents();
            }
            setupSideDrawer();
            navigateTo(AppRoute::Home, false);
            if (needsInitialPlan)
                requestAIPlanWhenHomeReady();

        } else {
            qDebug() << "No profile found, navigating to profile page (page_3).";
            populateProfileEditFields(LifeBalanceAI::Models::ProfileData());
            navigateTo(AppRoute::ProfileSetup, false);
        }
    };

    if (result.fromAutoLogin) {
        QTimer::singleShot(0, this, routeAction);
        return;
    }

    showLoginSuccessTransition(welcomeName, routeAction);
}

void MainWindow::showLoginSuccessTransition(const QString &welcomeText, std::function<void()> routeAction)
{
    QWidget *host = centralWidget();
    if (!host) {
        if (routeAction)
            routeAction();
        return;
    }

    auto *overlay = new WelcomeOverlay(host);
#ifdef Q_OS_ANDROID
    overlay->showWelcome(welcomeText, 1900);
    QTimer::singleShot(1550, this, [overlay, routeAction]() {
        if (routeAction)
            routeAction();
        if (overlay)
            overlay->raise();
    });
    return;
#else
    overlay->showWelcome(welcomeText, 1500);

    QTimer::singleShot(80, this, [overlay, routeAction]() {
        if (routeAction)
            routeAction();
        if (overlay)
            overlay->raise();
    });
#endif
}

void MainWindow::syncShellRouteIndexes()
{
    if (!m_shellController || !ui || !ui->stackedWidget)
        return;

    m_shellController->setRouteIndex(AppRoute::Login, 0);
    m_shellController->setRouteIndex(AppRoute::Register, 1);
    m_shellController->setRouteIndex(AppRoute::ProfileSetup, ui->page_3 ? ui->stackedWidget->indexOf(ui->page_3) : 2);
    m_shellController->setRouteIndex(AppRoute::Home, ui->page_4 ? ui->stackedWidget->indexOf(ui->page_4) : 3);
    m_shellController->setRouteIndex(AppRoute::Profile, ui->page_5 ? ui->stackedWidget->indexOf(ui->page_5) : 4);
    m_shellController->setRouteIndex(AppRoute::Payment, ui->page_6 ? ui->stackedWidget->indexOf(ui->page_6) : 5);

    if (m_analysisPageIndex >= 0)
        m_shellController->setRouteIndex(AppRoute::Analysis, m_analysisPageIndex);
    else if (QWidget *analysis = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_analysis")))
        m_shellController->setRouteIndex(AppRoute::Analysis, ui->stackedWidget->indexOf(analysis));

    if (m_reportPageIndex >= 0)
        m_shellController->setRouteIndex(AppRoute::Report, m_reportPageIndex);
    else if (QWidget *reports = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_reports")))
        m_shellController->setRouteIndex(AppRoute::Report, ui->stackedWidget->indexOf(reports));

    if (QWidget *admin = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_admin")))
        m_shellController->setRouteIndex(AppRoute::Admin, ui->stackedWidget->indexOf(admin));
}

AppRoute MainWindow::routeForPageIndex(int pageIndex) const
{
    if (m_shellController)
        return m_shellController->routeForPageIndex(pageIndex);
    return AppRoute::Unknown;
}

int MainWindow::pageIndexForRoute(AppRoute route)
{
    syncShellRouteIndexes();
    if (m_shellController)
        return m_shellController->routeIndex(route);
    return -1;
}

void MainWindow::navigateTo(AppRoute route, bool animate)
{
    if (!ui || !ui->stackedWidget)
        return;

    int targetIndex = -1;
    switch (route) {
    case AppRoute::Analysis:
        targetIndex = setupAnalysisPage();
        syncShellRouteIndexes();
        break;
    case AppRoute::Report:
        targetIndex = setupReportPage();
        syncShellRouteIndexes();
        break;
    case AppRoute::Profile:
        loadPage5();
        targetIndex = pageIndexForRoute(AppRoute::Profile);
        break;
    case AppRoute::Admin:
        loadAdminPage();
        syncShellRouteIndexes();
        targetIndex = pageIndexForRoute(AppRoute::Admin);
        break;
    default:
        targetIndex = pageIndexForRoute(route);
        break;
    }

    if (targetIndex < 0) {
        qWarning() << "navigateTo route failed:" << appRouteName(route);
        return;
    }

    navigateTo(targetIndex, animate);
}



// ------------------------------------------------------------------------



// Animated page switching with slide effect



// ------------------------------------------------------------------------



void MainWindow::animatePageSwitch(int fromIndex, int toIndex, bool forward)
{
    MotionHelper::animateStackedSwitch(ui->stackedWidget, fromIndex, toIndex, forward);
    updateBottomNavVisibility(toIndex);
#ifdef Q_OS_ANDROID
    QTimer::singleShot(0, this, [this]() { normalizeAndroidHorizontalPosition(this); });
    QTimer::singleShot(0, this, [this]() {
        if (m_bottomNav && m_bottomNav->isVisible()) {
            positionBottomNav();
            m_bottomNav->raise();
        }
    });
    QTimer::singleShot(280, this, [this]() {
        normalizeAndroidHorizontalPosition(this);
        if (m_bottomNav && m_bottomNav->isVisible()) {
            positionBottomNav();
            m_bottomNav->raise();
        }
    });
#endif
}







void MainWindow::updateBottomNavVisibility()



{
    updateBottomNavVisibility(ui->stackedWidget->currentIndex());
}





void MainWindow::positionBottomNav()
{
    if (m_shellController)
        m_shellController->positionBottomNav();
}

void MainWindow::updateBottomNavVisibility(int pageIndex)



{
    if (!m_bottomNav)
        return;

    syncShellRouteIndexes();
    if (m_shellController) {
        m_shellController->updateBottomNavForPage(pageIndex);
        qDebug() << "updateBottomNavVisibility: index=" << pageIndex
                 << "route=" << appRouteName(routeForPageIndex(pageIndex))
                 << "visible=" << m_bottomNav->isVisible();
    }

}

void MainWindow::handleBottomNavTab(int tab)
{
    if (!m_shellController)
        return;

    const AppRoute route = m_shellController->routeForBottomTab(tab);
    if (route != AppRoute::Unknown)
        m_shellController->navigateTo(route, true);
}







void MainWindow::navigateTo(int toIndex, bool animate)



{



    int fromIndex = ui->stackedWidget->currentIndex();



    qDebug() << "navigateTo:" << fromIndex << "->" << toIndex << "animate=" << animate;



    if (fromIndex == toIndex) {



        updateBottomNavVisibility(toIndex);

#ifdef Q_OS_ANDROID
        QTimer::singleShot(0, this, [this]() { normalizeAndroidHorizontalPosition(this); });
#endif


        return;



    }



    if (animate) {



        updateBottomNavVisibility(toIndex);



        animatePageSwitch(fromIndex, toIndex, (toIndex > fromIndex));



    } else {



        ui->stackedWidget->setCurrentIndex(toIndex);



        updateBottomNavVisibility();

#ifdef Q_OS_ANDROID
        QTimer::singleShot(0, this, [this]() { normalizeAndroidHorizontalPosition(this); });
#endif


    }

    // Deferred refresh: populate plan content AFTER the resize cascade
    // has fully settled.  With animated navigation setCurrentIndex fires
    // ~180 ms after animatePageSwitch starts; calling updatePlanUI
    // before that (via singleShot(0)) means the textWidth we set gets
    // overwritten by the resize event that setCurrentIndex triggers.
#ifdef Q_OS_ANDROID
    int deferMs = animate ? 200 : 50;
#else
    int deferMs = animate ? 280 : 50;
#endif
    QTimer::singleShot(deferMs, this, [this, toIndex]() {
        if (!ui || !ui->tabWidgetDays) return;
        const int homeIndex = ui->page_4 ? ui->stackedWidget->indexOf(ui->page_4) : 3;
        if (toIndex == homeIndex) {
            updatePlanUI();
        }
    });
}







MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
#ifdef Q_OS_ANDROID
    QTimer::singleShot(0, this, [this]() {
        applyAndroidWindowInsets(centralWidget());
        normalizeAndroidHorizontalPosition(this);
        if (centralWidget() && centralWidget()->layout())
            centralWidget()->layout()->invalidate();
        if (ui && ui->stackedWidget)
            ui->stackedWidget->updateGeometry();
        positionBottomNav();
    });
#endif
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
#ifdef Q_OS_ANDROID
    if (event) {
        if (auto *button = qobject_cast<QPushButton *>(watched)) {
            const bool isPlanAction =
                button->objectName().startsWith(QStringLiteral("btnFeedback")) ||
                button->objectName() == QStringLiteral("btnAdjust");
            if (isPlanAction) {
                if (event->type() == QEvent::MouseButtonPress) {
                    auto *mouse = static_cast<QMouseEvent *>(event);
                    button->setProperty("_lbPressPos", mouse->position().toPoint());
                    button->setProperty("_lbCancelClick", false);
                    button->setProperty("_lbPressValid", true);
                } else if (event->type() == QEvent::MouseMove) {
                    auto *mouse = static_cast<QMouseEvent *>(event);
                    const QPoint pressPos = button->property("_lbPressPos").toPoint();
                    if (!pressPos.isNull() &&
                        (mouse->position().toPoint() - pressPos).manhattanLength() > 24) {
                        button->setProperty("_lbCancelClick", true);
                    }
                } else if (event->type() == QEvent::MouseButtonRelease) {
                    const bool invalidClick = !button->property("_lbPressValid").toBool()
                                            || button->property("_lbCancelClick").toBool();
                    button->setProperty("_lbPressValid", false);
                    if (invalidClick) {
                        button->setProperty("_lbCancelClick", false);
                        event->accept();
                        return true;
                    }
                } else if (event->type() == QEvent::TouchBegin) {
                    auto *touch = static_cast<QTouchEvent *>(event);
                    if (!touch->points().isEmpty()) {
                        button->setProperty("_lbPressPos", touch->points().first().position().toPoint());
                        button->setProperty("_lbCancelClick", false);
                        button->setProperty("_lbPressValid", true);
                    }
                } else if (event->type() == QEvent::TouchUpdate) {
                    auto *touch = static_cast<QTouchEvent *>(event);
                    if (!touch->points().isEmpty()) {
                        const QPoint pressPos = button->property("_lbPressPos").toPoint();
                        if (!pressPos.isNull() &&
                            (touch->points().first().position().toPoint() - pressPos).manhattanLength() > 24) {
                            button->setProperty("_lbCancelClick", true);
                        }
                    }
                } else if (event->type() == QEvent::TouchEnd) {
                    const bool invalidClick = !button->property("_lbPressValid").toBool()
                                            || button->property("_lbCancelClick").toBool();
                    button->setProperty("_lbPressValid", false);
                    if (invalidClick) {
                        button->setProperty("_lbCancelClick", false);
                        event->accept();
                        return true;
                    }
                } else if (event->type() == QEvent::Leave || event->type() == QEvent::TouchCancel) {
                    button->setProperty("_lbPressValid", false);
                    button->setProperty("_lbCancelClick", true);
                }
            }
        }
    }
#endif

#ifdef Q_OS_ANDROID
    Q_UNUSED(watched);
#endif

    if (event && event->type() == QEvent::Wheel && ui && ui->tabWidgetDays) {
        QWidget *watchedWidget = qobject_cast<QWidget *>(watched);
        if (watchedWidget) {
            // Let QTextEdit handle its own wheel events when it has scrollable content
            QTextEdit *textEdit = qobject_cast<QTextEdit *>(watchedWidget);
            if (!textEdit) {
                if (auto *vp = qobject_cast<QWidget *>(watchedWidget))
                    textEdit = qobject_cast<QTextEdit *>(vp->parentWidget());
            }
            if (textEdit && textEdit->verticalScrollBar()
                && textEdit->verticalScrollBar()->maximum() > 0) {
                return QMainWindow::eventFilter(watched, event);
            }

            auto applyWheelToScrollArea = [event](QScrollArea *scrollArea) -> bool {
                if (!scrollArea)
                    return false;

                QScrollBar *bar = scrollArea->verticalScrollBar();
                if (!bar || bar->maximum() <= 0)
                    return false;

                auto *wheelEvent = static_cast<QWheelEvent *>(event);
                const QPoint pixelDelta = wheelEvent->pixelDelta();
                const QPoint angleDelta = wheelEvent->angleDelta();
                const int delta = !pixelDelta.isNull() ? pixelDelta.y() : angleDelta.y();
                if (delta == 0)
                    return true;

                bar->setValue(qBound(bar->minimum(), bar->value() - delta, bar->maximum()));
                event->accept();
                return true;
            };

            const auto planScrollAreas = ui->tabWidgetDays->findChildren<QScrollArea *>(QStringLiteral("planScrollArea"));
            for (QScrollArea *scrollArea : planScrollAreas) {
                if (!scrollArea)
                    continue;

                QWidget *content = scrollArea->widget();
                const bool belongsToPlan =
                    watchedWidget == scrollArea ||
                    watchedWidget == scrollArea->viewport() ||
                    scrollArea->isAncestorOf(watchedWidget) ||
                    (content && (watchedWidget == content || content->isAncestorOf(watchedWidget)));
                if (!belongsToPlan)
                    continue;

                return applyWheelToScrollArea(scrollArea);
            }

            const int homeIndex = ui->page_4 ? ui->stackedWidget->indexOf(ui->page_4) : 3;
            if (ui->stackedWidget && ui->stackedWidget->currentIndex() == homeIndex &&
                ui->page_4 && (watchedWidget == ui->page_4 || ui->page_4->isAncestorOf(watchedWidget))) {
                QWidget *currentTab = ui->tabWidgetDays->currentWidget();
                if (auto *scrollArea = currentTab ? currentTab->findChild<QScrollArea *>(QStringLiteral("planScrollArea")) : nullptr)
                    return applyWheelToScrollArea(scrollArea);
            }
        }
    }

    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::applyWarmVisualPolish()
{
#ifdef Q_OS_ANDROID
    applyAndroidWindowInsets(centralWidget());
    setMinimumSize(0, 0);
    if (QScreen *screen = QGuiApplication::primaryScreen())
        resize(screen->geometry().size());
    if (centralWidget()) {
        centralWidget()->setMinimumSize(0, 0);
        centralWidget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    if (ui->stackedWidget) {
        ui->stackedWidget->setMinimumSize(0, 0);
        ui->stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }
    statusBar()->hide();
    menuBar()->hide();
#else
    setMinimumSize(470, 535);
#endif
    setWindowTitle(QStringLiteral("LifeBalance AI"));
    relaxAndroidWidthConstraints(this);

    auto clearInlineStyles = [](QWidget *root) {
        if (!root)
            return;
        const auto widgets = root->findChildren<QWidget *>();
        for (QWidget *widget : widgets) {
            if (qobject_cast<QPushButton *>(widget) ||
                qobject_cast<QLineEdit *>(widget) ||
                qobject_cast<QTextEdit *>(widget) ||
                qobject_cast<QComboBox *>(widget) ||
                qobject_cast<QGroupBox *>(widget) ||
                qobject_cast<QTableWidget *>(widget)) {
                widget->setStyleSheet(QString());
            }
        }
    };

    clearInlineStyles(ui->stackedWidget);

#ifdef Q_OS_ANDROID
    for (QComboBox *combo : findChildren<QComboBox *>()) {
        if (!combo)
            continue;
        auto *listView = qobject_cast<QListView *>(combo->view());
        if (!listView) {
            listView = new QListView(combo);
            combo->setView(listView);
        }
        combo->setMaxVisibleItems(6);
        if (listView) {
            listView->setFrameShape(QFrame::NoFrame);
            listView->setLineWidth(0);
            if (QWidget *popupWindow = listView->window()) {
                popupWindow->setWindowFlag(Qt::FramelessWindowHint, true);
                popupWindow->setAttribute(Qt::WA_StyledBackground, true);
                popupWindow->setStyleSheet(QStringLiteral(
                    "background:#FFFFFF;"
                    "border:1px solid #A8E6C3;"
                    "border-radius:8px;"));
            }
            listView->setStyleSheet(QStringLiteral(
                "QListView {"
                "background:#FFFFFF;"
                "border:1px solid #A8E6C3;"
                "border-radius:0;"
                "padding:4px;"
                "outline:0;"
                "selection-background-color:#A8E6C3;"
                "selection-color:#1A1A1A;"
                "}"
                "QListView::item { min-height:32px; padding:5px 10px; }"
                "QListView::item:selected { background:#A8E6C3; color:#1A1A1A; }"));
        }
    }
#endif

    auto tuneSpacer = [](QBoxLayout *layout, int index, int height, QSizePolicy::Policy verticalPolicy = QSizePolicy::Fixed) {
        if (!layout || index < 0 || index >= layout->count())
            return;
        QLayoutItem *item = layout->itemAt(index);
        if (!item || !item->spacerItem())
            return;
        item->spacerItem()->changeSize(20, height, QSizePolicy::Minimum, verticalPolicy);
        layout->setStretch(index, verticalPolicy == QSizePolicy::Expanding ? 1 : 0);
        layout->invalidate();
    };

    if (ui->lblLoginLogo) {
        QPixmap hero = UiFactory::loadPixmap(QStringLiteral(":/assets/app_logo.png"));
        ui->lblLoginLogo->setText(QString());
        ui->lblLoginLogo->setMinimumHeight(168);
        ui->lblLoginLogo->setMaximumHeight(184);
        ui->lblLoginLogo->setAlignment(Qt::AlignCenter);
        if (!hero.isNull()) {
            ui->lblLoginLogo->setPixmap(hero.scaled(164, 164, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }

    if (ui->label_5) {
        ui->label_5->setText(QStringLiteral("LifeBalance AI"));
        const QStringList families = QFontDatabase::families();
        const QStringList displayCandidates = {
            QStringLiteral("MiSans"),
            QStringLiteral("MiSans Regular"),
            QStringLiteral("PingFang SC"),
            QStringLiteral("Noto Sans SC")
        };
        for (const QString &candidate : displayCandidates) {
            if (families.contains(candidate, Qt::CaseInsensitive)) {
                QFont logoFont = ui->label_5->font();
                logoFont.setFamily(candidate);
                logoFont.setWeight(QFont::Normal);
                logoFont.setPointSize(29);
                ui->label_5->setFont(logoFont);
                break;
            }
        }
    }
    if (ui->lblLoginSubtitle)
        ui->lblLoginSubtitle->setText(QStringLiteral("把健康照顾成每天一点点的安心"));
    if (ui->lblRegTitle)
        ui->lblRegTitle->setText(QStringLiteral("开始你的健康陪伴"));

    if (auto *loginLayout = qobject_cast<QVBoxLayout *>(ui->page_2->layout())) {
#ifdef Q_OS_ANDROID
        loginLayout->setContentsMargins(18, 8, 18, 18);
        loginLayout->setSpacing(6);
        tuneSpacer(loginLayout, 0, 10);
        tuneSpacer(loginLayout, 4, 4);
        tuneSpacer(loginLayout, 6, 10);
        tuneSpacer(loginLayout, 8, 8, QSizePolicy::Expanding);
#else
        loginLayout->setContentsMargins(32, 28, 32, 22);
        loginLayout->setSpacing(8);
        tuneSpacer(loginLayout, 0, 42);
        tuneSpacer(loginLayout, 4, 16);
        tuneSpacer(loginLayout, 6, 16);
        tuneSpacer(loginLayout, 8, 16, QSizePolicy::Expanding);
#endif
    }

    if (QFrame *loginCard = ui->page_2->findChild<QFrame *>(QStringLiteral("loginCard"))) {
        loginCard->setMinimumWidth(0);
        loginCard->setMaximumWidth(QWIDGETSIZE_MAX);
        loginCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        if (auto *cardLayout = qobject_cast<QVBoxLayout *>(loginCard->layout())) {
#ifdef Q_OS_ANDROID
            cardLayout->setContentsMargins(16, 16, 16, 14);
#else
            cardLayout->setContentsMargins(20, 18, 20, 18);
#endif
            cardLayout->setSpacing(10);
        }
    }

    if (auto *regLayout = qobject_cast<QVBoxLayout *>(ui->page->layout())) {
#ifdef Q_OS_ANDROID
        regLayout->setContentsMargins(18, 22, 18, 18);
#else
        regLayout->setContentsMargins(32, 24, 32, 22);
#endif
        regLayout->setSpacing(8);
        if (QLabel *asset = ui->page->findChild<QLabel *>(QStringLiteral("registerHeroAsset"))) {
            regLayout->removeWidget(asset);
            asset->deleteLater();
        }
        for (int i = 0; i < regLayout->count(); ++i) {
            if (!regLayout->itemAt(i) || !regLayout->itemAt(i)->spacerItem())
                continue;
            const bool lastSpacer = (i == regLayout->count() - 1);
            regLayout->itemAt(i)->spacerItem()->changeSize(20, lastSpacer ? 16 : 10,
                                                           QSizePolicy::Minimum,
                                                           lastSpacer ? QSizePolicy::Expanding : QSizePolicy::Fixed);
            regLayout->setStretch(i, lastSpacer ? 1 : 0);
        }
    }

    if (ui->btnSendVerifyCode) {
#ifdef Q_OS_ANDROID
        ui->btnSendVerifyCode->setMinimumWidth(104);
        ui->btnSendVerifyCode->setMaximumWidth(120);
#else
        ui->btnSendVerifyCode->setMinimumWidth(148);
        ui->btnSendVerifyCode->setMaximumWidth(176);
#endif
        ui->btnSendVerifyCode->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    if (ui->hLayoutVerify) {
#ifdef Q_OS_ANDROID
        ui->hLayoutVerify->setSpacing(8);
        ui->hLayoutVerify->setStretch(0, 1);
        ui->hLayoutVerify->setStretch(1, 0);
#endif
    }

    for (QWidget *page : {ui->page_3, ui->page_4, ui->page_5, ui->page_6}) {
        if (auto *layout = qobject_cast<QVBoxLayout *>(page->layout())) {
            layout->setContentsMargins(20, 16, 20, 18);
            layout->setSpacing(14);
        }
    }

    if (ui->tabWidgetDays) {
        ui->tabWidgetDays->setDocumentMode(true);
        ui->tabWidgetDays->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    if (ui->page_6 && !ui->page_6->findChild<QLabel *>(QStringLiteral("upgradeHeroAsset"))) {
        if (auto *layout = qobject_cast<QVBoxLayout *>(ui->page_6->layout())) {
            auto *asset = UiFactory::assetLabel(QStringLiteral(":/assets/ai_analysis.png"),
#ifdef Q_OS_ANDROID
                                                176,
#else
                                                144,
#endif
                                                ui->page_6);
            asset->setObjectName(QStringLiteral("upgradeHeroAsset"));
            asset->setMinimumHeight(
#ifdef Q_OS_ANDROID
                176
#else
                144
#endif
            );
            asset->setMaximumHeight(
#ifdef Q_OS_ANDROID
                190
#else
                160
#endif
            );
            layout->insertWidget(0, asset);
        }
    }

    if (ui->page_6) {
        if (auto *layout = qobject_cast<QVBoxLayout *>(ui->page_6->layout())) {
#ifdef Q_OS_ANDROID
            layout->setContentsMargins(18, 18, 18, 18);
            layout->setSpacing(12);
            if (QLayoutItem *firstSpacer = layout->itemAt(1)) {
                if (firstSpacer->spacerItem())
                    firstSpacer->spacerItem()->changeSize(20, 4, QSizePolicy::Minimum, QSizePolicy::Fixed);
            }
#else
            layout->setContentsMargins(24, 24, 24, 24);
            layout->setSpacing(14);
#endif
        }
        if (ui->lblTitle6) {
            QFont titleFont = ui->lblTitle6->font();
            titleFont.setFamily(QStringLiteral("MiSans"));
            titleFont.setWeight(QFont::Bold);
#ifdef Q_OS_ANDROID
            titleFont.setPointSize(24);
            ui->lblTitle6->setMinimumHeight(48);
#else
            titleFont.setPointSize(26);
            ui->lblTitle6->setMinimumHeight(52);
#endif
            ui->lblTitle6->setFont(titleFont);
            ui->lblTitle6->setStyleSheet(QStringLiteral("color:#1A1A1A;background:transparent;padding:0;"));
            ui->lblTitle6->setAlignment(Qt::AlignCenter);
        }
        if (ui->lblDesc6) {
            QFont descFont = ui->lblDesc6->font();
#ifdef Q_OS_ANDROID
            descFont.setPointSize(14);
#else
            descFont.setPointSize(15);
#endif
            ui->lblDesc6->setFont(descFont);
            ui->lblDesc6->setStyleSheet(QStringLiteral("color:#333333;background:transparent;padding:4px 0;"));
        }
        if (ui->btnPay6) {
#ifdef Q_OS_ANDROID
            ui->btnPay6->setMinimumHeight(50);
            ui->btnPay6->setMaximumHeight(54);
#else
            ui->btnPay6->setMinimumHeight(48);
#endif
            UiFactory::setClass(ui->btnPay6, QStringLiteral("primary"));
        }
    }

    UiFactory::applyWarmPolish(ui->stackedWidget);

    auto reflowChoiceGrid = [](QGroupBox *box, QLineEdit *extraEdit, int columns) {
        if (!box || columns <= 0)
            return;
        auto *grid = qobject_cast<QGridLayout *>(box->layout());
        if (!grid)
            return;

        struct Entry {
            int row = 0;
            int column = 0;
            QCheckBox *checkBox = nullptr;
        };
        QList<Entry> entries;
        QWidget *editWidget = extraEdit;
        for (int i = 0; i < grid->count(); ++i) {
            int row = 0;
            int column = 0;
            int rowSpan = 0;
            int columnSpan = 0;
            grid->getItemPosition(i, &row, &column, &rowSpan, &columnSpan);
            QWidget *widget = grid->itemAt(i) ? grid->itemAt(i)->widget() : nullptr;
            if (auto *check = qobject_cast<QCheckBox *>(widget))
                entries.append({row, column, check});
        }
        std::sort(entries.begin(), entries.end(), [](const Entry &a, const Entry &b) {
            return a.row == b.row ? a.column < b.column : a.row < b.row;
        });

        while (QLayoutItem *item = grid->takeAt(0))
            delete item;

        grid->setContentsMargins(8, 12, 8, 8);
        grid->setHorizontalSpacing(8);
        grid->setVerticalSpacing(6);
        for (int i = 0; i < entries.size(); ++i) {
            QCheckBox *check = entries.at(i).checkBox;
            check->setMinimumWidth(0);
            check->setMinimumHeight(24);
            check->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            grid->addWidget(check, i / columns, i % columns);
        }
        if (editWidget) {
            editWidget->setMinimumWidth(0);
            editWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            grid->addWidget(editWidget, (entries.size() + columns - 1) / columns, 0, 1, columns);
        }
        for (int column = 0; column < columns; ++column)
            grid->setColumnStretch(column, 1);
    };

    reflowChoiceGrid(ui->groupBoxFood, ui->editFood, 2);
    reflowChoiceGrid(ui->groupBoxSports, ui->editSports, 2);
    reflowChoiceGrid(ui->groupBoxTarget, ui->editTarget, 2);

    if (ui->page_3) {
        for (auto *scrollArea : ui->page_3->findChildren<QScrollArea *>()) {
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            if (QWidget *content = scrollArea->widget()) {
                content->setMinimumWidth(0);
                content->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
            }
        }
    }

    if (ui->page_4) {
        if (auto *layout = qobject_cast<QVBoxLayout *>(ui->page_4->layout())) {
#ifdef Q_OS_ANDROID
            const int bottomMargin = 8;
            layout->setContentsMargins(14, 8, 14, bottomMargin);
            layout->setSpacing(10);
            layout->setAlignment(Qt::AlignTop);
#else
            layout->setContentsMargins(20, 16, 20, 12);
            layout->setSpacing(12);
#endif
        }
        if (ui->frameGoal) {
            ui->frameGoal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            if (auto *goalLayout = qobject_cast<QVBoxLayout *>(ui->frameGoal->layout())) {
#ifdef Q_OS_ANDROID
                goalLayout->setContentsMargins(14, 10, 14, 10);
                goalLayout->setSpacing(6);
#else
                goalLayout->setContentsMargins(16, 14, 16, 14);
                goalLayout->setSpacing(8);
#endif

                if (!ui->frameGoal->findChild<QWidget *>(QStringLiteral("goalHeaderRow"))) {
                    if (ui->lblGoalTitle) {
                        const int titleIndex = goalLayout->indexOf(ui->lblGoalTitle);
                        if (titleIndex >= 0) {
                            QLayoutItem *titleItem = goalLayout->takeAt(titleIndex);
                            delete titleItem;
                        }
                    }

                    auto *headerRow = new QWidget(ui->frameGoal);
                    headerRow->setObjectName(QStringLiteral("goalHeaderRow"));
                    headerRow->setCursor(Qt::PointingHandCursor);
                    auto *headerLayout = new QHBoxLayout(headerRow);
                    headerLayout->setContentsMargins(0, 0, 0, 0);
                    headerLayout->setSpacing(8);

                    if (ui->lblGoalTitle) {
                        ui->lblGoalTitle->setParent(headerRow);
                        ui->lblGoalTitle->setCursor(Qt::PointingHandCursor);
                        headerLayout->addWidget(ui->lblGoalTitle, 1);
                    }

                    m_btnGoalToggle = new QPushButton(tr("展开"), headerRow);
                    m_btnGoalToggle->setObjectName(QStringLiteral("btnGoalToggle"));
                    m_btnGoalToggle->setCursor(Qt::PointingHandCursor);
#ifdef Q_OS_ANDROID
                    m_btnGoalToggle->setMinimumSize(58, 28);
                    m_btnGoalToggle->setMaximumSize(66, 30);
                    headerRow->setMinimumHeight(30);
#else
                    m_btnGoalToggle->setMinimumSize(76, 40);
                    m_btnGoalToggle->setMaximumHeight(44);
                    headerRow->setMinimumHeight(46);
#endif
                    headerLayout->addWidget(m_btnGoalToggle, 0, Qt::AlignRight | Qt::AlignVCenter);
                    goalLayout->insertWidget(0, headerRow);

                    connect(m_btnGoalToggle, &QPushButton::clicked, this, [this]() {
                        m_goalExpanded = !m_goalExpanded;
                        updateGoalCollapseState();
                        if (m_goalExpanded && m_goalContentEdit && !m_goalText.isEmpty()) {
                            if (ui->page_4 && ui->page_4->layout()) {
                                ui->page_4->layout()->invalidate();
                                ui->page_4->layout()->activate();
                            }
                            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

                            // setWidgetResizable(true) may not resize content on the
                            // first hidden→visible transition on Android.
                            if (auto *gs = ui->frameGoal->findChild<QScrollArea *>(QStringLiteral("goalContentScroll"))) {
                                if (gs->viewport() && gs->widget()) {
                                    const int vpW = gs->viewport()->width();
                                    if (vpW > 0 && gs->widget()->width() != vpW) {
                                        gs->widget()->setMinimumWidth(0);
                                        gs->widget()->setMaximumWidth(vpW);
                                        gs->widget()->resize(vpW, gs->widget()->height());
                                        if (gs->widget()->layout())
                                            gs->widget()->layout()->activate();
                                    }
                                }
                            }

                            QTimer::singleShot(0, this, [this]() {
                                if (!m_goalExpanded || !m_goalContentEdit || m_goalText.isEmpty())
                                    return;
                                m_goalContentEdit->setPlainText(m_goalText);
                                forceTextWidth(m_goalContentEdit);
                            });
                        }
                    });
                } else if (!m_btnGoalToggle) {
                    m_btnGoalToggle = ui->frameGoal->findChild<QPushButton *>(QStringLiteral("btnGoalToggle"));
                }

                if (ui->lblGoalContent &&
                    !ui->frameGoal->findChild<QScrollArea *>(QStringLiteral("goalContentScroll"))) {
                    const int contentIndex = goalLayout->indexOf(ui->lblGoalContent);
                    if (contentIndex >= 0) {
                        QLayoutItem *contentItem = goalLayout->takeAt(contentIndex);
                        delete contentItem;
                    }

                    // Hide the original QLabel — QLabel caches word-wrap layout at the
                    // width when setText() runs and never recalculates.  Replace it with
                    // a read-only QTextEdit whose resizeEvent correctly updates
                    // QTextDocument::textWidth, so text reflows after any resize.
                    ui->lblGoalContent->hide();

                    auto *contentHost = new QWidget(ui->frameGoal);
                    contentHost->setObjectName(QStringLiteral("goalContentHost"));
                    auto *contentLayout = new QVBoxLayout(contentHost);
                    contentLayout->setContentsMargins(0, 0, 0, 0);
                    contentLayout->setSpacing(0);

                    m_goalContentEdit = new QTextEdit(contentHost);
                    m_goalContentEdit->setObjectName(QStringLiteral("goalContentEdit"));
                    m_goalContentEdit->setReadOnly(true);
                    m_goalContentEdit->setTextInteractionFlags(Qt::NoTextInteraction);
                    m_goalContentEdit->setFrameShape(QFrame::NoFrame);
                    m_goalContentEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    m_goalContentEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
                    m_goalContentEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
                    m_goalContentEdit->setMinimumHeight(0);
                    m_goalContentEdit->setMaximumHeight(QWIDGETSIZE_MAX);
                    m_goalContentEdit->setFocusPolicy(Qt::NoFocus);
                    // Inherit the label's font and styling
                    m_goalContentEdit->setFont(ui->lblGoalContent->font());
                    m_goalContentEdit->document()->setDefaultFont(ui->lblGoalContent->font());
                    m_goalContentEdit->document()->setDocumentMargin(4);
                    QPalette pal = m_goalContentEdit->palette();
                    pal.setColor(QPalette::Base, Qt::transparent);
                    m_goalContentEdit->setPalette(pal);
                    contentLayout->addWidget(m_goalContentEdit);

                    auto *goalScroll = new QScrollArea(ui->frameGoal);
                    goalScroll->setObjectName(QStringLiteral("goalContentScroll"));
                    goalScroll->setWidgetResizable(true);
                    goalScroll->setFrameShape(QFrame::NoFrame);
                    goalScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#ifdef Q_OS_ANDROID
                    goalScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
                    if (goalScroll->viewport()) {
                        goalScroll->viewport()->setAttribute(Qt::WA_AcceptTouchEvents, true);
                        QScroller::grabGesture(goalScroll->viewport(), QScroller::TouchGesture);
                        QScroller *gs = QScroller::scroller(goalScroll->viewport());
                        QScrollerProperties gsp = gs->scrollerProperties();
                        gsp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy,
                                           QScrollerProperties::OvershootAlwaysOff);
                        gs->setScrollerProperties(gsp);
                    }
#else
                    goalScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
                    goalScroll->setWidget(contentHost);
                    goalLayout->insertWidget(1, goalScroll);
                }
            }
        }
#ifdef Q_OS_ANDROID
        const QList<QPair<QGroupBox *, QString>> planTitles = {
            {ui->groupTodayMorning, QStringLiteral("早餐")},
            {ui->groupTodayLunch, QStringLiteral("午餐")},
            {ui->groupTodayDinner, QStringLiteral("晚餐")},
            {ui->groupTodayExercise, QStringLiteral("运动")},
            {ui->groupTomorrowMorning, QStringLiteral("早餐")},
            {ui->groupTomorrowLunch, QStringLiteral("午餐")},
            {ui->groupTomorrowDinner, QStringLiteral("晚餐")},
            {ui->groupTomorrowExercise, QStringLiteral("运动")},
            {ui->groupDayAfterMorning, QStringLiteral("早餐")},
            {ui->groupDayAfterLunch, QStringLiteral("午餐")},
            {ui->groupDayAfterDinner, QStringLiteral("晚餐")},
            {ui->groupDayAfterExercise, QStringLiteral("运动")}
        };
        for (const auto &entry : planTitles) {
            if (entry.first)
                entry.first->setTitle(entry.second);
        }
#endif

        auto installPlanWheelFilters = [this](QScrollArea *scrollArea) {
            if (!scrollArea)
                return;

            scrollArea->installEventFilter(this);
            if (scrollArea->viewport())
                scrollArea->viewport()->installEventFilter(this);
            if (QWidget *content = scrollArea->widget()) {
                content->installEventFilter(this);
                const auto children = content->findChildren<QWidget *>();
                for (QWidget *child : children) {
                    child->installEventFilter(this);
                    if (auto *childArea = qobject_cast<QAbstractScrollArea *>(child)) {
                        if (childArea->viewport())
                            childArea->viewport()->installEventFilter(this);
                    }
                }
            }
        };

        auto ensurePlanTabScroll = [this, &installPlanWheelFilters](QWidget *tabPage) {
            if (!tabPage || tabPage->findChild<QScrollArea *>(QStringLiteral("planScrollArea")))
                return;

            auto *outerLayout = qobject_cast<QVBoxLayout *>(tabPage->layout());
            if (!outerLayout)
                return;

            auto *content = new QWidget(tabPage);
            content->setObjectName(QStringLiteral("planScrollContent"));
            content->setMinimumWidth(0);
            content->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

            auto *contentLayout = new QVBoxLayout(content);
#ifdef Q_OS_ANDROID
            contentLayout->setContentsMargins(2, 6, 2, 10);
            contentLayout->setSpacing(10);
#else
            contentLayout->setContentsMargins(14, 28, 14, 42);
            contentLayout->setSpacing(58);
#endif

            while (QLayoutItem *item = outerLayout->takeAt(0)) {
                if (QWidget *widget = item->widget()) {
                    widget->setParent(content);
                    contentLayout->addWidget(widget);
                    delete item;
                } else {
                    contentLayout->addItem(item);
                }
            }

            auto *disclaimer = new QLabel(tr("本系统不提供医疗诊断，仅供参考"), content);
            disclaimer->setObjectName(QStringLiteral("legalDisclaimer"));
            disclaimer->setAlignment(Qt::AlignCenter);
#ifdef Q_OS_ANDROID
            contentLayout->addSpacing(18);
            disclaimer->setMinimumHeight(24);
            disclaimer->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
#endif
            contentLayout->addWidget(disclaimer);
#ifdef Q_OS_ANDROID
            contentLayout->addSpacing(LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavPagePadding());
#endif

            auto *scrollArea = new QScrollArea(tabPage);
            scrollArea->setObjectName(QStringLiteral("planScrollArea"));
            scrollArea->setWidgetResizable(true);
            scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustIgnored);
            scrollArea->setMinimumHeight(0);
            scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
            scrollArea->setFrameShape(QFrame::NoFrame);
            scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#ifdef Q_OS_ANDROID
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
#else
            scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#endif
            scrollArea->setWidget(content);

            outerLayout->setContentsMargins(0, 0, 0, 0);
            outerLayout->setSpacing(0);
            outerLayout->addWidget(scrollArea);
            installPlanWheelFilters(scrollArea);
        };

        ensurePlanTabScroll(ui->tabToday);
        ensurePlanTabScroll(ui->tabTomorrow);
        ensurePlanTabScroll(ui->tabDayAfter);

        if (ui->tabWidgetDays) {
            ui->tabWidgetDays->setMinimumHeight(540);
            ui->tabWidgetDays->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
#ifdef Q_OS_ANDROID
            ui->tabWidgetDays->tabBar()->setExpanding(true);
            ui->tabWidgetDays->tabBar()->setUsesScrollButtons(false);
            ui->tabWidgetDays->setMaximumHeight(QWIDGETSIZE_MAX);
#endif
        }

        for (QScrollArea *scrollArea : ui->tabWidgetDays->findChildren<QScrollArea *>(QStringLiteral("planScrollArea")))
            installPlanWheelFilters(scrollArea);

        for (QGroupBox *group : ui->tabWidgetDays->findChildren<QGroupBox *>()) {
            group->setProperty("class", QStringLiteral("planCard"));
#ifdef Q_OS_ANDROID
            group->setMinimumHeight(132);
            group->setMaximumHeight(138);
#else
            group->setMinimumHeight(218);
            group->setMaximumHeight(242);
#endif
            group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
            if (auto *row = qobject_cast<QHBoxLayout *>(group->layout())) {
#ifdef Q_OS_ANDROID
                row->setContentsMargins(12, 10, 12, 10);
                row->setSpacing(10);
#else
                row->setContentsMargins(16, 22, 16, 16);
                row->setSpacing(24);
#endif

                QPushButton *feedback = nullptr;
                QPushButton *adjust = nullptr;
                const auto buttons = group->findChildren<QPushButton *>();
                for (QPushButton *button : buttons) {
                    if (!button)
                        continue;
                    if (button->objectName().startsWith(QStringLiteral("btnFeedback")))
                        feedback = button;
                    else if (button->objectName() == QStringLiteral("btnAdjust"))
                        adjust = button;
                }

                QWidget *actionColumn = group->findChild<QWidget *>(QStringLiteral("planActionColumn"));
                if (!actionColumn && feedback && adjust) {
                    row->removeWidget(feedback);
                    row->removeWidget(adjust);

                    actionColumn = new QWidget(group);
                    actionColumn->setObjectName(QStringLiteral("planActionColumn"));
                    auto *actionLayout = new QVBoxLayout(actionColumn);
                    actionLayout->setContentsMargins(0, 0, 0, 0);
                    actionLayout->setSpacing(8);
                    actionLayout->addWidget(feedback);
                    actionLayout->addWidget(adjust);
                    actionLayout->addStretch();
                    row->addWidget(actionColumn, 0, Qt::AlignTop);
                }

                if (actionColumn) {
#ifdef Q_OS_ANDROID
                    actionColumn->setMinimumWidth(58);
                    actionColumn->setMaximumWidth(66);
                    actionColumn->setMinimumHeight(72);
#else
                    actionColumn->setMinimumWidth(112);
                    actionColumn->setMaximumWidth(128);
                    actionColumn->setMinimumHeight(120);
#endif
                    actionColumn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
                    if (auto *actionLayout = qobject_cast<QVBoxLayout *>(actionColumn->layout())) {
#ifdef Q_OS_ANDROID
                        actionLayout->setSpacing(8);
#else
                        actionLayout->setSpacing(24);
#endif
                    }
                }

                if (QTextEdit *textEdit = group->findChild<QTextEdit *>()) {
                    row->setStretchFactor(textEdit, 1);
                    row->setAlignment(textEdit, Qt::AlignTop);
                }
            }
            group->style()->unpolish(group);
            group->style()->polish(group);
        }

        for (QTextEdit *textEdit : ui->tabWidgetDays->findChildren<QTextEdit *>()) {
            textEdit->setProperty("class", QStringLiteral("planText"));
#ifdef Q_OS_ANDROID
            textEdit->setMinimumHeight(84);
            textEdit->setMaximumHeight(90);
#else
            textEdit->setMinimumHeight(120);
#endif
            textEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
            textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
            textEdit->setReadOnly(true);
            textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
            textEdit->setFocusPolicy(Qt::NoFocus);
            QFont planFont = textEdit->font();
            planFont.setWeight(QFont::Normal);
#ifdef Q_OS_ANDROID
            planFont.setPointSize(11);
#endif
            textEdit->setFont(planFont);
            textEdit->document()->setDefaultFont(planFont);
            textEdit->document()->setDocumentMargin(4);
#ifdef Q_OS_ANDROID
            if (textEdit->verticalScrollBar()) {
                textEdit->verticalScrollBar()->setFixedWidth(0);
                textEdit->verticalScrollBar()->hide();
            }
            if (textEdit->viewport())
                QScroller::grabGesture(textEdit->viewport(), QScroller::TouchGesture);
#endif
            if (textEdit->viewport())
                textEdit->viewport()->installEventFilter(this);
            textEdit->style()->unpolish(textEdit);
            textEdit->style()->polish(textEdit);
        }

        for (QPushButton *button : ui->tabWidgetDays->findChildren<QPushButton *>()) {
            if (button->objectName().startsWith(QStringLiteral("btnFeedback")) ||
                button->objectName() == QStringLiteral("btnAdjust")) {
#ifdef Q_OS_ANDROID
                button->setMinimumSize(56, 28);
                button->setMaximumSize(66, 30);
#else
                button->setMinimumSize(112, 48);
                button->setMaximumSize(128, 52);
#endif
                button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            }
        }

        for (QScrollArea *scrollArea : ui->tabWidgetDays->findChildren<QScrollArea *>(QStringLiteral("planScrollArea"))) {
            if (QWidget *content = scrollArea->widget()) {
                if (auto *contentLayout = qobject_cast<QVBoxLayout *>(content->layout())) {
                    contentLayout->activate();
                    const int groupCount = content->findChildren<QGroupBox *>().size();
#ifdef Q_OS_ANDROID
                    const int fixedHeight = 6 + 10 + qMax(1, groupCount) * 138
                                          + qMax(0, groupCount - 1) * 10
                                          + 24 + 18;
#else
                    const int fixedHeight = 28 + 42 + qMax(1, groupCount) * 242 + qMax(0, groupCount - 1) * 58;
#endif
                    content->setMinimumHeight(fixedHeight);
                    content->setMaximumHeight(fixedHeight);
                    content->resize(qMax(scrollArea->viewport()->width(), 1), fixedHeight);
                }
            }
            scrollArea->verticalScrollBar()->setSingleStep(48);
            scrollArea->verticalScrollBar()->setPageStep(qMax(120, scrollArea->viewport()->height() - 48));
#ifdef Q_OS_ANDROID
            scrollArea->verticalScrollBar()->setFixedWidth(0);
            scrollArea->verticalScrollBar()->hide();
#endif
            installPlanWheelFilters(scrollArea);
        }

        if (auto *layout = qobject_cast<QVBoxLayout *>(ui->page_4->layout())) {
            if (ui->frameGoal)
                layout->setStretchFactor(ui->frameGoal, 0);
            if (ui->tabWidgetDays)
                layout->setStretchFactor(ui->tabWidgetDays, 1);
        }
        ui->page_4->installEventFilter(this);
        for (QWidget *child : ui->page_4->findChildren<QWidget *>())
            child->installEventFilter(this);
        updateGoalCollapseState();
    }

    installKineticVerticalScroll(ui->stackedWidget);
}


void MainWindow::updateGoalCollapseState()
{
    if (!ui || !ui->frameGoal)
        return;

QScrollArea *goalScroll = ui->frameGoal->findChild<QScrollArea *>(QStringLiteral("goalContentScroll"));

#ifdef Q_OS_ANDROID
    const int expandedScrollH = 96;
    const int collapsedFrameH = 64;
    const int expandedFrameH = 156;
#else
    const int expandedScrollH = 184;
    const int collapsedFrameH = 120;
    const int expandedFrameH = 286;
#endif
    const int targetScrollH = m_goalExpanded ? expandedScrollH : 0;
    const int targetFrameH = m_goalExpanded ? expandedFrameH : collapsedFrameH;

#ifdef Q_OS_ANDROID
    auto homeTabTargetHeight = [this, targetFrameH]() {
        int viewportH = height();
        if (QScreen *screen = QGuiApplication::primaryScreen())
            viewportH = viewportH > 0 ? qMin(viewportH, screen->geometry().height())
                                      : screen->geometry().height();
        if (centralWidget() && centralWidget()->height() > 0)
            viewportH = qMin(viewportH, centralWidget()->height());
        const int navH = (m_bottomNav && m_bottomNav->isVisible())
                             ? LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavHeight()
                             : 0;
        const int topBarH = 94;
        const int spacingBudget = 28;
        const int stackH = qMax(0, viewportH - navH);
        const int desired = stackH - topBarH - targetFrameH - spacingBudget;
        return m_goalExpanded ? qBound(330, desired, 420)
                               : qBound(460, desired, 540);
    };
    const int targetTabH = ui->tabWidgetDays ? homeTabTargetHeight() : 0;
#endif

    if (m_btnGoalToggle)
        m_btnGoalToggle->setText(m_goalExpanded ? tr("收起") : tr("展开"));

    if (!goalScroll) {
        ui->frameGoal->setMinimumHeight(targetFrameH);
        ui->frameGoal->setMaximumHeight(targetFrameH);
        ui->frameGoal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
#ifdef Q_OS_ANDROID
        if (ui->tabWidgetDays && targetTabH > 0) {
            ui->tabWidgetDays->setMinimumHeight(targetTabH);
            ui->tabWidgetDays->setMaximumHeight(QWIDGETSIZE_MAX);
            ui->tabWidgetDays->updateGeometry();
        }
#endif
        return;
    }

    const int currentScrollH = goalScroll->isVisible() ? goalScroll->height() : 0;
    const int currentFrameH = ui->frameGoal->height() > 0
        ? ui->frameGoal->height()
        : (m_goalExpanded ? collapsedFrameH : expandedFrameH);
    const int startScrollH = qBound(0, currentScrollH, qMax(targetScrollH, currentScrollH));
    const int startFrameH = qBound(collapsedFrameH, currentFrameH, expandedFrameH);
#ifdef Q_OS_ANDROID
    const int currentTabH = ui->tabWidgetDays && ui->tabWidgetDays->height() > 0
        ? ui->tabWidgetDays->height()
        : (ui->tabWidgetDays ? ui->tabWidgetDays->minimumHeight() : targetTabH);
    const int startTabH = ui->tabWidgetDays
        ? qBound(320, currentTabH, 620)
        : targetTabH;
#endif

    goalScroll->setVisible(true);
    goalScroll->setFixedHeight(startScrollH);
    goalScroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    ui->frameGoal->setFixedHeight(startFrameH);
    ui->frameGoal->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
#ifdef Q_OS_ANDROID
    if (ui->tabWidgetDays && targetTabH > 0) {
        ui->tabWidgetDays->setMinimumHeight(startTabH);
        ui->tabWidgetDays->setMaximumHeight(startTabH);
        ui->tabWidgetDays->updateGeometry();
    }
#endif

    if (m_btnGoalToggle)
        m_btnGoalToggle->setEnabled(false);

#ifdef Q_OS_ANDROID
    auto keepBottomNavOnTop = [this]() {
        if (!ui || !ui->stackedWidget || !m_bottomNav)
            return;
        const int idx = ui->stackedWidget->currentIndex();
        const int homeIndex = ui->page_4 ? ui->stackedWidget->indexOf(ui->page_4) : 3;
        const int profileIndex = ui->page_5 ? ui->stackedWidget->indexOf(ui->page_5) : 4;
        const bool navPage = idx == homeIndex || idx == profileIndex || idx == m_analysisPageIndex || idx == m_reportPageIndex;
        if (!navPage)
            return;
        if (!m_bottomNav->isVisible())
            m_bottomNav->show();
        positionBottomNav();
        m_bottomNav->raise();
    };
    keepBottomNavOnTop();
#endif

    auto *anim = new QVariantAnimation(this);
    anim->setDuration(220);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    QPointer<QScrollArea> scrollPtr(goalScroll);
    QPointer<QFrame> framePtr(ui->frameGoal);
#ifdef Q_OS_ANDROID
    QPointer<QTabWidget> tabsPtr(ui->tabWidgetDays);
#endif

    connect(anim, &QVariantAnimation::valueChanged, this, [=](const QVariant &val) {
        const qreal t = val.toReal();
        if (scrollPtr) {
            const int h = qRound(startScrollH + (targetScrollH - startScrollH) * t);
            scrollPtr->setFixedHeight(h);
        }
        if (framePtr) {
            const int h = qRound(startFrameH + (targetFrameH - startFrameH) * t);
            framePtr->setFixedHeight(h);
            framePtr->updateGeometry();
        }
#ifdef Q_OS_ANDROID
        if (tabsPtr && targetTabH > 0) {
            const int h = qRound(startTabH + (targetTabH - startTabH) * t);
            tabsPtr->setMinimumHeight(h);
            tabsPtr->setMaximumHeight(h);
            tabsPtr->updateGeometry();
        }
#endif
#ifdef Q_OS_ANDROID
        keepBottomNavOnTop();
#endif
    });

    connect(anim, &QVariantAnimation::finished, this, [=]() {
        if (scrollPtr) {
            scrollPtr->setMinimumHeight(targetScrollH);
            scrollPtr->setMaximumHeight(targetScrollH);
            scrollPtr->setVisible(m_goalExpanded);
            scrollPtr->setSizePolicy(m_goalExpanded ? QSizePolicy::Expanding : QSizePolicy::Ignored,
                                     m_goalExpanded ? QSizePolicy::Preferred : QSizePolicy::Ignored);
        }
        if (framePtr) {
            framePtr->setMinimumHeight(targetFrameH);
            framePtr->setMaximumHeight(targetFrameH);
            framePtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
            framePtr->updateGeometry();
        }
#ifdef Q_OS_ANDROID
        if (tabsPtr && targetTabH > 0) {
            tabsPtr->setMinimumHeight(targetTabH);
            tabsPtr->setMaximumHeight(QWIDGETSIZE_MAX);
            tabsPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            tabsPtr->updateGeometry();
        }
#endif
        if (m_btnGoalToggle)
            m_btnGoalToggle->setEnabled(true);
#ifdef Q_OS_ANDROID
        updateBottomNavVisibility(ui->stackedWidget ? ui->stackedWidget->currentIndex() : -1);
        keepBottomNavOnTop();
#endif
    });

    anim->start(QAbstractAnimation::DeleteWhenStopped);
}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// setupPage4Widgets 锟?Add "鏌ョ湅鏄ㄦ棩瑙勫垝", all-done label, and



//                     12 "璋冩暣瑙勫垝" buttons programmatically



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::setupPage4Widgets()



{



    // Find the page_4 main layout (verticalLayout_page4)



    QWidget *page4 = ui->page_4;



    if (!page4) {



        qWarning() << "setupPage4Widgets: page_4 not found!";



        return;



    }







    QVBoxLayout *mainLayout = qobject_cast<QVBoxLayout *>(page4->layout());



    if (!mainLayout) {



        qWarning() << "setupPage4Widgets: page_4 layout not a QVBoxLayout";



        return;



    }







    // 鈹€鈹€ 1. 锟?lblStreakDays 锟?mainLayout 涓彇鍑猴紝鏁村悎杩涢《閮ㄦí锟?鈹€鈹€



    // 姝ゆ椂 mainLayout index 0 = lblStreakDays锛坧age_4 VBox 鐨勭涓€锟?item锟?

    QLayoutItem *streakLayoutItem = mainLayout->takeAt(0);



    QWidget *streakWidget = streakLayoutItem ? streakLayoutItem->widget() : nullptr;



    delete streakLayoutItem;   // 鍒犻櫎 item 鍖呰鍣紝涓嶅垹锟?widget 鏈韩







    auto *homeHeaderCard = new QFrame(page4);
    homeHeaderCard->setObjectName(QStringLiteral("homeHeaderCard"));
    homeHeaderCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    auto *homeHeaderLayout = new QVBoxLayout(homeHeaderCard);
#ifdef Q_OS_ANDROID
    homeHeaderLayout->setContentsMargins(12, 10, 12, 10);
    homeHeaderLayout->setSpacing(8);
#else
    homeHeaderLayout->setContentsMargins(16, 14, 16, 14);
    homeHeaderLayout->setSpacing(10);
#endif

    auto *topBar = new QHBoxLayout;



    topBar->setContentsMargins(0, 0, 0, 0);



#ifdef Q_OS_ANDROID
    topBar->setSpacing(6);
#else
    topBar->setSpacing(12);
#endif







    // 鍧氭寔澶╂暟鏍囩鏀惧乏锟?

    if (streakWidget) {



    // Hamburger button



    auto *btnHamburger = new QPushButton(QString::fromUtf8("\u2261"), page4);

    btnHamburger->setObjectName(QStringLiteral("btnHamburger"));

#ifdef Q_OS_ANDROID
    btnHamburger->setFixedSize(36, 36);
    QFont hamburgerFont = btnHamburger->font();
    hamburgerFont.setPointSize(20);
    hamburgerFont.setBold(false);
    btnHamburger->setFont(hamburgerFont);
#else
    btnHamburger->setFixedSize(48, 48);
#endif



    btnHamburger->setFlat(true);



    btnHamburger->setCursor(Qt::PointingHandCursor);



    btnHamburger->setStyleSheet(QString());



    connect(btnHamburger, &QPushButton::clicked, this, [this]() {



            setupSideDrawer();  // refresh user role before showing



            qDebug() << "Hamburger clicked, showing drawer";



        if (m_sideDrawer) m_sideDrawer->showAnimated();



    });



    topBar->addWidget(btnHamburger);

        auto *homeHeaderText = new QWidget(homeHeaderCard);
        homeHeaderText->setObjectName(QStringLiteral("homeHeaderText"));
        auto *homeHeaderTextLayout = new QVBoxLayout(homeHeaderText);
        homeHeaderTextLayout->setContentsMargins(0, 0, 0, 0);
        homeHeaderTextLayout->setSpacing(1);

        auto *homeHeaderTitle = new QLabel(QString::fromUtf8("\u4eca\u65e5\u8ba1\u5212"), homeHeaderText);
        homeHeaderTitle->setObjectName(QStringLiteral("homeHeaderTitle"));
        homeHeaderTitle->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        homeHeaderTextLayout->addWidget(homeHeaderTitle);

        auto *homeHeaderSubtitle = new QLabel(QString::fromUtf8("\u7528\u8f7b\u91cf\u8bb0\u5f55\uff0c\u966a\u4f60\u628a\u5065\u5eb7\u4e60\u60ef\u7a33\u7a33\u505a\u4e0b\u53bb"), homeHeaderText);
        homeHeaderSubtitle->setObjectName(QStringLiteral("homeHeaderSubtitle"));
        homeHeaderSubtitle->setWordWrap(false);
        homeHeaderSubtitle->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Fixed);
        homeHeaderTextLayout->addWidget(homeHeaderSubtitle);

        topBar->addWidget(homeHeaderText, 1);



    }







    topBar->addSpacing(0);







    // 鍏ㄩ儴鎵撳崱寰界珷灞呬腑鍋忓彸



    m_lblAllDone = new QLabel(tr("今日全部打卡完成"), page4);



    m_lblAllDone->setObjectName(QStringLiteral("lblAllDone"));



    m_lblAllDone->hide();



    // Keep this state available for logic, but do not place it in the top bar.
    // On narrow mobile screens the extra pill pushes the whole header sideways.







    // 鏄ㄦ棩鍥為【鎸夐挳鏀炬渶鍙充晶



    m_btnViewYesterday = new QPushButton(tr("\U0001F4C5 昨日回顾"), page4);



    m_btnViewYesterday->setObjectName(QStringLiteral("btnViewYesterday"));
    m_btnViewYesterday->setText(tr("昨日回顾"));



#ifdef Q_OS_ANDROID
    m_btnViewYesterday->setMinimumSize(82, 32);
    m_btnViewYesterday->setMaximumSize(92, 34);
#else
    m_btnViewYesterday->setMinimumSize(132, 48);
    m_btnViewYesterday->setMaximumSize(148, 52);
#endif
    m_btnViewYesterday->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);



    topBar->addWidget(m_btnViewYesterday);







    // 鎻掑叆锟?mainLayout 鏈€椤堕儴锛堢幇锟?index 0 锟?frameGoal锛屽洜锟?lblStreakDays 宸插彇鍑猴級



    homeHeaderLayout->addLayout(topBar);
    if (streakWidget) {
        streakWidget->setMinimumWidth(0);
#ifdef Q_OS_ANDROID
        streakWidget->setMinimumHeight(34);
        streakWidget->setMaximumHeight(36);
#else
        streakWidget->setMinimumHeight(40);
#endif
        streakWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        homeHeaderLayout->addWidget(streakWidget);
    }
    mainLayout->insertWidget(0, homeHeaderCard);







    connect(m_btnViewYesterday, &QPushButton::clicked,



            this, &MainWindow::onViewYesterdayClicked);







    // 鈹€鈹€ 2. 12 "璋冩暣瑙勫垝" buttons 锟?one per QGroupBox 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    // We map each group box 锟?an existing feedback button + text edit







    // Helper lambda: given a group box, its existing feedback button, and



    // a slot key/title, add an "璋冩暣" button inside the group box's layout



    auto addAdjustButton = [&](QGroupBox *groupBox,



                                QPushButton *&outAdjustBtn,



                                QTextEdit *textEdit,



                                const QString &slotKey,



                                const QString &slotTitle)



    {



        if (!groupBox) return;







        QHBoxLayout *hLayout = qobject_cast<QHBoxLayout *>(groupBox->layout());



        if (!hLayout) return;







        outAdjustBtn = new QPushButton(tr("调整"), groupBox);



        outAdjustBtn->setObjectName(QStringLiteral("btnAdjust"));



        outAdjustBtn->setMinimumWidth(112);
        outAdjustBtn->setMaximumWidth(128);



        outAdjustBtn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);



        hLayout->addWidget(outAdjustBtn);







        m_allAdjustButtons.append(outAdjustBtn);







        connect(outAdjustBtn, &QPushButton::clicked, this, [this, textEdit, slotKey, slotTitle]() {



            onAdjustPlanClicked(textEdit, slotKey, slotTitle);



        });



    };







    // Today



    addAdjustButton(ui->groupTodayMorning,   m_btnAdjustTodayMorning,   ui->txtTodayMorning,   QStringLiteral("breakfast"), tr("今天早餐"));



    addAdjustButton(ui->groupTodayLunch,     m_btnAdjustTodayLunch,     ui->txtTodayLunch,     QStringLiteral("lunch"),     tr("今天午餐"));



    addAdjustButton(ui->groupTodayDinner,    m_btnAdjustTodayDinner,    ui->txtTodayDinner,    QStringLiteral("dinner"),    tr("今天晚餐"));



    addAdjustButton(ui->groupTodayExercise,  m_btnAdjustTodayExercise,  ui->txtTodayExercise,  QStringLiteral("sports"),    tr("今天运动"));



    // Tomorrow



    addAdjustButton(ui->groupTomorrowMorning,   m_btnAdjustTomorrowMorning,   ui->txtTomorrowMorning,   QStringLiteral("breakfast"), tr("明天早餐"));



    addAdjustButton(ui->groupTomorrowLunch,     m_btnAdjustTomorrowLunch,     ui->txtTomorrowLunch,     QStringLiteral("lunch"),     tr("明天午餐"));



    addAdjustButton(ui->groupTomorrowDinner,    m_btnAdjustTomorrowDinner,    ui->txtTomorrowDinner,    QStringLiteral("dinner"),    tr("明天晚餐"));



    addAdjustButton(ui->groupTomorrowExercise,  m_btnAdjustTomorrowExercise,  ui->txtTomorrowExercise,  QStringLiteral("sports"),    tr("明天运动"));



    // DayAfter



    addAdjustButton(ui->groupDayAfterMorning,   m_btnAdjustDayAfterMorning,   ui->txtDayAfterMorning,   QStringLiteral("breakfast"), tr("后天早餐"));



    addAdjustButton(ui->groupDayAfterLunch,     m_btnAdjustDayAfterLunch,     ui->txtDayAfterLunch,     QStringLiteral("lunch"),     tr("后天午餐"));



    addAdjustButton(ui->groupDayAfterDinner,    m_btnAdjustDayAfterDinner,    ui->txtDayAfterDinner,    QStringLiteral("dinner"),    tr("后天晚餐"));



    addAdjustButton(ui->groupDayAfterExercise,  m_btnAdjustDayAfterExercise,  ui->txtDayAfterExercise,  QStringLiteral("sports"),    tr("后天运动"));







    qDebug() << "setupPage4Widgets: added" << m_allAdjustButtons.size() << "adjust buttons.";







    // -- navigation buttons below tab widget --



    {



        int tabIdx = mainLayout->indexOf(ui->tabWidgetDays);



        if (tabIdx >= 0) {



            // 深度分析 (above 个人中心)



            QPushButton *btnAnalysis = new QPushButton(QString::fromUtf8("\U0001F52C 深度分析"), page4);



            btnAnalysis->setMinimumHeight(38);



            btnAnalysis->setCursor(Qt::PointingHandCursor);



            connect(btnAnalysis, &QPushButton::clicked, this, &MainWindow::onDeepAnalysisTriggered);



            mainLayout->insertWidget(tabIdx + 1, btnAnalysis);







            // 个人中心 (below 深度分析)



            QPushButton *btnProfile = new QPushButton(QString::fromUtf8("\U0001F464 个人中心"), page4);



            btnProfile->setMinimumHeight(38);



            btnProfile->setCursor(Qt::PointingHandCursor);



            connect(btnProfile, &QPushButton::clicked, this, [this]() {



                navigateTo(AppRoute::Profile, true);



            });



            mainLayout->insertWidget(tabIdx + 2, btnProfile);



            btnAnalysis->hide();  // replaced by bottom nav



            btnProfile->hide();   // replaced by bottom nav



        }



    }



}







// ------------------------------------------------------------------------



// Side Drawer setup



// ------------------------------------------------------------------------



void MainWindow::setupSideDrawer()



{



    if (!m_sideDrawer) return;



    if (m_sideDrawer->findChild<QWidget *>(QStringLiteral("drawerPanel"))) {



        QWidget *panel = m_sideDrawer->findChild<QWidget *>(QStringLiteral("drawerPanel"));



        if (panel && panel->layout()) {



            QLayout *lay = panel->layout();



            QLayoutItem *item;



            while ((item = lay->takeAt(0)) != nullptr) {



                if (item->widget()) item->widget()->deleteLater();



                delete item;



            }



        }



    }







    LifeBalanceAI::Models::UserInfo info = DatabaseManager::instance().getUserInfo(m_currentUserId);



    LifeBalanceAI::Models::ProfileData profile;



    if (m_currentUserId > 0)



        profile = DatabaseManager::instance().getProfile(m_currentUserId);







    QString roleDisplay = QString::fromUtf8("\u63a2\u7d22\u8005");



    if (info.role == QStringLiteral("Ascendant"))



        roleDisplay = QString::fromUtf8("\u5f8b\u884c\u8005");



    else if (info.role == QStringLiteral("admin"))



        roleDisplay = QString::fromUtf8("\u7ba1\u7406\u5458");







    QString roleBadge = roleDisplay;
    if (info.role == QStringLiteral("Ascendant"))
        roleBadge += QString::fromUtf8(" \u00b7 \u5df2\u5347\u7ea7");
    else if (info.role == QStringLiteral("Explorer"))
        roleBadge += QString::fromUtf8(" \u00b7 \u53ef\u5347\u7ea7");
    else if (info.role == QStringLiteral("admin"))
        roleBadge += QString::fromUtf8(" \u00b7 \u7ba1\u7406\u6a21\u5f0f");

    m_sideDrawer->addHeader(profile.nickname, roleBadge);







    m_sideDrawer->addItem(



        QString::fromUtf8("\u270f"),



        QString::fromUtf8("\u4fee\u6539\u753b\u50cf"),



        [this]() {



            m_profileEditFromPage5 = true;



            LifeBalanceAI::Models::ProfileData existingProfile = DatabaseManager::instance().getProfile(m_currentUserId);
            populateProfileEditFields(existingProfile);



            navigateTo(AppRoute::ProfileSetup, false);



            if (QPushButton *btn = ui->page_3->findChild<QPushButton *>(QStringLiteral("btnBackPage3")))



                btn->setVisible(true);



        });







    if (info.role == QStringLiteral("Ascendant")) {



        m_sideDrawer->addItem(



            QString::fromUtf8("\u2726"),



            QString::fromUtf8("\u8eab\u4efd\u72b6\u6001 \u00b7 \u5f8b\u884c\u8005"),



            nullptr);



    } else if (info.role == QStringLiteral("admin")) {



        m_sideDrawer->addItem(



            QString::fromUtf8("\u2726"),



            QString::fromUtf8("\u8eab\u4efd\u72b6\u6001 \u00b7 \u7ba1\u7406\u5458"),



            nullptr);



    } else {



        m_sideDrawer->addItem(



            QString::fromUtf8("\u2b50"),



            QString::fromUtf8("\u5347\u7ea7\u4e3a\u5f8b\u884c\u8005"),



            [this]() {



                m_prePaymentPageIndex = ui->stackedWidget->currentIndex();



                navigateTo(AppRoute::Payment, false);



            });



    }











    m_sideDrawer->addSeparator();

    // Theme toggle - show opposite mode of current theme
    {
        const bool isDark = ThemeManager::instance().isDarkMode();
        const QString themeLabel = isDark ? QString::fromUtf8("\u2600 \u6d45\u8272\u6a21\u5f0f")
                                          : QString::fromUtf8("\U0001F319 \u6df1\u8272\u6a21\u5f0f");
        m_sideDrawer->addItem(
            QString::fromUtf8("\U0001F3A8"),
            themeLabel,
            [this]() {
                ThemeManager &tm = ThemeManager::instance();
                tm.setTheme(tm.isDarkMode() ? ThemeManager::Light : ThemeManager::Dark);
                qApp->setStyleSheet(QString());
                QFile qssFile(QStringLiteral(":/resources/style.qss"));
                QString qssContent;
                if (qssFile.open(QFile::ReadOnly | QFile::Text)) {
                    QTextStream ts(&qssFile);
                    qssContent = ts.readAll();
                    qssFile.close();
                }
                if (tm.isDarkMode())
                    qssContent += QStringLiteral("\n\n/* Dark theme overlay */\n") + tm.getStylesheet();
                qApp->setStyleSheet(qssContent);
                m_sideDrawer->deleteLater();
                m_sideDrawer = new SideDrawer(this);
                m_sideDrawer->setObjectName(QStringLiteral("sideDrawer"));
                setupSideDrawer();
            });
    }

    m_sideDrawer->addSeparator();

    m_sideDrawer->addItem(

        QString::fromUtf8("\u2139"),

        QString::fromUtf8("\u5173\u4e8e"),



        [this]() {



            AnimatedDialog::info(this,



                QString::fromUtf8("\u5173\u4e8e"),



                QString::fromUtf8("LifeBalance AI v3.0\n\n\u4e00\u6b3e\u667a\u80fd\u5065\u5eb7\u7ba1\u7406\u52a9\u624b\uff0c\n\u57fa\u4e8e AI \u4e3a\u4f60\u5b9a\u5236\u996e\u98df\u4e0e\u8fd0\u52a8\u8ba1\u5212\u3002"));



        });







    m_sideDrawer->addSeparator();







    m_sideDrawer->addItem(



        QString::fromUtf8("\U0001F6AA"),



        QString::fromUtf8("\u9000\u51fa\u767b\u5f55"),



        [this]() {
            confirmLogoutToLogin();

        });



}















// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Page switching



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::on_btnGoRegister_clicked()



{



    navigateTo(AppRoute::Register, false);



}







void MainWindow::on_btnGoLogin_clicked()



{



    navigateTo(AppRoute::Login, false);



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Simulate verification code (mock SMS)



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::on_btnSendVerifyCode_clicked()
{
    AnimatedDialog::info(this,
                         QString::fromUtf8("\u6a21\u62df\u9a8c\u8bc1"),
                         LifeBalanceAI::Services::AccountFlowService::mockVerificationMessage());
}


// Registration



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::on_btnRegister_clicked()
{
    LifeBalanceAI::Models::RegisterInput input;
    input.phone = ui->editPhoneReg->text().trimmed();
    input.password = ui->editPwdReg->text();
    input.verifyCode = ui->editVerifyCode->text().trimmed();
    input.role = ui->comboRole->currentIndex() == 1
        ? QStringLiteral("Ascendant")
        : QStringLiteral("Explorer");

    const auto result = LifeBalanceAI::Services::AccountFlowService::registerWithPassword(input);
    if (!result.ok) {
        AnimatedDialog::warn(this,
                             QString::fromUtf8("\u6ce8\u518c\u5931\u8d25"),
                             result.message);
        return;
    }

    AnimatedDialog::info(this,
                         QString::fromUtf8("\u6ce8\u518c\u6210\u529f"),
                         result.message);

    ui->editPhoneReg->clear();
    ui->editPwdReg->clear();
    ui->editVerifyCode->clear();
    ui->comboRole->setCurrentIndex(0);

    navigateTo(result.nextRoute, false);
}


// Login



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::onForgotPassword()
{
    bool ok = false;
    QString phone = AnimatedInputDialog::getText(
        this,
        QString::fromUtf8("\u5fd8\u8bb0\u5bc6\u7801"),
        QString::fromUtf8("\u8bf7\u8f93\u5165\u6ce8\u518c\u65f6\u4f7f\u7528\u7684\u624b\u673a\u53f7\uff1a"),
        QString(),
        &ok);
    if (!ok || phone.trimmed().isEmpty())
        return;
    phone = phone.trimmed();

    AnimatedDialog::info(this,
                         QString::fromUtf8("\u6a21\u62df\u9a8c\u8bc1"),
                         LifeBalanceAI::Services::AccountFlowService::mockVerificationMessage());

    const QString code = AnimatedInputDialog::getText(
        this,
        QString::fromUtf8("\u5fd8\u8bb0\u5bc6\u7801"),
        QString::fromUtf8("\u8bf7\u8f93\u5165\u9a8c\u8bc1\u7801\uff1a"),
        QString(),
        &ok);
    if (!ok)
        return;

    const QString newPassword = AnimatedInputDialog::getText(
        this,
        QString::fromUtf8("\u5fd8\u8bb0\u5bc6\u7801"),
        QString::fromUtf8("\u8bf7\u8f93\u5165\u65b0\u5bc6\u7801\uff08\u81f3\u5c11 6 \u4f4d\uff09\uff1a"),
        QString(),
        &ok);
    if (!ok)
        return;

    const auto result = LifeBalanceAI::Services::AccountFlowService::resetPasswordWithCode(
        phone, code, newPassword);
    if (result.ok) {
        AnimatedDialog::info(this,
                             QString::fromUtf8("\u6210\u529f"),
                             result.message);
    } else {
        AnimatedDialog::warn(this,
                             QString::fromUtf8("\u91cd\u7f6e\u5931\u8d25"),
                             result.message);
    }
}
void MainWindow::on_btnLogin_clicked()
{
    const QString phone = ui->editPhoneLogin->text().trimmed();
    const QString password = ui->editPwdLogin->text();

    const auto result = LifeBalanceAI::Services::AuthFlowService::loginWithPassword(phone, password);
    if (!result.ok) {
        const QString title = result.message.startsWith(QStringLiteral("查询用户时出错"))
            ? tr("数据库错误")
            : tr("登录失败");
        AnimatedDialog::warn(this, title, result.message);
        return;
    }

    saveAutoLoginSessionIfNeeded(result.userId, result.role);
    handleAuthenticatedUser(result);
}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Profile helpers (private)



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







QString MainWindow::collectCheckedTexts(QObject *groupBox, const QString &editName) const



{



    QStringList parts;



    for (QCheckBox *cb : groupBox->findChildren<QCheckBox *>()) {



        if (cb->isChecked())



            parts << cb->text();



    }



    QLineEdit *edit = groupBox->findChild<QLineEdit *>(editName);



    if (edit) {



        QString custom = edit->text().trimmed();



        if (!custom.isEmpty())



            parts << custom;



    }



    return parts.join(QStringLiteral(", "));



}







QString MainWindow::getDietString() const



{



    return collectCheckedTexts(ui->groupBoxFood, QStringLiteral("editFood"));



}







QString MainWindow::getSportString() const



{



    return collectCheckedTexts(ui->groupBoxSports, QStringLiteral("editSports"));



}







QString MainWindow::getTargetString() const



{



    return collectCheckedTexts(ui->groupBoxTarget, QStringLiteral("editTarget"));



}

LifeBalanceAI::Models::ProfileInput MainWindow::profileInputFromSetupPage() const
{
    LifeBalanceAI::Models::ProfileInput input;
    input.nickname = ui->editNickname ? ui->editNickname->text().trimmed() : QString();
    input.ageText = ui->spinBoxAge ? QString::number(ui->spinBoxAge->value()) : QString();
    input.heightText = ui->editHeight ? ui->editHeight->text().trimmed() : QString();
    input.weightText = ui->editWeight ? ui->editWeight->text().trimmed() : QString();
    input.gender = ui->comboGender ? ui->comboGender->currentText() : QString();
    input.goal = getTargetString();
    if (QLineEdit *editAllergy = ui->page_3->findChild<QLineEdit *>(QStringLiteral("editAllergy")))
        input.allergy = editAllergy->text().trimmed();
    input.dietPref = getDietString();
    input.sportPref = getSportString();
    return input;
}

LifeBalanceAI::Models::ProfileInput MainWindow::profileInputFromProfilePage() const
{
    LifeBalanceAI::Models::ProfileInput input;
    const LifeBalanceAI::Models::ProfileData existing =
        DatabaseManager::instance().getProfile(m_currentUserId);

    QLineEdit *editNickname5 = ui->page_5->findChild<QLineEdit *>(QStringLiteral("editNickname5"));
    input.nickname = editNickname5 ? editNickname5->text().trimmed() : existing.nickname;
    input.ageText = ui->spinBoxAge5 ? QString::number(ui->spinBoxAge5->value()) : QString();
    input.heightText = ui->editHeight5 ? numericPrefix(ui->editHeight5->text()) : QString();
    input.weightText = ui->editWeight5 ? numericPrefix(ui->editWeight5->text()) : QString();
    input.gender = ui->comboGender5 ? ui->comboGender5->currentText() : QString();
    input.goal = ui->editGoal5 ? ui->editGoal5->text().trimmed() : existing.goal;
    input.allergy = ui->editAllergy5 ? ui->editAllergy5->text().trimmed() : existing.allergy;
    input.dietPref = existing.dietPref;
    input.sportPref = existing.sportPref;
    return input;
}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Save profile



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







// Populate page_3 form fields with existing profile data for editing
void MainWindow::populateProfileEditFields(const LifeBalanceAI::Models::ProfileData &profile)
{
    ui->editNickname->setText(profile.nickname);
    ui->spinBoxAge->setValue(profile.age);
    ui->editHeight->setText(profile.height > 0 ? QString::number(profile.height, 'f', 1) : QString());
    ui->editWeight->setText(profile.weight > 0 ? QString::number(profile.weight, 'f', 1) : QString());
    if (!profile.gender.isEmpty()) {
        int idx = ui->comboGender->findText(profile.gender);
        if (idx >= 0) ui->comboGender->setCurrentIndex(idx);
    } else {
        ui->comboGender->setCurrentIndex(0);
    }
    QLineEdit *editAllergy = ui->page_3->findChild<QLineEdit *>(QStringLiteral("editAllergy"));
    if (editAllergy) editAllergy->setText(profile.allergy);

    // Parse and set diet preferences
    const QStringList diets = profile.dietPref.split(QStringLiteral(", "), Qt::SkipEmptyParts);
    for (QCheckBox *cb : ui->groupBoxFood->findChildren<QCheckBox *>()) {
        cb->setChecked(diets.contains(cb->text(), Qt::CaseInsensitive));
    }

    // Parse and set sports preferences
    const QStringList sports = profile.sportPref.split(QStringLiteral(", "), Qt::SkipEmptyParts);
    for (QCheckBox *cb : ui->groupBoxSports->findChildren<QCheckBox *>()) {
        cb->setChecked(sports.contains(cb->text(), Qt::CaseInsensitive));
    }

    // Set goal-related checkboxes (from goal field)
    const QStringList goals = profile.goal.split(QStringLiteral(", "), Qt::SkipEmptyParts);
    for (QCheckBox *cb : ui->groupBoxTarget->findChildren<QCheckBox *>()) {
        cb->setChecked(goals.contains(cb->text(), Qt::CaseInsensitive));
    }
}

void MainWindow::on_btnSaveProfile_clicked()
{
    if (m_currentUserId < 0) {
        AnimatedDialog::warn(this,
                             QStringLiteral("\u9519\u8bef"),
                             QStringLiteral("\u672a\u68c0\u6d4b\u5230\u767b\u5f55\u7528\u6237\uff0c\u8bf7\u91cd\u65b0\u767b\u5f55\uff01"));
        return;
    }

    const bool editingFromProfileSetup = m_profileEditFromPage5;
    const bool adminUser =
        DatabaseManager::instance().getUserRole(m_currentUserId) == QStringLiteral("admin");
    const auto mode = adminUser ? LifeBalanceAI::Models::ProfileSaveMode::AdminSetup
        : editingFromProfileSetup ? LifeBalanceAI::Models::ProfileSaveMode::EditFromProfile
                                  : LifeBalanceAI::Models::ProfileSaveMode::InitialSetup;
    const auto saveResult = LifeBalanceAI::Services::ProfileFlowService::saveProfile(
        m_currentUserId, profileInputFromSetupPage(), mode);

    if (!saveResult.ok) {
        AnimatedDialog::warn(this, QStringLiteral("\u4fdd\u5b58\u5931\u8d25"), saveResult.message);
        return;
    }

    AnimatedDialog::info(this, QStringLiteral("\u4fdd\u5b58\u6210\u529f"), saveResult.message);
    if (saveResult.requestNickname) {
        const QString profileSummary = buildUserProfileString();
        if (!profileSummary.isEmpty())
            AIManager::instance().generateNickname(profileSummary);
    }

    if (editingFromProfileSetup) {
        m_profileEditFromPage5 = false;
        if (QPushButton *btn = ui->page_3->findChild<QPushButton *>(QStringLiteral("btnBackPage3")))
            btn->setVisible(false);
        loadMainPage();
        updatePlanUI();
        navigateTo(saveResult.nextRoute, false);
        return;
    }

    if (saveResult.nextRoute == AppRoute::Admin) {
        loadAdminPage();
        navigateTo(AppRoute::Admin, false);
        return;
    }

    loadMainPage();
    updatePlanUI();
    navigateTo(AppRoute::Home, false);
    if (saveResult.needsInitialPlan)
        requestAIPlanWhenHomeReady();
}
// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Load main page (page_4) with data



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::loadMainPage()



{



    // Daily cleanup: remove data older than 90 days (once per session)



    static bool s_cleaned = false;



    if (!s_cleaned) {



        DatabaseManager::instance().cleanOldData(90);



        s_cleaned = true;



    }







    // Process daily checkin and get real streak info



    if (m_currentUserId > 0 && m_checkinService) {



        // Monthly freeze card refill (only for Ascendant, 1st day of month)



        if (m_freezeCardService)



            m_freezeCardService->processMonthlyRefill(m_currentUserId);







        auto result = m_checkinService->processDailyCheckin(m_currentUserId);



        auto info   = m_checkinService->getStreakInfo(m_currentUserId);







#ifdef Q_OS_ANDROID
        ui->lblStreakDays->setText(tr("连续%1天 | 累计%2天 | 冻结%3天")
                                       .arg(info.streakDays)
                                       .arg(info.totalCheckins)
                                       .arg(info.freezeCards));
#else
        ui->lblStreakDays->setText(tr("连续 %1 天 | 累计 %2 天 | 冻结 %3 天")
                                       .arg(info.streakDays)
                                       .arg(info.totalCheckins)
                                       .arg(info.freezeCards));
#endif







        if (result.reprofileRequired && !m_reprofilePromptPending) {
            m_reprofilePromptPending = true;
            QTimer::singleShot(900, this, [this]() {
                if (!ui || !ui->stackedWidget || m_currentUserId <= 0) {
                    m_reprofilePromptPending = false;
                    return;
                }
                const int homeIndex = ui->page_4 ? ui->stackedWidget->indexOf(ui->page_4) : 3;
                if (ui->stackedWidget->currentIndex() != homeIndex) {
                    m_reprofilePromptPending = false;
                    return;
                }

                const int choice = AnimatedDialog::choose(
                    this,
                    tr("温馨提示"),
                    tr("您近期有较多时段未完成，建议重新填写画像以获得更精准的规划！"),
                    tr("修改画像"),
                    tr("暂时跳过"));

                m_reprofilePromptPending = false;
                if (choice != 0)
                    return;

                m_profileEditFromPage5 = true;
                LifeBalanceAI::Models::ProfileData existingProfile = DatabaseManager::instance().getProfile(m_currentUserId);
                populateProfileEditFields(existingProfile);
                navigateTo(AppRoute::ProfileSetup, false);
                if (QPushButton *btn = ui->page_3->findChild<QPushButton *>(QStringLiteral("btnBackPage3")))
                    btn->setVisible(true);
            });
        }



    } else {



#ifdef Q_OS_ANDROID
        ui->lblStreakDays->setText(tr("连续0天 | 累计0天 | 冻结0天"));
#else
        ui->lblStreakDays->setText(tr("连续 0 天 | 累计 0 天 | 冻结 0 天"));
#endif



    }







    // Load long-term goal from Plans table (latest one)
    // Do NOT set text on lblGoalContent here — it may have width 0 (page hidden),
    // which would cause incorrect word-wrap. Just cache it; the expand click handler
    // sets text at the correct width.
    QString goal = DatabaseManager::instance().getLongTermGoal(m_currentUserId);
    m_goalText = goal.isEmpty() ? tr("（暂未设置）") : goal;








    updateGoalCollapseState();

    // Detect day change and auto-trigger AI plan gaps



    if (m_currentUserId > 0 && !m_isRequestPending) {



        QDate today = QDate::currentDate();



        bool dayChanged = (today != m_lastRefreshDate);



        if (dayChanged) {



            m_lastRefreshDate = today;



        }



        int planId = DatabaseManager::instance().getLatestPlanId(m_currentUserId);



        if (planId > 0) {



            auto todayItems = DatabaseManager::instance().getItemsForDate(planId, today);



            if (todayItems.isEmpty()) {



                qDebug() << "loadMainPage: No items for today, triggering AI plan...";



                requestAIPlanWhenHomeReady();



            } else {



                // Today has items, but check if future days (tomorrow, day-after) are missing



                QDate tomorrow = today.addDays(1);



                QDate dayAfter = today.addDays(2);



                auto tomorrowItems = DatabaseManager::instance().getItemsForDate(planId, tomorrow);



                auto dayAfterItems = DatabaseManager::instance().getItemsForDate(planId, dayAfter);



                if (tomorrowItems.isEmpty() || dayAfterItems.isEmpty()) {



                    qDebug() << "loadMainPage: Future days missing items, triggering AI plan...";



                    requestAIPlanWhenHomeReady();



                }



            }



        }



    }















    // Admin simulate-login: add return button



    {



        QPushButton *btnReturn = ui->page_4->findChild<QPushButton *>(QStringLiteral("btnAdminReturn"));



        if (m_adminUserId >= 0 && !btnReturn) {



            QVBoxLayout *p4Layout = qobject_cast<QVBoxLayout *>(ui->page_4->layout());



            if (p4Layout) {



                btnReturn = new QPushButton(tr("返回管理员"), ui->page_4);



                btnReturn->setObjectName(QStringLiteral("btnAdminReturn"));



                btnReturn->setMinimumHeight(34);



                UiFactory::setClass(btnReturn, QStringLiteral("secondary"));



                connect(btnReturn, &QPushButton::clicked, this, [this]() {



                    m_currentUserId = m_adminUserId;



                    m_adminUserId = -1;



                    QPushButton *br = ui->page_4->findChild<QPushButton *>(QStringLiteral("btnAdminReturn"));



                    if (br) br->deleteLater();



                    loadAdminPage();



                    navigateTo(AppRoute::Admin, false);



                });



                p4Layout->addWidget(btnReturn);



            }



        } else if (m_adminUserId < 0 && btnReturn) {



            btnReturn->deleteLater();



        }



    }



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Build user profile string for AI consumption



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







QString MainWindow::buildUserProfileString() const



{



    QSqlDatabase &db = DatabaseManager::instance().database();



    QSqlQuery query(db);







    query.prepare(QStringLiteral(



        "SELECT age, height, weight, gender, goal, allergy, preferences "



        "FROM Profiles WHERE user_id = :uid"



    ));



    query.bindValue(QStringLiteral(":uid"), m_currentUserId);







    if (!query.exec() || !query.next()) {



        qWarning() << "buildUserProfileString: No profile found for user" << m_currentUserId;



        return QString();



    }







    QStringList parts;



    parts << QStringLiteral("年龄: %1 岁").arg(query.value(0).toInt());



    parts << QStringLiteral("身高: %1 cm").arg(query.value(1).toDouble());



    parts << QStringLiteral("体重: %1 kg").arg(query.value(2).toDouble());



    parts << QStringLiteral("性别: %1").arg(query.value(3).toString());







    QString goal = query.value(4).toString().trimmed();



    if (!goal.isEmpty())



        parts << QStringLiteral("目标: %1").arg(goal);







    QString allergy = query.value(5).toString().trimmed();



    if (!allergy.isEmpty())



        parts << QStringLiteral("过敏源: %1").arg(allergy);







    QString preferences = query.value(6).toString().trimmed();



    if (!preferences.isEmpty())



        parts << QStringLiteral("偏好: %1").arg(preferences);







    return parts.join(QStringLiteral("; "));



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Resolve slot info from a QTextEdit pointer



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::resolveSlotInfo(QTextEdit *textEdit,



                                  QString &outSlotKey,



                                  QString &outSlotTitle) const



{
    const auto slotRef = LifeBalanceAI::Services::HomePlanService::resolveSlotRef(
        textEdit ? textEdit->objectName() : QString());
    outSlotKey = slotRef.valid ? slotRef.slotKey : QStringLiteral("unknown");
    outSlotTitle = slotRef.valid ? slotRef.slotTitle : outSlotKey;



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Feedback button handler 锟?enhanced for all 7 points



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::onFeedbackButtonClicked(QTextEdit *textEdit, const QString &slotTitle)



{



    if (m_currentUserId < 0) {



        AnimatedDialog::warn(this, tr("错误"), tr("未检测到登录用户！"));



        return;



    }







    // 鈹€鈹€ 0. Resolve slot key, date, and find itemId 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    QString slotKey;



    QString resolvedTitle;



    resolveSlotInfo(textEdit, slotKey, resolvedTitle);
    const auto slotRef = LifeBalanceAI::Services::HomePlanService::resolveSlotRef(
        textEdit ? textEdit->objectName() : QString());
    if (!slotRef.valid) {
        showHomePlanStatus(tr("未识别的计划时段，请刷新后重试。"));
        return;
    }







    int planId = DatabaseManager::instance().getLatestPlanId(m_currentUserId);



    if (planId < 0) {



        showHomePlanStatus(tr("暂无当前计划，无法反馈。"));



        return;



    }







    QDate targetDate = slotRef.date;



    QString dateStr = targetDate.toString(QStringLiteral("yyyy-MM-dd"));







    int itemId = DatabaseManager::instance().getItemId(planId, dateStr, slotKey);



    if (itemId < 0) {



        showHomePlanStatus(tr("该时段暂无计划，无法反馈。"));



        return;



    }







    // 鈹€鈹€ 1. Check if this item is already done 锟?use "淇敼鍙嶉" mode 鈹€鈹€



    QString existingFeedback = DatabaseManager::instance().getFeedbackContent(itemId);



    bool isModifyMode = !existingFeedback.isEmpty();







    // 鈹€鈹€ 2. Open feedback dialog (pre-filled if in modify mode) 鈹€鈹€



    FeedbackDialog dlg(slotTitle, existingFeedback, this);



    if (dlg.exec() != QDialog::Accepted)



        return;







    QString feedbackContent = dlg.feedbackText();



    if (feedbackContent.isEmpty()) {



        AnimatedDialog::warn(this, tr("提示"), tr("反馈内容不能为空！"));



        return;



    }







    // 鈹€鈹€ 3. Save feedback to DB (idempotent, with optional image) 鈹€



    QString imgPath = dlg.imagePath();



    if (!DatabaseManager::instance().saveFeedback(m_currentUserId, itemId, feedbackContent, imgPath)) {



        AnimatedDialog::warn(this, tr("错误"), tr("反馈保存失败，请重试！"));



        return;



    }







    // 鈹€鈹€ 4. Mark item as done 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    DatabaseManager::instance().markItemDone(itemId);

    MotionHelper::animateCompletionFeedback(
        ui->stackedWidget,
        isModifyMode ? tr("反馈已更新") : tr("打卡完成"));







    // 鈹€鈹€ 5. Ascendant role 锟?partial update via AI 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    QString role = DatabaseManager::instance().getUserRole(m_currentUserId);







    if (role == QStringLiteral("Ascendant")) {



        // Get current plan JSON for partial update context



        QJsonObject planJson = DatabaseManager::instance().getPlanAsJson(planId);



        QString existingPlanStr = QString::fromUtf8(



            QJsonDocument(planJson).toJson(QJsonDocument::Compact));







        QString userProfile = buildUserProfileString();



        if (userProfile.isEmpty()) {



            AnimatedDialog::warn(this, tr("生成失败"), tr("无法读取用户画像！"));



        } else {



            showLoadingBar(tr("正在根据反馈调整计划"));



            m_isRequestPending = true;







            // Combine the feedback content with any historical context



            QString requestText;



            if (isModifyMode) {



                requestText = QString(



                    "之前已有反馈：%1。本次修改反馈：%2\n"



                    "请根据本次反馈调整后续规划。"



                ).arg(existingFeedback, feedbackContent);



            } else {



                requestText = QString(



                    "用户反馈：%1。请根据此反馈调整后续规划。"



                ).arg(feedbackContent);



            }







            AIManager::instance().partialUpdatePlan(



                userProfile, slotKey, requestText, existingPlanStr, 0);



        }



    }







    // 鈹€鈹€ 6. Refresh UI 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    updatePlanUI();



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Adjust plan button handler



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::onAdjustPlanClicked(QTextEdit *textEdit,



                                      const QString &slotKey,



                                      const QString &slotTitle)



{



    if (m_currentUserId < 0) {



        AnimatedDialog::warn(this, tr("错误"), tr("未检测到登录用户！"));



        return;



    }







    int planId = DatabaseManager::instance().getLatestPlanId(m_currentUserId);



    if (planId < 0) {



        showHomePlanStatus(tr("暂无当前计划，无法调整。"));



        return;



    }







    const auto slotRef = LifeBalanceAI::Services::HomePlanService::resolveSlotRef(
        textEdit ? textEdit->objectName() : QString());
    if (!slotRef.valid || slotRef.slotKey != slotKey) {
        showHomePlanStatus(tr("未识别的计划时段，请刷新后重试。"));
        return;
    }

    const int dayOffset = slotRef.dayOffset;
    QDate targetDate = slotRef.date;
    int itemId = DatabaseManager::instance().getItemId(
        planId, targetDate.toString(QStringLiteral("yyyy-MM-dd")), slotKey);
    if (itemId < 0) {
        showHomePlanStatus(tr("该时段暂无计划，无法调整。"));
        return;
    }







    QString role = DatabaseManager::instance().getUserRole(m_currentUserId);



    bool isAscendant = (role == QStringLiteral("Ascendant"));







    // 鈹€鈹€ Get user input 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    bool ok = false;



    QString userInput = AnimatedInputDialog::getText(



        this,



        tr("调整规划 - %1").arg(slotTitle),



        tr("请输入您对「%1」的调整要求：\n（例如：我不想吃这个，换个简单的）").arg(slotTitle),



        QString(),



        &ok);







    if (!ok || userInput.trimmed().isEmpty())



        return;



    userInput = userInput.trimmed();







    // 鈹€鈹€ Tomorrow/Day-after for Explorer: defer 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    if (dayOffset > 0 && !isAscendant) {



        // Explorer + future: direct DB write (no AI available)



        if (itemId > 0) {



            DatabaseManager::instance().updateDailyItemContent(itemId, userInput);



            DatabaseManager::instance().markItemAdjusted(itemId);



        }



        QString dayLabel = (dayOffset == 1) ? tr("明天") : tr("后天");



        AnimatedDialog::info(this, tr("调整成功"), tr("调整成功，%1 将更新计划").arg(dayLabel));



        // Reload UI to show directly-updated content



        updatePlanUI();



        updateAllDoneLabel();



        return;



    }







    // 鈹€鈹€ Today (both roles) or Tomorrow/Day-after (Ascendant) 鈹€鈹€鈹€鈹€



    QJsonObject planJson = DatabaseManager::instance().getPlanAsJson(planId);



    QString existingPlanStr = QString::fromUtf8(



        QJsonDocument(planJson).toJson(QJsonDocument::Compact));







    QString userProfile = buildUserProfileString();



    if (userProfile.isEmpty()) {



        AnimatedDialog::warn(this, tr("生成失败"), tr("无法读取用户画像！"));



        return;



    }







    // Store pending slot info so onPartialUpdateGenerated can mark it adjusted after AI response



    m_pendingAdjustDate = targetDate.toString(QStringLiteral("yyyy-MM-dd"));



    m_pendingAdjustSlotKey = slotKey;







    showLoadingBar(tr("正在调整当前计划"));







    if (dayOffset > 0 && isAscendant) {



        // Ascendant adjusting future day: AI polishes and updates all slots



        QString dayLabel = (dayOffset == 1) ? QStringLiteral("明天") : QStringLiteral("后天");



        QString requestText = QString(



            "用户想将%1「%2」改为：%3"



            "请润色整理为完整饮食/运动规划，更新「%2」，并根据变化调整%1其他时段，保持 JSON 格式不变"



        ).arg(dayLabel, slotTitle, userInput);



        AIManager::instance().partialUpdatePlan(



            userProfile, slotKey, requestText, existingPlanStr, dayOffset);



    } else {



        // Today (both roles): AI polishes adjusted slot and updates other undone slots



        QString requestText = QString(



            "用户想将今天「%1」改为：%2"



            "请润色整理为完整饮食/运动规划，更新「%1」，并根据变化调整今天其他未完成时段，保持 JSON 格式不变"



        ).arg(slotTitle, userInput);



        AIManager::instance().partialUpdatePlan(



            userProfile, slotKey, requestText, existingPlanStr, dayOffset);



    }



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Partial update result handler



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::onPartialUpdateGenerated(const QString &jsonResult)



{



    m_isRequestPending = false;



    hideLoadingBar();

    auto clearPendingAdjust = [this]() {
        m_pendingAdjustSlotKey.clear();
        m_pendingAdjustDate.clear();
    };

    const auto parseResult = LifeBalanceAI::Services::HomePlanService::parsePartialUpdate(
        jsonResult,
        QDate::currentDate(),
        m_pendingAdjustDate);
    if (!parseResult.ok) {
        qWarning() << "onPartialUpdateGenerated: parse failed:" << parseResult.errorMessage;
        showHomePlanStatus(tr("计划调整失败，可稍后重试。"));
        clearPendingAdjust();
        return;
    }

    int planId = DatabaseManager::instance().getLatestPlanId(m_currentUserId);
    if (planId < 0) {
        showHomePlanStatus(tr("未找到当前计划，请稍后重试。"));
        clearPendingAdjust();
        return;
    }

    int updatedCount = 0;
    for (const auto &item : parseResult.items) {
        const int itemId = DatabaseManager::instance().getItemId(
            planId,
            item.date.toString(QStringLiteral("yyyy-MM-dd")),
            item.timeSlot);
        if (itemId < 0) {
            qWarning() << "onPartialUpdateGenerated: item not found for"
                       << item.date << item.timeSlot;
            continue;
        }

        if (DatabaseManager::instance().updateDailyItemContent(itemId, item.content))
            ++updatedCount;
    }

    if (!m_pendingAdjustSlotKey.isEmpty()) {
        const int adjustedItemId = DatabaseManager::instance().getItemId(
            planId,
            m_pendingAdjustDate,
            m_pendingAdjustSlotKey);
        if (adjustedItemId > 0)
            DatabaseManager::instance().markItemAdjusted(adjustedItemId);
    }
    clearPendingAdjust();

    if (updatedCount > 0) {
        updatePlanUI();
        updateAllDoneLabel();
        showHomePlanStatus(tr("已更新 %1 个时段的规划。").arg(updatedCount));
    } else if (parseResult.items.isEmpty()) {
        showHomePlanStatus(tr("当前规划已合适，无需调整。"));
    } else {
        showHomePlanStatus(tr("计划调整未更新任何时段。"));
    }
    return;
























}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Request AI plan generation



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::requestAIPlanWhenHomeReady()
{
    QTimer::singleShot(0, this, [this]() {
        if (!ui || !ui->stackedWidget || m_currentUserId <= 0 || m_isRequestPending)
            return;

        if (routeForPageIndex(ui->stackedWidget->currentIndex()) != AppRoute::Home)
            return;

        requestAIPlan();
    });
}

void MainWindow::requestAIPlan()



{



    if (m_isRequestPending) {



        qWarning() << "requestAIPlan: Another AI request is already in progress.";



        return;



    }



    QString userProfile = buildUserProfileString();



    if (userProfile.isEmpty()) {



        AnimatedDialog::warn(this, tr("生成失败"), tr("无法读取用户画像，请重新填写！"));



        return;



    }







    qDebug() << "=== requestAIPlan ===";



    qDebug() << "Sending profile to AI:" << userProfile;


    if (!AIManager::hasChatApiKey()) {
        m_isRequestPending = false;
        hideLoadingBar();
        showHomePlanStatus(uiText("\u0041\u0049 \u670d\u52a1\u672a\u914d\u7f6e\uff0c\u6682\u65f6\u65e0\u6cd5\u751f\u6210\u8ba1\u5212\u3002"));
        return;
    }







    // Show a loading hint in the UI



    ui->lblGoalContent->setText(tr("\U0001F916 AI 正在为你生成计划，请稍等..."));
    if (m_goalContentEdit)
        m_goalContentEdit->setPlainText(tr("\U0001F916 AI 正在为你生成计划，请稍等..."));
    m_goalText = tr("\U0001F916 AI 正在为你生成计划，请稍等...");



    showLoadingBar(tr("正在生成三日健康计划"));







    // Async call - result comes back via onPlanGenerated signal



    m_isRequestPending = true;



    AIManager::instance().generatePlan(userProfile);



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Request AI plan with a custom prompt (overloaded)



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲















// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Show/hide the bottom loading bar with status text



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::showLoadingBar(const QString &message)



{



    if (!m_loadingLabel || !m_typewriterTimer)
        return;

    m_typewriterFullText = message.isEmpty()
        ? QStringLiteral("AI 正在轻轻整理中")
        : message;



    m_typewriterPos = 0;



    m_loadingLabel->setText(m_typewriterFullText);



    m_loadingLabel->show();

    if (m_loadingOverlay)
        m_loadingOverlay->showLoading(m_typewriterFullText);



    if (m_breathAnim)
        m_breathAnim->start();



    m_typewriterTimer->start(260);



}







void MainWindow::hideLoadingBar()



{



    if (!m_loadingLabel || !m_typewriterTimer)
        return;

    if (m_breathAnim)
        m_breathAnim->stop();



    m_typewriterTimer->stop();



    m_loadingLabel->hide();

    if (m_loadingOverlay)
        m_loadingOverlay->hideLoading();



    m_typewriterFullText.clear();



    m_typewriterPos = 0;



}

void MainWindow::showHomePlanStatus(const QString &message)
{
    m_goalText = message;
    if (ui && ui->lblGoalContent)
        ui->lblGoalContent->setText(message);
    if (m_goalContentEdit)
        m_goalContentEdit->setPlainText(message);
    if (statusBar())
        statusBar()->showMessage(message, 4500);
}

void MainWindow::showAnalysisStatus(const QString &message)
{
    QLabel *status = ui && ui->stackedWidget
        ? ui->stackedWidget->findChild<QLabel *>(QStringLiteral("analysisStatusLabel"))
        : nullptr;
    if (status) {
        status->setText(message);
        status->show();
    }
    if (statusBar())
        statusBar()->showMessage(message, 4500);
}

void MainWindow::showReportStatus(const QString &message)
{
    QLabel *status = ui && ui->stackedWidget
        ? ui->stackedWidget->findChild<QLabel *>(QStringLiteral("reportStatusLabel"))
        : nullptr;
    if (status) {
        status->setText(message);
        status->show();
    }
    if (statusBar())
        statusBar()->showMessage(message, 4500);
}

void showUserActionNotice(QWidget *parent,
                          const QString &title,
                          const QString &message,
                          const std::function<void(const QString &)> &statusSink)
{
    statusSink(message);
    AnimatedDialog::info(parent, title, message);
}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Handle AI plan generation result (full plan)



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::onPlanGenerated(const QString &jsonResult)



{



    m_isRequestPending = false;



    hideLoadingBar();

    qDebug() << "=== onPlanGenerated ===";
    qDebug() << "Raw JSON:" << jsonResult;

    const auto parseResult = LifeBalanceAI::Services::HomePlanService::parseGeneratedPlan(
        jsonResult,
        QDate::currentDate());
    if (!parseResult.ok) {
        qWarning() << "onPlanGenerated: parse failed:" << parseResult.errorMessage;
        showHomePlanStatus(tr("计划生成失败，可稍后重试。"));
        return;
    }

    QDate today = QDate::currentDate();
    int existingPlanId = DatabaseManager::instance().getLatestPlanId(m_currentUserId);
    int planId = -1;
    bool reusePlan = false;

    if (existingPlanId > 0) {
        auto existingTodayItems = DatabaseManager::instance().getItemsForDate(existingPlanId, today);
        if (!existingTodayItems.isEmpty()) {
            planId = existingPlanId;
            reusePlan = true;
            qDebug() << "onPlanGenerated: Reusing existing plan ID:" << planId;
        }
    }

    if (planId < 0) {
        planId = DatabaseManager::instance().createPlan(
            m_currentUserId, QStringLiteral("3-day"), parseResult.longTermText);
        if (planId < 0) {
            showHomePlanStatus(tr("创建计划失败，请稍后重试。"));
            return;
        }
        qDebug() << "Created plan with ID:" << planId;
    }

    int savedCount = 0;
    for (const auto &item : parseResult.items) {
        if (reusePlan) {
            auto existingItems = DatabaseManager::instance().getItemsForDate(planId, item.date);
            if (!existingItems.isEmpty())
                continue;
        }

        if (DatabaseManager::instance().saveDailyItem(
                planId,
                item.date.toString(QStringLiteral("yyyy-MM-dd")),
                item.timeSlot,
                item.content)) {
            ++savedCount;
        }
    }

    qDebug() << "Saved" << savedCount << "daily items.";
    showHomePlanStatus(tr("AI 计划已生成并保存。"));
    loadMainPage();
    updatePlanUI();
    return;






















































}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Tab change handler 锟?restrict cross-day operations



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::onTabChanged(int index)



{



    if (index == 0) {



        loadMainPage();



    }



    // Force the newly-visible tab's layout to complete before populating
    // text.  On Android the QScrollArea viewport may still have 0 width
    // when the currentChanged signal fires.
    QWidget *tab = ui->tabWidgetDays->widget(index);
    if (tab && tab->layout())
        tab->layout()->activate();
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    updatePlanUI();
    updateButtonStatesForTab();



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Update button states based on current tab index



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::updateButtonStatesForTab()



{



    int tabIndex = ui->tabWidgetDays->currentIndex();



    bool isToday = (tabIndex == 0);







    // Get only the buttons relevant to the current tab



    // Today buttons: indices 0-3, Tomorrow: 4-7, DayAfter: 8-11



    auto getButtonsForTab = [&](int start, int end) -> QList<QPushButton *> {



        QList<QPushButton *> result;



        for (int i = start; i <= end && i < m_allFeedbackButtons.size(); ++i)



            result.append(m_allFeedbackButtons[i]);



        for (int i = start; i <= end && i < m_allAdjustButtons.size(); ++i)



            result.append(m_allAdjustButtons[i]);



        return result;



    };







    QList<QPushButton *> buttonsToToggle;



    if (tabIndex == 0) {



        buttonsToToggle = getButtonsForTab(0, 3);



    } else if (tabIndex == 1) {



        buttonsToToggle = getButtonsForTab(4, 7);



    } else {



        buttonsToToggle = getButtonsForTab(8, 11);



    }







    // Feedback buttons: only enabled on today tab



    // Adjust buttons: per-slot state managed by updatePlanUI (disabled when feedback done)



    for (int i = 0; i < buttonsToToggle.size(); ++i) {



        QPushButton *btn = buttonsToToggle[i];



        bool isAdjustBtn = m_allAdjustButtons.contains(btn);



        if (!isAdjustBtn)



            btn->setEnabled(isToday);



    }



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Update plan UI with enhanced button states + all-done label



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::updatePlanUI()



{



    if (m_currentUserId < 0)



        return;







    int planId = DatabaseManager::instance().getLatestPlanId(m_currentUserId);



    if (planId < 0) {



        qDebug() << "updatePlanUI: No plan found for user" << m_currentUserId;



        return;



    }







    QSqlDatabase &db = DatabaseManager::instance().database();



    QSqlQuery query(db);



    query.prepare(QStringLiteral(



        "SELECT date, time_slot, content, id, is_done FROM Daily_Items "



        "WHERE plan_id = :pid ORDER BY date, id"



    ));



    query.bindValue(QStringLiteral(":pid"), planId);







    if (!query.exec()) {



        qWarning() << "updatePlanUI: query failed:" << query.lastError().text();



        return;



    }







    // 鈹€鈹€ Mapping structure 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    struct SlotMap {



        QTextEdit   *textEdit    = nullptr;



        QPushButton *btnFeedback = nullptr;



        QPushButton *btnAdjust   = nullptr;



    };







    struct DayGroup {



        SlotMap morning;



        SlotMap lunch;



        SlotMap dinner;



        SlotMap exercise;



    };







    QMap<int, DayGroup> dayWidgets;



    {



        DayGroup today;



        today.morning.textEdit    = ui->txtTodayMorning;



        today.morning.btnFeedback = ui->btnFeedbackTodayMorning;



        today.morning.btnAdjust   = m_btnAdjustTodayMorning;



        today.lunch.textEdit      = ui->txtTodayLunch;



        today.lunch.btnFeedback   = ui->btnFeedbackTodayLunch;



        today.lunch.btnAdjust     = m_btnAdjustTodayLunch;



        today.dinner.textEdit     = ui->txtTodayDinner;



        today.dinner.btnFeedback  = ui->btnFeedbackTodayDinner;



        today.dinner.btnAdjust    = m_btnAdjustTodayDinner;



        today.exercise.textEdit   = ui->txtTodayExercise;



        today.exercise.btnFeedback = ui->btnFeedbackTodayExercise;



        today.exercise.btnAdjust   = m_btnAdjustTodayExercise;



        dayWidgets[0] = today;







        DayGroup tomorrow;



        tomorrow.morning.textEdit    = ui->txtTomorrowMorning;



        tomorrow.morning.btnFeedback = ui->btnFeedbackTomorrowMorning;



        tomorrow.morning.btnAdjust   = m_btnAdjustTomorrowMorning;



        tomorrow.lunch.textEdit      = ui->txtTomorrowLunch;



        tomorrow.lunch.btnFeedback   = ui->btnFeedbackTomorrowLunch;



        tomorrow.lunch.btnAdjust     = m_btnAdjustTomorrowLunch;



        tomorrow.dinner.textEdit     = ui->txtTomorrowDinner;



        tomorrow.dinner.btnFeedback  = ui->btnFeedbackTomorrowDinner;



        tomorrow.dinner.btnAdjust    = m_btnAdjustTomorrowDinner;



        tomorrow.exercise.textEdit   = ui->txtTomorrowExercise;



        tomorrow.exercise.btnFeedback = ui->btnFeedbackTomorrowExercise;



        tomorrow.exercise.btnAdjust   = m_btnAdjustTomorrowExercise;



        dayWidgets[1] = tomorrow;







        DayGroup dayAfter;



        dayAfter.morning.textEdit    = ui->txtDayAfterMorning;



        dayAfter.morning.btnFeedback = ui->btnFeedbackDayAfterMorning;



        dayAfter.morning.btnAdjust   = m_btnAdjustDayAfterMorning;



        dayAfter.lunch.textEdit      = ui->txtDayAfterLunch;



        dayAfter.lunch.btnFeedback   = ui->btnFeedbackDayAfterLunch;



        dayAfter.lunch.btnAdjust     = m_btnAdjustDayAfterLunch;



        dayAfter.dinner.textEdit     = ui->txtDayAfterDinner;



        dayAfter.dinner.btnFeedback  = ui->btnFeedbackDayAfterDinner;



        dayAfter.dinner.btnAdjust    = m_btnAdjustDayAfterDinner;



        dayAfter.exercise.textEdit   = ui->txtDayAfterExercise;



        dayAfter.exercise.btnFeedback = ui->btnFeedbackDayAfterExercise;



        dayAfter.exercise.btnAdjust   = m_btnAdjustDayAfterExercise;



        dayWidgets[2] = dayAfter;



    }







    QDate today = QDate::currentDate();







    // First pass: reset all widgets



    for (auto it = dayWidgets.begin(); it != dayWidgets.end(); ++it) {



        DayGroup &dg = it.value();



        for (SlotMap *sm : {&dg.morning, &dg.lunch, &dg.dinner, &dg.exercise}) {



            if (sm->textEdit) {



                sm->textEdit->setPlainText(QString());



                // Reset palette instead of setStyleSheet to preserve QSS



                sm->textEdit->setPalette(QPalette());



            }



            if (sm->btnFeedback) {



                sm->btnFeedback->setText(tr("反馈"));



                // Clear any palette override to let QSS fully control



                sm->btnFeedback->setPalette(QPalette());



            }



            if (sm->btnAdjust) {



                sm->btnAdjust->setText(tr("调整"));



                sm->btnAdjust->setPalette(QPalette());



            }



        }



    }







    // Second pass: populate from DB and apply is_done state



    // Use QPalette instead of setStyleSheet() to preserve QSS theme rules.



    // setStyleSheet() on individual widgets OVERRIDES the global QSS,



    // which would wipe all styling (border, padding, border-radius, etc.)



    const QColor doneBg(246, 255, 237);   // #F6FFED success background



    const QColor doneBtnBg(183, 235, 143); // #B7EB8F success border/accent







    while (query.next()) {



        QDate itemDate  = QDate::fromString(query.value(0).toString(), QStringLiteral("yyyy-MM-dd"));



        QString slot    = query.value(1).toString();



        QString content = query.value(2).toString();



        int itemId      = query.value(3).toInt();



        QString doneStr = query.value(4).toString();



        bool isDone     = (doneStr == QStringLiteral("1"));







        int daysDiff = today.daysTo(itemDate);



        if (daysDiff < 0 || daysDiff > 2)



            continue;







        DayGroup &dg = dayWidgets[daysDiff];



        SlotMap *sm = nullptr;







        if (slot == QStringLiteral("breakfast")) sm = &dg.morning;



        else if (slot == QStringLiteral("lunch"))   sm = &dg.lunch;



        else if (slot == QStringLiteral("dinner"))  sm = &dg.dinner;



        else if (slot == QStringLiteral("sports"))  sm = &dg.exercise;







        if (!sm) continue;







        // 鈹€鈹€ Set text content + done background via QPalette 鈹€鈹€



        if (sm->textEdit) {



            if (isDone) {



                sm->textEdit->setPlainText(QStringLiteral("[已打卡] ") + content);



                QPalette pal = sm->textEdit->palette();



                pal.setColor(QPalette::Base, doneBg);



                sm->textEdit->setPalette(pal);



            } else {



                sm->textEdit->setPlainText(content);



            }



        }







        // 鈹€鈹€ Update feedback button (text + palette, not setStyleSheet) 鈹€鈹€



        if (sm->btnFeedback) {



            QString existingFeedback = DatabaseManager::instance().getFeedbackContent(itemId);



            bool hasFeedback = !existingFeedback.isEmpty();







            if (isDone && hasFeedback) {



                sm->btnFeedback->setText(tr("修改反馈"));



                QPalette pal = sm->btnFeedback->palette();



                pal.setColor(QPalette::Button, doneBtnBg);



                sm->btnFeedback->setPalette(pal);



            } else if (isDone && !hasFeedback) {



                sm->btnFeedback->setText(tr("反馈（已打卡）"));



                QPalette pal = sm->btnFeedback->palette();



                pal.setColor(QPalette::Button, doneBtnBg);



                sm->btnFeedback->setPalette(pal);



            } else {



                sm->btnFeedback->setText(tr("反馈"));



                sm->btnFeedback->setPalette(QPalette());



            }



            sm->btnFeedback->setEnabled(true);



        }







        // 鈹€鈹€ Update adjust button 鈹€鈹€



        if (sm->btnAdjust) {



            sm->btnAdjust->setEnabled(!isDone);



            if (isDone) {



                QPalette pal = sm->btnAdjust->palette();



                pal.setColor(QPalette::Button, doneBtnBg);



                sm->btnAdjust->setPalette(pal);



            } else {



                sm->btnAdjust->setPalette(QPalette());



            }



        }



    }







    // 鈹€鈹€ Apply tab-based restrictions 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    updateButtonStatesForTab();







    // 鈹€鈹€ Update all-done label 鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€鈹€



    updateAllDoneLabel();







    auto tunePlanTextHeight = [](QTextEdit *textEdit) {
        if (!textEdit)
            return;
#ifdef Q_OS_ANDROID
        textEdit->setMinimumHeight(84);
        textEdit->setMaximumHeight(90);
#else
        textEdit->setMinimumHeight(120);
        textEdit->setMaximumHeight(QWIDGETSIZE_MAX);
#endif
        textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        textEdit->setLineWrapMode(QTextEdit::WidgetWidth);
        textEdit->setReadOnly(true);
        textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
        textEdit->setFocusPolicy(Qt::NoFocus);
        QFont planFont = textEdit->font();
        planFont.setWeight(QFont::Normal);
#ifdef Q_OS_ANDROID
        planFont.setPointSize(11);
#endif
        textEdit->setFont(planFont);
        textEdit->document()->setDefaultFont(planFont);
        textEdit->document()->setDocumentMargin(4);
#ifdef Q_OS_ANDROID
        if (textEdit->verticalScrollBar()) {
            textEdit->verticalScrollBar()->setFixedWidth(0);
            textEdit->verticalScrollBar()->hide();
        }
        if (textEdit->viewport())
            QScroller::grabGesture(textEdit->viewport(), QScroller::TouchGesture);
#endif
        textEdit->updateGeometry();
    };

    for (auto it = dayWidgets.begin(); it != dayWidgets.end(); ++it) {
        DayGroup &dg = it.value();
        for (SlotMap *sm : {&dg.morning, &dg.lunch, &dg.dinner, &dg.exercise})
            tunePlanTextHeight(sm->textEdit);
    }

    for (QGroupBox *group : ui->tabWidgetDays->findChildren<QGroupBox *>()) {
#ifdef Q_OS_ANDROID
        constexpr int cardHeight = 138;
#else
        constexpr int cardHeight = 242;
#endif
        group->setMinimumHeight(cardHeight);
        group->setMaximumHeight(cardHeight);
    }

    for (QScrollArea *scrollArea : ui->tabWidgetDays->findChildren<QScrollArea *>(QStringLiteral("planScrollArea"))) {
        QWidget *content = scrollArea->widget();
        auto *layout = content ? qobject_cast<QVBoxLayout *>(content->layout()) : nullptr;
        if (!content || !layout)
            continue;
        const int hintHeight = qMax(layout->sizeHint().height(), content->minimumSizeHint().height());
        content->setMinimumHeight(hintHeight);
        content->setMaximumHeight(hintHeight);
        // setWidgetResizable(true) may fail to resize the content widget when
        // the QScrollArea first becomes visible on Android.  Force it here.
        const int vpW = scrollArea->viewport() ? scrollArea->viewport()->width() : 0;
        if (vpW > 0) {
            content->setMinimumWidth(0);
            content->setMaximumWidth(vpW);
        }
        content->resize(qMax(vpW, 1), hintHeight);
        // Re-activate after the resize so children (QTextEdits) pick up the
        // new content width.  layout->activate() above ran with the old size.
        layout->activate();
        scrollArea->verticalScrollBar()->setSingleStep(48);
        scrollArea->verticalScrollBar()->setPageStep(qMax(120, scrollArea->viewport()->height() - 48));
    }

    // Force correct text width on all plan QTextEdits.  On Android the
    // resize cascade through QScrollArea may not have reached them yet.
    {
        const QList<QTextEdit *> edits = {
            ui->txtTodayMorning, ui->txtTodayLunch, ui->txtTodayDinner, ui->txtTodayExercise,
            ui->txtTomorrowMorning, ui->txtTomorrowLunch, ui->txtTomorrowDinner, ui->txtTomorrowExercise,
            ui->txtDayAfterMorning, ui->txtDayAfterLunch, ui->txtDayAfterDinner, ui->txtDayAfterExercise
        };
        for (QTextEdit *edit : edits)
            forceTextWidth(edit);
    }

    qDebug() << "updatePlanUI: plan UI updated with is_done & all-done state.";



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Update the "浠婃棩宸插叏閮ㄦ墦锟? label



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::updateAllDoneLabel()
{
    if (!m_lblAllDone) return;

    // The top-bar completion pill used to push the header wider than the phone
    // viewport after all slots were checked in. The card/button states already
    // communicate completion, so the standalone pill stays hidden.
    m_lblAllDone->hide();
}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Yesterday view handler



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::onViewYesterdayClicked()



{



    if (m_currentUserId < 0) {



        AnimatedDialog::warn(this, tr("错误"), tr("未检测到登录用户！"));



        return;



    }







    int planId = DatabaseManager::instance().getLatestPlanId(m_currentUserId);



    if (planId < 0) {



        AnimatedDialog::info(this, tr("暂无记录"),



                                 tr("这是你的第一天，暂无历史记录。"));



        return;



    }







    QList<DatabaseManager::DailyItemRow> yesterdayItems =



        DatabaseManager::instance().getYesterdayItems(planId);







    if (yesterdayItems.isEmpty()) {



        AnimatedDialog::info(this, tr("暂无记录"),



                                 tr("这是你的第一天，暂无历史记录。"));



        return;



    }







    // Build a formatted text display



    QString display;



    display += tr("\U0001F4CB 昨日规划：%1\n\n").arg(



        QDate::currentDate().addDays(-1).toString(QStringLiteral("yyyy-MM-dd")));







    for (const auto &item : yesterdayItems) {



        QString slotChinese;



        if (item.timeSlot == QStringLiteral("breakfast")) slotChinese = tr("\U0001F373 早餐");



        else if (item.timeSlot == QStringLiteral("lunch"))    slotChinese = tr("\U0001F35A 午餐");



        else if (item.timeSlot == QStringLiteral("dinner"))   slotChinese = tr("\U0001F31B 晚餐");



        else if (item.timeSlot == QStringLiteral("sports"))   slotChinese = tr("\U0001F3C3 运动");



        else slotChinese = item.timeSlot;







        QString status = item.isDone ? tr("✅ 已完成") : tr("❌ 未完成");







        display += QStringLiteral("  %1  %2\n%3\n\n").arg(slotChinese, status, item.content);



    }







    QDialog reviewDialog(this);
    reviewDialog.setObjectName(QStringLiteral("yesterdayReviewDialog"));
    reviewDialog.setWindowTitle(tr("昨日规划回顾"));
    reviewDialog.setModal(true);
    reviewDialog.setAttribute(Qt::WA_StyledBackground, true);

#ifdef Q_OS_ANDROID
    const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
    const int dialogW = qBound(300, available.width() * 86 / 100, 360);
    const int dialogH = qBound(320, available.height() * 58 / 100, 420);
    reviewDialog.setFixedSize(dialogW, dialogH);
#else
    reviewDialog.resize(460, 520);
    reviewDialog.setMinimumSize(420, 440);
#endif

    auto *layout = new QVBoxLayout(&reviewDialog);
#ifdef Q_OS_ANDROID
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(10);
#else
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(12);
#endif

    auto *title = new QLabel(tr("昨日规划回顾"), &reviewDialog);
    title->setObjectName(QStringLiteral("dialogTitle"));
    title->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    layout->addWidget(title);

    auto *content = new QTextEdit(&reviewDialog);
    content->setObjectName(QStringLiteral("yesterdayReviewText"));
    content->setReadOnly(true);
    content->setTextInteractionFlags(Qt::NoTextInteraction);
    content->setPlainText(display);
    content->setLineWrapMode(QTextEdit::WidgetWidth);
    content->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    content->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(content, 1);

    auto *closeButton = new QPushButton(tr("知道了"), &reviewDialog);
    closeButton->setProperty("class", QStringLiteral("primary"));
    closeButton->setMinimumHeight(40);
    closeButton->setMaximumHeight(44);
    layout->addWidget(closeButton);
    connect(closeButton, &QPushButton::clicked, &reviewDialog, &QDialog::accept);

    reviewDialog.setStyleSheet(QStringLiteral(
        "QDialog#yesterdayReviewDialog{background:#FFFDF9;border:1px solid #E8DED2;border-radius:16px;}"
        "QLabel#dialogTitle{font-family:\"MiSans Medium\",\"MiSans\",\"Noto Sans SC\",\"Microsoft YaHei UI\";"
        "font-size:16px;font-weight:600;color:#1F2A24;background:transparent;padding:0 0 2px 0;}"
        "QTextEdit#yesterdayReviewText{background:#FFFDF9;border:1px solid #F1E9DF;border-radius:10px;"
        "padding:10px;color:#48423C;font-size:13px;line-height:1.6;}"
    ));

#ifdef Q_OS_ANDROID
    LifeBalanceAI::Ui::PlatformLayoutPolicy::centerWidgetOnSafeArea(&reviewDialog);
#endif
    reviewDialog.exec();



}







// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲



// Page 5 锟?Personal Center (Profile Viewing & Editing)



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::loadPage5()



{
    if (ui->page_5)
        UiFactory::applyWarmPolish(ui->page_5);



    if (m_currentUserId < 0)



        return;







    auto &db = DatabaseManager::instance();



    LifeBalanceAI::Models::UserInfo info = db.getUserInfo(m_currentUserId);



    LifeBalanceAI::Models::ProfileData profile = db.getProfile(m_currentUserId);

#ifdef Q_OS_ANDROID
    if (ui->lblPage5Title) {
        ui->lblPage5Title->setMinimumHeight(46);
        ui->lblPage5Title->setMaximumHeight(56);
        ui->lblPage5Title->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }
    if (ui->groupBoxUserInfo) {
        ui->groupBoxUserInfo->setFixedHeight(142);
        ui->groupBoxUserInfo->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
        if (auto *form = qobject_cast<QFormLayout *>(ui->groupBoxUserInfo->layout())) {
            form->setContentsMargins(14, 12, 14, 10);
            form->setHorizontalSpacing(14);
            form->setVerticalSpacing(5);
            form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        }
    }
    if (ui->groupBoxEditProfile) {
        ui->groupBoxEditProfile->setFixedHeight(372);
        ui->groupBoxEditProfile->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }
#endif







    // 鈹€鈹€ Account info 鈹€鈹€



    // Nickname display (add to account info group)



    QLabel *lblNickname = ui->page_5->findChild<QLabel *>(QStringLiteral("lblNickname"));



    if (!lblNickname) {



        // First time - add nickname row to the form layout



        QFormLayout *formLayout = qobject_cast<QFormLayout *>(ui->groupBoxUserInfo->layout());



        if (formLayout) {



            QLabel *nickLabel = new QLabel(tr("昵称"), ui->page_5);



            lblNickname = new QLabel(ui->page_5);



            lblNickname->setObjectName(QStringLiteral("lblNickname"));



            formLayout->insertRow(0, nickLabel, lblNickname);



        }



    }



    if (lblNickname)



        lblNickname->setText(profile.nickname.isEmpty() ? tr("(未设置)") : profile.nickname);







    ui->lblPhone->setText(info.phone);



    // Map role to Chinese display name



    QString roleDisplay = info.role;



    if (info.role == QStringLiteral("Ascendant"))



        roleDisplay = QStringLiteral("律行者");



    else if (info.role == QStringLiteral("Explorer"))



        roleDisplay = QStringLiteral("探索者");



    else if (info.role == QStringLiteral("User"))



        roleDisplay = QStringLiteral("普通用户");



    ui->lblRole->setText(roleDisplay);







    ui->lblPage5Streak->setText(



        tr("连续 %1 天 | 累计 %2 天 | 冻结卡: %3")



            .arg(info.streakDays).arg(info.totalCheckins).arg(info.freezeCards));







    // 鈹€鈹€ Profile display (read-only) 鈹€鈹€



    ui->groupBoxEditProfile->setTitle(tr("用户画像"));







    // Uniform read-only styling for all profile fields



    ui->editNickname5->setReadOnly(true);



    ui->editNickname5->setProperty("class", QStringLiteral("profileDisplay"));
    ui->editNickname5->setStyleSheet(QString());



    ui->spinBoxAge5->setReadOnly(true);



    ui->spinBoxAge5->setButtonSymbols(QAbstractSpinBox::NoButtons);



    ui->spinBoxAge5->setProperty("class", QStringLiteral("profileDisplay"));
    ui->spinBoxAge5->setStyleSheet(QString());



    ui->editHeight5->setReadOnly(true);



    ui->editHeight5->setProperty("class", QStringLiteral("profileDisplay"));
    ui->editHeight5->setStyleSheet(QString());



    ui->editWeight5->setReadOnly(true);



    ui->editWeight5->setProperty("class", QStringLiteral("profileDisplay"));
    ui->editWeight5->setStyleSheet(QString());



    ui->comboGender5->setEnabled(false);



    ui->comboGender5->setFocusPolicy(Qt::NoFocus);



    ui->comboGender5->setProperty("class", QStringLiteral("profileDisplay"));
    ui->comboGender5->setStyleSheet(QString());



    ui->editAllergy5->setReadOnly(true);



    ui->editAllergy5->setProperty("class", QStringLiteral("profileDisplay"));
    ui->editAllergy5->setStyleSheet(QString());



    ui->editGoal5->setReadOnly(true);



    ui->editGoal5->setProperty("class", QStringLiteral("profileDisplay"));
    ui->editGoal5->setStyleSheet(QString());

    if (auto *form = qobject_cast<QFormLayout *>(ui->groupBoxEditProfile->layout())) {
#ifdef Q_OS_ANDROID
        form->setContentsMargins(14, 12, 14, 12);
        form->setHorizontalSpacing(14);
        form->setVerticalSpacing(6);
#else
        form->setContentsMargins(12, 14, 12, 14);
        form->setHorizontalSpacing(16);
        form->setVerticalSpacing(10);
#endif
        form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        form->setRowWrapPolicy(QFormLayout::DontWrapRows);
        form->setLabelAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    }

    auto compactProfileField = [](QWidget *widget) {
        if (!widget)
            return;
        widget->setFocusPolicy(Qt::NoFocus);
        widget->setMinimumWidth(0);
#ifdef Q_OS_ANDROID
        widget->setMinimumHeight(26);
        widget->setMaximumHeight(34);
#else
        widget->setMinimumHeight(34);
        widget->setMaximumHeight(42);
#endif
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    };
    auto polishProfileField = [](QWidget *widget) {
        if (!widget)
            return;
        widget->style()->unpolish(widget);
        widget->style()->polish(widget);
        widget->update();
    };
    for (QWidget *field : {static_cast<QWidget *>(ui->editNickname5),
                           static_cast<QWidget *>(ui->spinBoxAge5),
                           static_cast<QWidget *>(ui->editHeight5),
                           static_cast<QWidget *>(ui->editWeight5),
                           static_cast<QWidget *>(ui->comboGender5),
                           static_cast<QWidget *>(ui->editAllergy5),
                           static_cast<QWidget *>(ui->editGoal5)}) {
        compactProfileField(field);
        polishProfileField(field);
    }

    QTimer::singleShot(0, ui->page_5, [this, polishProfileField]() {
        for (QWidget *field : {static_cast<QWidget *>(ui->editNickname5),
                               static_cast<QWidget *>(ui->spinBoxAge5),
                               static_cast<QWidget *>(ui->editHeight5),
                               static_cast<QWidget *>(ui->editWeight5),
                               static_cast<QWidget *>(ui->comboGender5),
                               static_cast<QWidget *>(ui->editAllergy5),
                               static_cast<QWidget *>(ui->editGoal5)}) {
            polishProfileField(field);
        }
        if (ui->groupBoxEditProfile && ui->groupBoxEditProfile->layout()) {
            ui->groupBoxEditProfile->layout()->invalidate();
            ui->groupBoxEditProfile->layout()->activate();
        }
    });

    for (QLabel *label : ui->groupBoxEditProfile->findChildren<QLabel *>()) {
#ifdef Q_OS_ANDROID
        label->setMinimumHeight(26);
        label->setMaximumHeight(34);
#else
        label->setMinimumHeight(34);
#endif
        label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    }







    // Populate values



    ui->editNickname5->setText(profile.nickname.isEmpty() ? tr("(未设置)") : profile.nickname);



    ui->spinBoxAge5->setValue(profile.age);



    ui->editHeight5->setText(profile.height > 0 ? QString::number(profile.height, 'f', 1) + QStringLiteral(" cm") : tr("(未设置)"));



    ui->editWeight5->setText(profile.weight > 0 ? QString::number(profile.weight, 'f', 1) + QStringLiteral(" kg") : tr("(未设置)"));



    const int genderIndex = ui->comboGender5->findText(profile.gender);
    if (genderIndex >= 0)
        ui->comboGender5->setCurrentIndex(genderIndex);



    ui->editAllergy5->setText(profile.allergy.isEmpty() ? tr("(无)") : profile.allergy);



    ui->editGoal5->setText(profile.goal.isEmpty() ? tr("(未设置)") : profile.goal);

    auto markInfoLabel = [](QLabel *label, const QString &className, bool wrap = false) {
        if (!label)
            return;
        label->setProperty("class", className);
        label->setWordWrap(wrap);
        label->setMinimumWidth(0);
        label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        label->style()->unpolish(label);
        label->style()->polish(label);
    };

    for (QLabel *label : {ui->page_5->findChild<QLabel *>(QStringLiteral("label_phone")),
                          ui->page_5->findChild<QLabel *>(QStringLiteral("lblRoleTitle")),
                          ui->page_5->findChild<QLabel *>(QStringLiteral("label_streak"))}) {
        markInfoLabel(label, QStringLiteral("profileKey"));
    }
    for (QLabel *label : {lblNickname, ui->lblPhone, ui->lblRole, ui->lblPage5Streak}) {
        markInfoLabel(label, QStringLiteral("profileValue"), true);
    }

    if (auto *form = qobject_cast<QFormLayout *>(ui->groupBoxEditProfile->layout())) {
        auto installValueLabel = [this, form, markInfoLabel](QWidget *field,
                                                             const QString &name,
                                                             const QString &text) {
            if (!field || !ui->groupBoxEditProfile)
                return;

            QLabel *value = ui->groupBoxEditProfile->findChild<QLabel *>(name);
            int row = -1;
            QFormLayout::ItemRole role = QFormLayout::FieldRole;
            form->getWidgetPosition(field, &row, &role);

            if (!value) {
                if (row < 0)
                    return;
                value = new QLabel(ui->groupBoxEditProfile);
                value->setObjectName(name);
                form->removeWidget(field);
                field->hide();
                form->setWidget(row, QFormLayout::FieldRole, value);
            }

            value->setText(text);
            value->setTextInteractionFlags(Qt::TextSelectableByMouse);
            value->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            markInfoLabel(value, QStringLiteral("profileValue"), true);
        };

        installValueLabel(ui->editNickname5, QStringLiteral("profileValueNickname"),
                          ui->editNickname5 ? ui->editNickname5->text() : QString());
        installValueLabel(ui->spinBoxAge5, QStringLiteral("profileValueAge"),
                          ui->spinBoxAge5 ? QString::number(ui->spinBoxAge5->value()) : QString());
        installValueLabel(ui->editHeight5, QStringLiteral("profileValueHeight"),
                          ui->editHeight5 ? ui->editHeight5->text() : QString());
        installValueLabel(ui->editWeight5, QStringLiteral("profileValueWeight"),
                          ui->editWeight5 ? ui->editWeight5->text() : QString());
        installValueLabel(ui->comboGender5, QStringLiteral("profileValueGender"),
                          ui->comboGender5 ? ui->comboGender5->currentText() : QString());
        installValueLabel(ui->editAllergy5, QStringLiteral("profileValueAllergy"),
                          ui->editAllergy5 ? ui->editAllergy5->text() : QString());
        installValueLabel(ui->editGoal5, QStringLiteral("profileValueGoal"),
                          ui->editGoal5 ? ui->editGoal5->text() : QString());

        for (QLabel *label : ui->groupBoxEditProfile->findChildren<QLabel *>()) {
            if (label->property("class").toString() != QStringLiteral("profileValue"))
                markInfoLabel(label, QStringLiteral("profileKey"));
        }
    }

    for (QLineEdit *lineEdit : {ui->editNickname5, ui->editHeight5, ui->editWeight5,
                                ui->editAllergy5, ui->editGoal5}) {
        if (!lineEdit)
            continue;
        lineEdit->deselect();
        lineEdit->setCursorPosition(0);
        lineEdit->clearFocus();
    }
    if (ui->spinBoxAge5)
        ui->spinBoxAge5->clearFocus();
    if (ui->comboGender5)
        ui->comboGender5->clearFocus();
    if (ui->page_5)
        ui->page_5->setFocus(Qt::OtherFocusReason);







    // Add "修改画像" button at bottom of the profile group box



    QPushButton *btnEditProfile = ui->groupBoxEditProfile->findChild<QPushButton *>(QStringLiteral("btnEditProfile"));



    if (!btnEditProfile) {



        QFormLayout *formL = qobject_cast<QFormLayout *>(ui->groupBoxEditProfile->layout());



        if (formL) {



            btnEditProfile = new QPushButton(tr("修改画像"), ui->groupBoxEditProfile);



            btnEditProfile->setObjectName(QStringLiteral("btnEditProfile"));



            btnEditProfile->setMinimumHeight(38);
#ifdef Q_OS_ANDROID
            btnEditProfile->setMaximumHeight(40);
#endif



            UiFactory::setClass(btnEditProfile, QStringLiteral("secondary"));



            connect(btnEditProfile, &QPushButton::clicked, this, [this]() {



                m_profileEditFromPage5 = true;



                LifeBalanceAI::Models::ProfileData existingProfile = DatabaseManager::instance().getProfile(m_currentUserId);
                populateProfileEditFields(existingProfile);



                navigateTo(AppRoute::ProfileSetup, false);



                if (QPushButton *btn = ui->page_3->findChild<QPushButton *>(QStringLiteral("btnBackPage3")))



                    btn->setVisible(true);



            });



            formL->addRow(btnEditProfile);



        }



    }







    // Hide the save button (no longer needed on page_5)



    if (ui->btnSaveProfile5)



    // Hide the save and back buttons (replaced by bottom nav)



    if (ui->btnSaveProfile5)



        ui->btnSaveProfile5->setVisible(false);



    if (ui->btnBackToMain)



        ui->btnBackToMain->setVisible(false);







    // 鈹€鈹€ Bottom: 閫€鍑虹櫥锟?+ 鍗囩骇涓哄緥琛岋拷?鈹€鈹€



    QVBoxLayout *page5Layout = qobject_cast<QVBoxLayout *>(ui->page_5->layout());



    if (page5Layout) {

#ifdef Q_OS_ANDROID
        page5Layout->setContentsMargins(14, 14, 14, 12);
        page5Layout->setSpacing(10);
#endif



        // Ensure logout button exists



        QPushButton *btnLogout = ui->page_5->findChild<QPushButton *>(QStringLiteral("btnLogout"));



        if (!btnLogout) {



            btnLogout = new QPushButton(tr("退出登录"), ui->page_5);



            btnLogout->setObjectName(QStringLiteral("btnLogout"));



            btnLogout->setMinimumHeight(36);



            UiFactory::setClass(btnLogout, QStringLiteral("danger"));



            connect(btnLogout, &QPushButton::clicked, this, [this]() {
                confirmLogoutToLogin();

            });



            // Find the stretch spacer at end and insert before it



            int spacerIdx = -1;



            for (int k = page5Layout->count() - 1; k >= 0; --k) {



                if (page5Layout->itemAt(k)->spacerItem()) { spacerIdx = k; break; }



            }



            page5Layout->insertWidget(spacerIdx > 0 ? spacerIdx : page5Layout->count(), btnLogout);



        }

#ifdef Q_OS_ANDROID
        btnLogout->setMinimumHeight(46);
        btnLogout->setMaximumHeight(50);
        btnLogout->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
#endif







        // Upgrade button (Explorer only)



        LifeBalanceAI::Models::UserInfo info2 = DatabaseManager::instance().getUserInfo(m_currentUserId);



        QPushButton *btnUpgrade = ui->page_5->findChild<QPushButton *>(QStringLiteral("btnUpgrade"));



        if (info2.role != QStringLiteral("Ascendant")) {



            if (!btnUpgrade) {



                btnUpgrade = new QPushButton(tr("升级为律行者 →"), ui->page_5);



                btnUpgrade->setObjectName(QStringLiteral("btnUpgrade"));



                btnUpgrade->setMinimumHeight(40);



                UiFactory::setClass(btnUpgrade, QStringLiteral("primary"));



                connect(btnUpgrade, &QPushButton::clicked, this, [this]() {



                    m_prePaymentPageIndex = 4;



                    navigateTo(AppRoute::Payment, false);



                });



                // Insert before logout



                int logIdx = page5Layout->indexOf(btnLogout);



                page5Layout->insertWidget(logIdx > 0 ? logIdx : page5Layout->count(), btnUpgrade);



            }

#ifdef Q_OS_ANDROID
            btnUpgrade->setMinimumHeight(46);
            btnUpgrade->setMaximumHeight(50);
            btnUpgrade->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
#endif



            btnUpgrade->setVisible(true);



        } else {



            if (btnUpgrade) btnUpgrade->setVisible(false);



        }



#ifdef Q_OS_ANDROID
        ui->page_5->installEventFilter(this);
        updateBottomNavVisibility(ui->page_5 ? ui->stackedWidget->indexOf(ui->page_5) : 4);
#endif

    }



}











void MainWindow::on_btnPay6_clicked()



{



    if (m_currentUserId < 0) {



        AnimatedDialog::warn(this, tr("错误"), tr("未检测到登录用户！"));



        return;



    }







    qDebug() << "on_btnPay6_clicked: attempting upgrade for userId=" << m_currentUserId;



    auto *us = new LifeBalanceAI::Services::UserService(this);



    bool ok = us->requestUpgrade(m_currentUserId);



    qDebug() << "on_btnPay6_clicked: upgrade result=" << ok;



    if (ok) {



        AnimatedDialog::info(this, tr("支付成功"), tr("恭喜你已升级为律行者！立即享受全部高级功能。"));



        loadMainPage();



        updatePlanUI();



        navigateTo(AppRoute::Home, false);  // back to page_4



    } else {



        AnimatedDialog::warn(this, tr("升级失败"), tr("请稍后重试！"));



    }



    us->deleteLater();



}







void MainWindow::on_btnBack6_clicked()



{



    navigateTo(m_prePaymentPageIndex, false);



}







void MainWindow::on_btnSaveProfile5_clicked()
{
    qDebug() << "on_btnSaveProfile5_clicked called";

    if (m_currentUserId < 0) {
        AnimatedDialog::warn(this,
                             QStringLiteral("\u9519\u8bef"),
                             QStringLiteral("\u672a\u68c0\u6d4b\u5230\u767b\u5f55\u7528\u6237\uff01"));
        return;
    }

    const auto saveResult = LifeBalanceAI::Services::ProfileFlowService::saveProfile(
        m_currentUserId,
        profileInputFromProfilePage(),
        LifeBalanceAI::Models::ProfileSaveMode::EditFromProfile);
    if (!saveResult.ok) {
        AnimatedDialog::warn(this, QStringLiteral("\u9519\u8bef"), saveResult.message);
        return;
    }

    AnimatedDialog::info(this, QStringLiteral("\u6210\u529f"), saveResult.message);
    if (saveResult.requestNickname) {
        const QString profileSummary = buildUserProfileString();
        if (!profileSummary.isEmpty())
            AIManager::instance().generateNickname(profileSummary);
    }

    loadMainPage();
    updatePlanUI();
    navigateTo(saveResult.nextRoute, false);
}
void MainWindow::on_btnBackToMain_clicked()



{



    qDebug() << "on_btnBackToMain_clicked called";



    navigateTo(AppRoute::Home, true);



}







// ============================================================



// Deep Analysis



// ============================================================







void MainWindow::onDeepAnalysisTriggered()
{
    qDebug() << "onDeepAnalysisTriggered called";

    if (m_currentUserId < 0) {
        showUserActionNotice(this,
                             uiText("\u63d0\u793a"),
                             uiText("\u672a\u68c0\u6d4b\u5230\u767b\u5f55\u7528\u6237\uff0c\u8bf7\u91cd\u65b0\u767b\u5f55\u540e\u518d\u8bd5\u3002"),
                             [this](const QString &message) { showAnalysisStatus(message); });
        return;
    }

    const QString role = DatabaseManager::instance().getUserRole(m_currentUserId);
    if (role != QStringLiteral("Ascendant")) {
        showUserActionNotice(this,
                             uiText("\u9700\u8981\u5347\u7ea7"),
                             uiText("\u6df1\u5ea6\u5206\u6790\u9700\u8981\u5347\u7ea7\u4e3a\u5f8b\u884c\u8005\u540e\u4f7f\u7528\u3002"),
                             [this](const QString &message) { showAnalysisStatus(message); });
        return;
    }

    if (!m_deepAnalysisService) {
        showUserActionNotice(this,
                             uiText("\u670d\u52a1\u4e0d\u53ef\u7528"),
                             uiText("\u5206\u6790\u670d\u52a1\u672a\u521d\u59cb\u5316\uff0c\u8bf7\u91cd\u542f\u5e94\u7528\u540e\u518d\u8bd5\u3002"),
                             [this](const QString &message) { showAnalysisStatus(message); });
        return;
    }

    if (m_deepAnalysisService->hasAnalysisThisWeek(m_currentUserId)) {
        auto result = m_deepAnalysisService->getLatestAnalysis(m_currentUserId);
        if (!result.nutritionAdvice.isEmpty()) {
            DeepAnalysisDialog dlg(this);
            dlg.setAnalysisResult(result);
            dlg.exec();
            return;
        }
    }

    if (!AIManager::hasChatApiKey()) {
        hideLoadingBar();
        showUserActionNotice(this,
                             uiText("\u0041\u0049 \u672a\u914d\u7f6e"),
                             uiText("\u0041\u0049 \u670d\u52a1\u672a\u914d\u7f6e\uff0c\u6682\u65f6\u65e0\u6cd5\u751f\u6210\u5206\u6790\u3002"),
                             [this](const QString &message) { showAnalysisStatus(message); });
        return;
    }

    showAnalysisStatus(uiText("\u6b63\u5728\u751f\u6210\u6df1\u5ea6\u5206\u6790\uff0c\u8bf7\u7a0d\u7b49..."));
    showLoadingBar(uiText("\u6b63\u5728\u751f\u6210\u6df1\u5ea6\u5206\u6790"));
    m_deepAnalysisService->requestAnalysis(m_currentUserId);
}


void MainWindow::onDeepAnalysisReady(int userId, const LifeBalanceAI::Models::DeepAnalysisResult &result)



{



    Q_UNUSED(userId);



    hideLoadingBar();



    DeepAnalysisDialog dlg(this);



    dlg.setAnalysisResult(result);



    dlg.exec();



}







void MainWindow::onNicknameGenerated(const QString &nickname)
{
    if (m_currentUserId <= 0 || nickname.isEmpty()) return;
    DatabaseManager::instance().updateNickname(m_currentUserId, nickname.trimmed());
    // Refresh sidebar to show new nickname
    setupSideDrawer();
}

void MainWindow::onDeepAnalysisError(int userId, const QString &error)
{
    Q_UNUSED(userId);
    hideLoadingBar();
    qWarning() << "Deep analysis failed:" << error;

    const QString message = AIManager::hasChatApiKey()
        ? uiText("\u5206\u6790\u6682\u65f6\u4e0d\u53ef\u7528\uff0c\u53ef\u7a0d\u540e\u91cd\u8bd5\u3002")
        : uiText("\u0041\u0049 \u670d\u52a1\u672a\u914d\u7f6e\uff0c\u6682\u65f6\u65e0\u6cd5\u751f\u6210\u5206\u6790\u3002");
    showAnalysisStatus(message);
}


// ============================================================



// Report



// ============================================================







void MainWindow::showReportHistory()



{



    if (m_currentUserId < 0) return;







    auto history = DatabaseManager::instance().getReportHistory(m_currentUserId);



    // Filter only reports (not deep_analysis)



    QVector<LifeBalanceAI::Models::ReportData> reports;



    for (const auto &r : history) {



        if (r.type != QStringLiteral("deep_analysis"))



            reports.append(r);



    }







    if (reports.isEmpty()) {



        AnimatedDialog::info(this, tr("我的周报"), tr("还没有生成过周报。"));



        return;



    }







    // Build display text



    QStringList lines;



    for (const auto &r : reports) {



        QString typeLabel = (r.type == QStringLiteral("monthly")) ? tr("月报") : tr("周报");



        lines << QStringLiteral("%1\n%2 ~ %3  ·  ID #%4")



                     .arg(typeLabel, r.startDate, r.endDate)



                     .arg(r.rid);



    }







    // Custom dialog with proper button labels



    QDialog picker(this);
    picker.setObjectName(QStringLiteral("reportHistoryDialog"));
    picker.setWindowTitle(tr("我的周报"));
    picker.setAttribute(Qt::WA_StyledBackground, true);
    picker.setStyleSheet(QStringLiteral(
        "#reportHistoryDialog{"
        "  background:#FFFDF9;"
        "  border:1px solid #E8DED2;"
        "  border-radius:16px;"
        "}"
    ));
#ifdef Q_OS_ANDROID
    const QRect pickerAvailable = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
    picker.setMinimumSize(qMax(300, qMin(pickerAvailable.width(), 430)),
                          qMax(360, qMin(pickerAvailable.height(), 520)));
#else
    picker.setMinimumSize(430, 420);
#endif

    auto *pickerLayout = new QVBoxLayout(&picker);
    pickerLayout->setContentsMargins(18, 16, 18, 16);
    pickerLayout->setSpacing(10);

    auto *pickerTitle = new QLabel(tr("我的周报"), &picker);
    pickerTitle->setObjectName(QStringLiteral("reportHistoryTitle"));
    pickerLayout->addWidget(pickerTitle);

    auto *pickerHint = new QLabel(tr("共 %1 份报告，选择一份查看").arg(reports.size()), &picker);
    pickerHint->setObjectName(QStringLiteral("reportHistoryHint"));
    pickerLayout->addWidget(pickerHint);

    auto *reportList = new QListWidget(&picker);
    reportList->setObjectName(QStringLiteral("reportHistoryList"));
    for (const QString &line : lines) {
        auto *item = new QListWidgetItem(line, reportList);
        item->setSizeHint(QSize(0, 68));
    }
    reportList->setCurrentRow(0);
    reportList->setWordWrap(true);
    reportList->setTextElideMode(Qt::ElideNone);
    reportList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    pickerLayout->addWidget(reportList, 1);

    auto *pickerButtons = new QHBoxLayout();
    pickerButtons->setSpacing(10);
    pickerButtons->addStretch();
    auto *btnClosePicker = UiFactory::ghostButton(tr("关闭"), &picker);
    auto *btnViewReport = UiFactory::primaryButton(tr("查看周报"), &picker);
    btnClosePicker->setMinimumWidth(96);
    btnViewReport->setMinimumWidth(128);
    pickerButtons->addWidget(btnClosePicker);
    pickerButtons->addWidget(btnViewReport);
    pickerLayout->addLayout(pickerButtons);

    connect(btnClosePicker, &QPushButton::clicked, &picker, &QDialog::reject);
    connect(btnViewReport, &QPushButton::clicked, &picker, &QDialog::accept);
    connect(reportList, &QListWidget::itemDoubleClicked, &picker, [&picker]() { picker.accept(); });

    // Center on screen (layout is now fully built)
    {
#ifdef Q_OS_ANDROID
        const QRect avail = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
        picker.resize(picker.sizeHint().expandedTo(picker.minimumSize()).boundedTo(avail.size()));
        LifeBalanceAI::Ui::PlatformLayoutPolicy::centerWidgetOnSafeArea(&picker);
#else
        const QRect avail = QGuiApplication::primaryScreen()->availableGeometry();
        picker.resize(picker.sizeHint());
        picker.move(avail.center() - picker.rect().center());
#endif
    }

    if (picker.exec() != QDialog::Accepted)
        return;
































    // Find the selected report



    int selIdx = reportList->currentRow();



    if (selIdx < 0 || selIdx >= reports.size()) return;







    const auto &report = reports[selIdx];







    // Show summary



    QString summaryText;



    const auto parsedSummary =
        LifeBalanceAI::Services::AiResponseParser::parseReportSummary(report.aiSummary);

    if (parsedSummary.ok) {



        QJsonObject ai = parsedSummary.summaryObject;



        summaryText = tr("【总体评价】\n%1\n\n").arg(ai[QStringLiteral("summary")].toString());



        summaryText += tr("饮食评分: %1/10 | 运动评分: %2/10 | 完成率: %3%\n")



            .arg(ai[QStringLiteral("diet_score")].toInt())



            .arg(ai[QStringLiteral("exercise_score")].toInt())



            .arg(ai[QStringLiteral("completion_rate")].toInt());



        summaryText += tr("下周目标: %1").arg(ai[QStringLiteral("next_week_goal")].toString());



    } else {



        summaryText = report.aiSummary;



    }

    if (parsedSummary.ok) {
        const QJsonObject ai = parsedSummary.summaryObject;
        summaryText = QString::fromUtf8("总体评价\n%1\n\n")
                          .arg(ai.value(QStringLiteral("summary")).toString());
        summaryText += QString::fromUtf8("饮食评分：%1/10\n运动评分：%2/10\n完成率：%3%\n")
                           .arg(ai.value(QStringLiteral("diet_score")).toInt())
                           .arg(ai.value(QStringLiteral("exercise_score")).toInt())
                           .arg(ai.value(QStringLiteral("completion_rate")).toInt());
        summaryText += QString::fromUtf8("\n下周目标：%1")
                           .arg(ai.value(QStringLiteral("next_week_goal")).toString());
    }

    QDialog detail(this);
    detail.setObjectName(QStringLiteral("reportHistoryDialog"));
    detail.setWindowTitle(tr("报告详情"));
    detail.setAttribute(Qt::WA_StyledBackground, true);
    detail.setStyleSheet(QStringLiteral(
        "#reportHistoryDialog{"
        "  background:#FFFDF9;"
        "  border:1px solid #E8DED2;"
        "  border-radius:16px;"
        "}"
    ));
#ifdef Q_OS_ANDROID
    {
        const QRect detailAvailable = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
        detail.setMinimumSize(qMax(300, qMin(detailAvailable.width(), 460)),
                              qMax(420, qMin(detailAvailable.height(), 560)));
    }
#else
    detail.setMinimumSize(460, 520);
#endif

    auto *detailLayout = new QVBoxLayout(&detail);
    detailLayout->setContentsMargins(18, 16, 18, 16);
    detailLayout->setSpacing(10);

    auto *detailTitle = new QLabel(tr("报告详情"), &detail);
    detailTitle->setObjectName(QStringLiteral("reportHistoryTitle"));
    detailLayout->addWidget(detailTitle);

    auto *detailMeta = new QLabel(lines.value(selIdx), &detail);
    detailMeta->setObjectName(QStringLiteral("reportHistoryHint"));
    detailLayout->addWidget(detailMeta);

    auto *detailText = new QTextEdit(&detail);
    detailText->setObjectName(QStringLiteral("reportDetailText"));
    detailText->setReadOnly(true);
    detailText->setTextInteractionFlags(Qt::NoTextInteraction);
    detailText->setPlainText(summaryText);
    detailText->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    detailText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    detailLayout->addWidget(detailText, 1);

    int action = 0;
    auto *detailButtons = new QHBoxLayout();
    detailButtons->setSpacing(10);
    detailButtons->addStretch();
    auto *btnCloseDetail = UiFactory::ghostButton(tr("关闭"), &detail);
    auto *btnExportDetail = UiFactory::primaryButton(tr("导出 PNG"), &detail);
    btnCloseDetail->setMinimumWidth(96);
    btnExportDetail->setMinimumWidth(128);
    detailButtons->addWidget(btnCloseDetail);
    detailButtons->addWidget(btnExportDetail);
    detailLayout->addLayout(detailButtons);

    connect(btnCloseDetail, &QPushButton::clicked, &detail, &QDialog::reject);
    connect(btnExportDetail, &QPushButton::clicked, &detail, [&detail, &action]() {
        action = 1;
        detail.accept();
    });

    // Center on screen (layout is now fully built)
    {
#ifdef Q_OS_ANDROID
        const QRect avail = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
        detail.resize(detail.sizeHint().expandedTo(detail.minimumSize()).boundedTo(avail.size()));
        LifeBalanceAI::Ui::PlatformLayoutPolicy::centerWidgetOnSafeArea(&detail);
#else
        const QRect avail = QGuiApplication::primaryScreen()->availableGeometry();
        detail.resize(detail.sizeHint());
        detail.move(avail.center() - detail.rect().center());
#endif
    }

    if (detail.exec() == QDialog::Accepted && action == 1)
        onExportReport(report.rid, QStringLiteral("png"));



}











// ------------------------------------------------------------------------



// Analysis Page (setup dynamically)



// ------------------------------------------------------------------------



int MainWindow::setupAnalysisPage()



{



    QWidget *existing = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_analysis"));



    if (existing) {



        QLayout *lay = existing->layout();



        if (lay) {



            QLayoutItem *item;



            while ((item = lay->takeAt(0)) != nullptr) {



                delete item->widget();



                delete item;



            }



        }



    } else {



        existing = new QWidget();



        existing->setObjectName(QStringLiteral("page_analysis"));



        auto *vlay = new QVBoxLayout(existing);



        vlay->setContentsMargins(24, 20, 24, 20);



        vlay->setSpacing(16);



        ui->stackedWidget->addWidget(existing);

    }



    m_analysisPageIndex = ui->stackedWidget->indexOf(existing);







    auto *rootLayout = qobject_cast<QVBoxLayout *>(existing->layout());
    if (!rootLayout) return -1;
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *pageScroll = new QScrollArea(existing);
    pageScroll->setObjectName(QStringLiteral("analysisPageScrollArea"));
    pageScroll->setWidgetResizable(true);
    pageScroll->setFrameShape(QFrame::NoFrame);
    pageScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto *scrollContent = new QWidget(pageScroll);
    scrollContent->setObjectName(QStringLiteral("analysisPageScrollContent"));
    scrollContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *vlay = new QVBoxLayout(scrollContent);

#ifdef Q_OS_ANDROID
    vlay->setContentsMargins(18, 8, 18, 16);
    vlay->setSpacing(10);
#else
    vlay->setContentsMargins(24, 24, 24, 20);
    vlay->setSpacing(16);
#endif
    vlay->setAlignment(Qt::AlignTop);
    pageScroll->setWidget(scrollContent);
    rootLayout->addWidget(pageScroll, 1);
    const int analysisHeroHeight =
#ifdef Q_OS_ANDROID
        126;
#else
        188;
#endif
    auto *analysisHero = UiFactory::assetLabel(QStringLiteral(":/assets/ai_analysis.png"), analysisHeroHeight, existing);
    analysisHero->setMinimumHeight(analysisHeroHeight);
    analysisHero->setMaximumHeight(analysisHeroHeight + 8);
    vlay->addWidget(analysisHero);



    QString role = DatabaseManager::instance().getUserRole(m_currentUserId);







    if (role == QStringLiteral("Ascendant")) {



        auto *titleLabel = new QLabel(QString::fromUtf8("AI 健康洞察"), existing);
        titleLabel->setObjectName(QStringLiteral("analysisPageTitle"));
        titleLabel->setAlignment(Qt::AlignCenter);
        vlay->addWidget(titleLabel);

        auto *subtitleLabel = new QLabel(QString::fromUtf8("基于近期饮食、运动和反馈记录，整理更适合手机阅读的习惯建议。"), existing);
        subtitleLabel->setObjectName(QStringLiteral("analysisPageSubtitle"));
        subtitleLabel->setAlignment(Qt::AlignCenter);
        subtitleLabel->setWordWrap(true);
        vlay->addWidget(subtitleLabel);







#if 0
        auto *summaryCard = UiFactory::infoCard(
            tr("AI 会基于近几天的饮食、运动和反馈记录，给出更具体的健康洞察与建议。"),
            tr("适合在完成一段时间打卡后查看，可以更快发现饮食结构、运动节奏和执行习惯里的变化。"),
            existing);
        vlay->addWidget(summaryCard);
#endif

        auto *summaryCard = new QFrame(existing);
        summaryCard->setObjectName(QStringLiteral("analysisIntroCard"));
        auto *summaryLayout = new QVBoxLayout(summaryCard);
#ifdef Q_OS_ANDROID
        summaryLayout->setContentsMargins(14, 12, 14, 12);
        summaryLayout->setSpacing(6);
#else
        summaryLayout->setContentsMargins(18, 16, 18, 16);
        summaryLayout->setSpacing(8);
#endif
        auto *summaryTitle = new QLabel(QString::fromUtf8("查看近期健康习惯的变化"), summaryCard);
        summaryTitle->setObjectName(QStringLiteral("analysisIntroTitle"));
        summaryTitle->setWordWrap(true);
        summaryLayout->addWidget(summaryTitle);

        auto *summaryBody = new QLabel(QString::fromUtf8("AI 会从饮食结构、运动节奏和执行反馈中提炼观察，帮助你发现可以继续坚持或轻微调整的地方。"), summaryCard);
        summaryBody->setObjectName(QStringLiteral("analysisIntroBody"));
        summaryBody->setWordWrap(true);
        summaryLayout->addWidget(summaryBody);

        auto *summaryNote = new QLabel(QString::fromUtf8("内容仅用于习惯管理参考，不作为医疗诊断。"), summaryCard);
        summaryNote->setObjectName(QStringLiteral("analysisIntroNote"));
        summaryNote->setWordWrap(true);
        summaryLayout->addWidget(summaryNote);
        vlay->addWidget(summaryCard);







        auto *btnAnalyze = UiFactory::primaryButton(QString::fromUtf8("开始分析"), existing);



        connect(btnAnalyze, &QPushButton::clicked, this, &MainWindow::onDeepAnalysisTriggered);



        vlay->addWidget(btnAnalyze);

        auto *analysisStatus = createInlineStatusLabel(QStringLiteral("analysisStatusLabel"), existing);
        vlay->addWidget(analysisStatus);







#ifdef Q_OS_ANDROID
        vlay->addSpacing(LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavPagePadding());
#else
        vlay->addStretch();
#endif



    } else {



        auto *titleLabel = new QLabel(QString::fromUtf8("AI 健康洞察"), existing);
        titleLabel->setObjectName(QStringLiteral("analysisPageTitle"));
        titleLabel->setAlignment(Qt::AlignCenter);
        vlay->addWidget(titleLabel);

        auto *subtitleLabel = new QLabel(QString::fromUtf8("升级后可查看更完整的饮食、运动和执行反馈洞察。"), existing);
        subtitleLabel->setObjectName(QStringLiteral("analysisPageSubtitle"));
        subtitleLabel->setAlignment(Qt::AlignCenter);
        subtitleLabel->setWordWrap(true);
        vlay->addWidget(subtitleLabel);







#if 0
        auto *benefitCard = UiFactory::infoCard(
            tr("升级后可获得"),
            tr("更细致的饮食结构分析、运动节奏判断，以及更有针对性的健康改进建议。"),
            existing);
        vlay->addWidget(benefitCard);

        auto *descLabel = new QLabel(tr(



            "\u6210\u4e3a\u5f8b\u884c\u8005\uff0c\u5373\u523b\u89e3\u9501 AI \u6df1\u5ea6\u5206\u6790\u80fd\u529b\uff01\n\n"



            "\u6df1\u5ea6\u5206\u6790\u53ef\u4ee5\uff1a\n"



            "\u25cf  \u7cbe\u51c6\u8bc4\u4f30\u4f60\u7684\u996e\u98df\u7ed3\u6784\u4e0e\u8425\u517b\u5747\u8861\n"



            "\u25cf  \u667a\u80fd\u8ffd\u8e2a\u8fd0\u52a8\u6a21\u5f0f\u4e0e\u4f53\u80fd\u53d8\u5316\u8d8b\u52bf\n"



            "\u25cf  \u53d1\u73b0\u6f5c\u5728\u5065\u5eb7\u98ce\u9669\u5e76\u63d0\u4f9b\u6539\u5584\u5efa\u8bae\n"



        ), existing);



        descLabel->setObjectName(QStringLiteral("emptyBody"));
        descLabel->setWordWrap(true);
        descLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);



        vlay->addWidget(descLabel);







        vlay->addSpacing(10);
#endif

        auto *benefitCard = new QFrame(existing);
        benefitCard->setObjectName(QStringLiteral("analysisIntroCard"));
        auto *benefitLayout = new QVBoxLayout(benefitCard);
#ifdef Q_OS_ANDROID
        benefitLayout->setContentsMargins(14, 12, 14, 12);
        benefitLayout->setSpacing(6);
#else
        benefitLayout->setContentsMargins(18, 16, 18, 16);
        benefitLayout->setSpacing(8);
#endif
        auto *benefitTitle = new QLabel(QString::fromUtf8("律行者专属洞察"), benefitCard);
        benefitTitle->setObjectName(QStringLiteral("analysisIntroTitle"));
        benefitTitle->setWordWrap(true);
        benefitLayout->addWidget(benefitTitle);

        auto *benefitBody = new QLabel(QString::fromUtf8("可获得饮食结构观察、运动节奏建议和更细致的执行习惯反馈。建议仅用于日常习惯管理参考。"), benefitCard);
        benefitBody->setObjectName(QStringLiteral("analysisIntroBody"));
        benefitBody->setWordWrap(true);
        benefitLayout->addWidget(benefitBody);

        auto *benefitList = new QLabel(QString::fromUtf8("• 识别近期营养摄入倾向\n• 观察运动完成节奏\n• 提供温和、可执行的调整方向"), benefitCard);
        benefitList->setObjectName(QStringLiteral("analysisIntroNote"));
        benefitList->setWordWrap(true);
        benefitLayout->addWidget(benefitList);
        vlay->addWidget(benefitCard);







        auto *btnUpgrade = UiFactory::primaryButton(QString::fromUtf8("升级为律行者"), existing);



        connect(btnUpgrade, &QPushButton::clicked, this, [this]() {



            m_prePaymentPageIndex = ui->stackedWidget->currentIndex();



            navigateTo(AppRoute::Payment, false);



        });



        vlay->addWidget(btnUpgrade);







#ifdef Q_OS_ANDROID
        vlay->addSpacing(LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavPagePadding());
#else
        vlay->addStretch();
#endif



    }



    UiFactory::applyWarmPolish(existing);
    installKineticVerticalScroll(existing);
    relaxAndroidWidthConstraints(existing);
    syncShellRouteIndexes();
    return m_analysisPageIndex;



}







// ------------------------------------------------------------------------



// Report Page (setup dynamically)



// ------------------------------------------------------------------------



int MainWindow::setupReportPage()



{



    QWidget *existing = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_reports"));



    if (existing) {



        QLayout *lay = existing->layout();



        if (lay) {



            QLayoutItem *item;



            while ((item = lay->takeAt(0)) != nullptr) {



                delete item->widget();



                delete item;



            }



        }



    } else {



        existing = new QWidget();



        existing->setObjectName(QStringLiteral("page_reports"));



        auto *vlay = new QVBoxLayout(existing);



        vlay->setContentsMargins(24, 20, 24, 20);



        vlay->setSpacing(16);



        ui->stackedWidget->addWidget(existing);

    }



    m_reportPageIndex = ui->stackedWidget->indexOf(existing);







    auto *rootLayout = qobject_cast<QVBoxLayout *>(existing->layout());
    if (!rootLayout) return -1;
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    auto *pageScroll = new QScrollArea(existing);
    pageScroll->setObjectName(QStringLiteral("reportPageScrollArea"));
    pageScroll->setWidgetResizable(true);
    pageScroll->setFrameShape(QFrame::NoFrame);
    pageScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    auto *scrollContent = new QWidget(pageScroll);
    scrollContent->setObjectName(QStringLiteral("reportPageScrollContent"));
    scrollContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *vlay = new QVBoxLayout(scrollContent);

#ifdef Q_OS_ANDROID
    vlay->setContentsMargins(18, 8, 18, 16);
    vlay->setSpacing(10);
#else
    vlay->setContentsMargins(24, 24, 24, 20);
    vlay->setSpacing(16);
#endif
    vlay->setAlignment(Qt::AlignTop);
    pageScroll->setWidget(scrollContent);
    rootLayout->addWidget(pageScroll, 1);
    const int reportHeroHeight =
#ifdef Q_OS_ANDROID
        126;
#else
        188;
#endif
    auto *reportHero = UiFactory::assetLabel(QStringLiteral(":/assets/empty_report.png"), reportHeroHeight, existing);
    reportHero->setMinimumHeight(reportHeroHeight);
    reportHero->setMaximumHeight(reportHeroHeight + 8);
    vlay->addWidget(reportHero);



    auto *titleLabel = new QLabel(tr("\u5065\u5eb7\u5468\u62a5"), existing);



    titleLabel->setObjectName(QStringLiteral("reportPageTitle"));



    titleLabel->setAlignment(Qt::AlignCenter);



    vlay->addWidget(titleLabel);







#if 0
    auto *reportSummary = UiFactory::infoCard(
        tr("把近期打卡记录整理成更完整的周视角总结。"),
        tr("周报会汇总饮食、运动和完成率变化，也可以继续导出为 PNG 图片保存或分享。"),
        existing);
    vlay->addWidget(reportSummary);
#endif

    auto *reportSummary = new QFrame(existing);
    reportSummary->setObjectName(QStringLiteral("reportIntroCard"));
    auto *summaryLayout = new QVBoxLayout(reportSummary);
#ifdef Q_OS_ANDROID
    summaryLayout->setContentsMargins(14, 12, 14, 12);
    summaryLayout->setSpacing(6);
#else
    summaryLayout->setContentsMargins(18, 16, 18, 16);
    summaryLayout->setSpacing(8);
#endif
    auto *summaryTitle = new QLabel(QString::fromUtf8("把近期打卡记录整理成周总结"), reportSummary);
    summaryTitle->setObjectName(QStringLiteral("reportIntroTitle"));
    summaryTitle->setWordWrap(true);
    summaryLayout->addWidget(summaryTitle);

    auto *summaryBody = new QLabel(QString::fromUtf8("周报会汇总饮食、运动和完成率变化，也可以导出为 PNG 图片保存或分享。"), reportSummary);
    summaryBody->setObjectName(QStringLiteral("reportIntroBody"));
    summaryBody->setWordWrap(true);
    summaryLayout->addWidget(summaryBody);

    auto *summaryNote = new QLabel(QString::fromUtf8("内容仅用于习惯管理参考，不作为医疗诊断。"), reportSummary);
    summaryNote->setObjectName(QStringLiteral("reportIntroNote"));
    summaryNote->setWordWrap(true);
    summaryLayout->addWidget(summaryNote);
    vlay->addWidget(reportSummary);

    auto *metricsCard = new QFrame(existing);
    metricsCard->setObjectName(QStringLiteral("reportMetricsCard"));
    UiFactory::setClass(metricsCard, QStringLiteral("warmCard"));
    metricsCard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    auto *metricsLayout = new QVBoxLayout(metricsCard);
#ifdef Q_OS_ANDROID
    metricsLayout->setContentsMargins(10, 10, 10, 10);
    metricsLayout->setSpacing(7);
#else
    metricsLayout->setContentsMargins(16, 16, 16, 16);
    metricsLayout->setSpacing(12);
#endif

    auto *metricsTitle = new QLabel(tr("打卡数据概览"), metricsCard);
    metricsTitle->setObjectName(QStringLiteral("emptyTitle"));
    metricsLayout->addWidget(metricsTitle);

    QMap<QDate, QPair<int, int>> sevenDayStats;
    const QDate today = QDate::currentDate();
    for (int offset = 6; offset >= 0; --offset)
        sevenDayStats.insert(today.addDays(-offset), qMakePair(0, 0));

    QSqlDatabase &db = DatabaseManager::instance().database();
    QSqlQuery statsQuery(db);
    statsQuery.prepare(QStringLiteral(
        "SELECT di.date, COUNT(*), "
        "SUM(CASE WHEN di.is_done = 1 THEN 1 ELSE 0 END) "
        "FROM Daily_Items di "
        "JOIN Plans p ON di.plan_id = p.id "
        "WHERE p.user_id = :uid AND di.date BETWEEN :start AND :end "
        "GROUP BY di.date"));
    statsQuery.bindValue(QStringLiteral(":uid"), m_currentUserId);
    statsQuery.bindValue(QStringLiteral(":start"), today.addDays(-6).toString(QStringLiteral("yyyy-MM-dd")));
    statsQuery.bindValue(QStringLiteral(":end"), today.toString(QStringLiteral("yyyy-MM-dd")));
    if (statsQuery.exec()) {
        while (statsQuery.next()) {
            const QDate date = QDate::fromString(statsQuery.value(0).toString(), QStringLiteral("yyyy-MM-dd"));
            if (sevenDayStats.contains(date))
                sevenDayStats[date] = qMakePair(statsQuery.value(1).toInt(), statsQuery.value(2).toInt());
        }
    }

    QVector<qreal> rates;
    QVector<QString> labels;
    int totalSlots = 0;
    int doneSlots = 0;
    for (auto it = sevenDayStats.cbegin(); it != sevenDayStats.cend(); ++it) {
        const int total = it.value().first;
        const int done = it.value().second;
        totalSlots += total;
        doneSlots += done;
        rates.append(total > 0 ? (done * 100.0 / total) : 0.0);
        labels.append(it.key().toString(QStringLiteral("M/d")));
    }
    const int completionRate = totalSlots > 0 ? qRound(doneSlots * 100.0 / totalSlots) : 0;

    auto *progress = new CircularProgressBar(metricsCard);
    progress->setObjectName(QStringLiteral("weeklyCompletionProgress"));
    progress->setLineWidth(10);
    progress->setColors(DesignTokens::primaryLightest(), DesignTokens::primary());
    progress->setText(QStringLiteral("%1%").arg(completionRate));
    progress->setValue(completionRate);

    auto *trend = new TrendLineChart(metricsCard);
    trend->setObjectName(QStringLiteral("weeklyTrendChart"));
    trend->setTitle(tr("近 7 日完成率"));
    trend->setYAxisLabel(tr("完成率"));
    trend->setColors(DesignTokens::primary(), DesignTokens::primaryLightest(), DesignTokens::borderLight());
    trend->setData(rates, labels);
    trend->setMinimumWidth(0);
    trend->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto *completionCard = new QFrame(metricsCard);
    completionCard->setObjectName(QStringLiteral("reportMetricModule"));
    auto *completionLayout = new QVBoxLayout(completionCard);
#ifdef Q_OS_ANDROID
    completionLayout->setContentsMargins(12, 10, 12, 12);
    completionLayout->setSpacing(6);
#else
    completionLayout->setContentsMargins(14, 12, 14, 14);
    completionLayout->setSpacing(8);
#endif
    auto *completionTitle = new QLabel(QString::fromUtf8("完成率概览"), completionCard);
    completionTitle->setObjectName(QStringLiteral("reportMetricModuleTitle"));
    completionLayout->addWidget(completionTitle);

    auto *progressWrap = new QWidget(completionCard);
    auto *progressLayout = new QHBoxLayout(progressWrap);
    progressLayout->setContentsMargins(0, 0, 0, 0);
    progressLayout->addStretch();
#ifdef Q_OS_ANDROID
    progress->setMaximumSize(84, 84);
#else
    progress->setMaximumSize(116, 116);
#endif
    progressLayout->addWidget(progress);
    progressLayout->addStretch();
    completionLayout->addWidget(progressWrap);

    auto *completionHint = new QLabel(QString::fromUtf8("近 7 日完成 %1 / %2 项").arg(doneSlots).arg(totalSlots), completionCard);
    completionHint->setObjectName(QStringLiteral("reportMetricHint"));
    completionHint->setAlignment(Qt::AlignCenter);
    completionLayout->addWidget(completionHint);
    metricsLayout->addWidget(completionCard);

    auto *trendCard = new QFrame(metricsCard);
    trendCard->setObjectName(QStringLiteral("reportMetricModule"));
    auto *trendLayout = new QVBoxLayout(trendCard);
#ifdef Q_OS_ANDROID
    trendLayout->setContentsMargins(12, 10, 12, 10);
    trendLayout->setSpacing(6);
#else
    trendLayout->setContentsMargins(14, 12, 14, 12);
    trendLayout->setSpacing(8);
#endif
    auto *trendTitle = new QLabel(QString::fromUtf8("近 7 日完成趋势"), trendCard);
    trendTitle->setObjectName(QStringLiteral("reportMetricModuleTitle"));
    trendLayout->addWidget(trendTitle);
#ifdef Q_OS_ANDROID
    trend->setMinimumHeight(110);
    trend->setMaximumHeight(124);
#else
    trend->setMinimumHeight(150);
#endif
    trendLayout->addWidget(trend);
    metricsLayout->addWidget(trendCard);

    QMap<QDate, CalendarGridView::CheckInStatus> monthStatus;
    const QDate monthStart(today.year(), today.month(), 1);
    const QDate monthEnd(today.year(), today.month(), today.daysInMonth());
    QSqlQuery monthQuery(db);
    monthQuery.prepare(QStringLiteral(
        "SELECT di.date, COUNT(*), "
        "SUM(CASE WHEN di.is_done = 1 THEN 1 ELSE 0 END) "
        "FROM Daily_Items di "
        "JOIN Plans p ON di.plan_id = p.id "
        "WHERE p.user_id = :uid AND di.date BETWEEN :start AND :end "
        "GROUP BY di.date"));
    monthQuery.bindValue(QStringLiteral(":uid"), m_currentUserId);
    monthQuery.bindValue(QStringLiteral(":start"), monthStart.toString(QStringLiteral("yyyy-MM-dd")));
    monthQuery.bindValue(QStringLiteral(":end"), monthEnd.toString(QStringLiteral("yyyy-MM-dd")));
    if (monthQuery.exec()) {
        while (monthQuery.next()) {
            const QDate date = QDate::fromString(monthQuery.value(0).toString(), QStringLiteral("yyyy-MM-dd"));
            const int total = monthQuery.value(1).toInt();
            const int done = monthQuery.value(2).toInt();
            CalendarGridView::CheckInStatus status = CalendarGridView::NoData;
            if (total > 0)
                status = (done == total) ? CalendarGridView::Excellent
                       : (done > 0) ? CalendarGridView::Complete
                                    : CalendarGridView::Incomplete;
            monthStatus.insert(date, status);
        }
    }

    auto *calendar = new CalendarGridView(metricsCard);
    calendar->setObjectName(QStringLiteral("monthlyCalendarGrid"));
    calendar->setTitle(tr("本月打卡"));
    calendar->setMonth(today.month(), today.year());
    calendar->setCheckInData(monthStatus);
    calendar->setMinimumWidth(0);
#ifdef Q_OS_ANDROID
    calendar->setFixedHeight(224);
#else
    calendar->setFixedHeight(350);
#endif
    auto *calendarCard = new QFrame(metricsCard);
    calendarCard->setObjectName(QStringLiteral("reportMetricModule"));
    auto *calendarLayout = new QVBoxLayout(calendarCard);
#ifdef Q_OS_ANDROID
    calendarLayout->setContentsMargins(12, 10, 12, 10);
    calendarLayout->setSpacing(6);
#else
    calendarLayout->setContentsMargins(14, 12, 14, 12);
    calendarLayout->setSpacing(8);
#endif
    auto *calendarTitle = new QLabel(QString::fromUtf8("本月打卡日历"), calendarCard);
    calendarTitle->setObjectName(QStringLiteral("reportMetricModuleTitle"));
    calendarLayout->addWidget(calendarTitle);
    calendarLayout->addWidget(calendar);
    metricsLayout->addWidget(calendarCard);

    vlay->addWidget(metricsCard);

    auto *btnGenerate = UiFactory::primaryButton(QString::fromUtf8("生成周报"), existing);



    connect(btnGenerate, &QPushButton::clicked, this, [this]() {
        onGenerateReport();
    });



    vlay->addWidget(btnGenerate);

    auto *reportStatus = createInlineStatusLabel(QStringLiteral("reportStatusLabel"), existing);
    vlay->addWidget(reportStatus);







    auto *btnHistory = UiFactory::secondaryButton(QString::fromUtf8("我的周报"), existing);



    connect(btnHistory, &QPushButton::clicked, this, [this]() {



        auto reports = DatabaseManager::instance().getReportHistory(m_currentUserId);



        if (reports.isEmpty()) {



            AnimatedDialog::info(this, tr("\u6211\u7684\u5468\u62a5"),



                tr("\u4f60\u8fd8\u6ca1\u6709\u751f\u6210\u8fc7\u5468\u62a5\u54e6\uff01\u8bf7\u5148\u751f\u6210\u5468\u62a5"));



        } else {



            showReportHistory();



        }



    });



    vlay->addWidget(btnHistory);







#ifdef Q_OS_ANDROID
    vlay->addSpacing(LifeBalanceAI::Ui::PlatformLayoutPolicy::bottomNavPagePadding());
#else
    vlay->addStretch();
#endif



    UiFactory::applyWarmPolish(existing);
    installKineticVerticalScroll(existing);
    relaxAndroidWidthConstraints(existing);
    syncShellRouteIndexes();
    return m_reportPageIndex;



}







void MainWindow::onGenerateReport()
{
    auto showReportNotice = [this](const QString &title, const QString &message) {
        showReportStatus(message);

        QDialog dialog(this);
        dialog.setObjectName(QStringLiteral("reportHistoryDialog"));
        dialog.setWindowTitle(title);
        dialog.setAttribute(Qt::WA_StyledBackground, true);
        dialog.setStyleSheet(QStringLiteral(
            "#reportHistoryDialog{"
            "  background:#FFFDF9;"
            "  border:1px solid #E8DED2;"
            "  border-radius:16px;"
            "}"
        ));
#ifdef Q_OS_ANDROID
        dialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
#endif

        auto *layout = new QVBoxLayout(&dialog);
        layout->setContentsMargins(18, 16, 18, 16);
        layout->setSpacing(10);

        auto *titleLabel = new QLabel(title, &dialog);
        titleLabel->setObjectName(QStringLiteral("reportHistoryTitle"));
        titleLabel->setWordWrap(true);
        layout->addWidget(titleLabel);

        auto *messageLabel = new QLabel(message, &dialog);
        messageLabel->setObjectName(QStringLiteral("reportIntroBody"));
        messageLabel->setWordWrap(true);
        layout->addWidget(messageLabel);

        auto *buttonRow = new QHBoxLayout();
        buttonRow->addStretch();
        auto *btnOk = UiFactory::primaryButton(QString::fromUtf8("确定"), &dialog);
        btnOk->setMinimumWidth(112);
        buttonRow->addWidget(btnOk);
        layout->addLayout(buttonRow);

        connect(btnOk, &QPushButton::clicked, &dialog, &QDialog::accept);

#ifdef Q_OS_ANDROID
        {
            const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
            dialog.resize(qMin(available.width(), qMax(300, int(available.width() * 0.82))),
                          qMin(available.height(), 260));
            LifeBalanceAI::Ui::PlatformLayoutPolicy::centerWidgetOnSafeArea(&dialog);
        }
#endif

        dialog.exec();
    };

    if (m_currentUserId < 0) {
        showReportNotice(uiText("\u63d0\u793a"),
                         uiText("\u672a\u68c0\u6d4b\u5230\u767b\u5f55\u7528\u6237\uff0c\u8bf7\u91cd\u65b0\u767b\u5f55\u540e\u518d\u8bd5\u3002"));
        return;
    }

    if (!m_reportService) {
        showReportNotice(uiText("\u670d\u52a1\u4e0d\u53ef\u7528"),
                         uiText("\u5468\u62a5\u670d\u52a1\u672a\u521d\u59cb\u5316\uff0c\u8bf7\u91cd\u542f\u5e94\u7528\u540e\u518d\u8bd5\u3002"));
        return;
    }

    if (DatabaseManager::instance().getLatestPlanId(m_currentUserId) < 0) {
        showReportNotice(uiText("\u6682\u65e0\u8ba1\u5212"),
                         uiText("\u6682\u65e0\u53ef\u7528\u8ba1\u5212\u6570\u636e\uff0c\u9700\u8981\u5148\u751f\u6210\u5065\u5eb7\u8ba1\u5212\u540e\u518d\u751f\u6210\u5468\u62a5\u3002"));
        return;
    }

    const auto reports = DatabaseManager::instance().getReportHistory(m_currentUserId);
    const QString todayStr = QDate::currentDate().toString(QStringLiteral("yyyy-MM-dd"));
    for (const auto &report : reports) {
        if (report.type != QStringLiteral("deep_analysis") && report.createdAt.startsWith(todayStr)) {
            showReportNotice(uiText("\u4eca\u65e5\u5df2\u751f\u6210"),
                             uiText("\u4eca\u5929\u5df2\u7ecf\u751f\u6210\u8fc7\u5468\u62a5\uff0c\u660e\u5929\u518d\u6765\u770b\u65b0\u7684\u603b\u7ed3\u5427\u3002"));
            return;
        }
    }

    const LifeBalanceAI::Models::UserInfo info =
        DatabaseManager::instance().getUserInfo(m_currentUserId);
    if (info.role != QStringLiteral("Ascendant") && info.streakDays < 30) {
        showReportNotice(uiText("\u6761\u4ef6\u4e0d\u8db3"),
                         uiText("\u63a2\u7d22\u8005\u9700\u8fde\u7eed\u6253\u5361 30 \u5929\u624d\u80fd\u751f\u6210\u5468\u62a5\u3002\u4f60\u5f53\u524d\u8fde\u7eed\u6253\u5361 %1 \u5929\uff1b\u5347\u7ea7\u5f8b\u884c\u8005\u53ef\u89e3\u9501\u6bcf\u5468\u62a5\u544a\u3002").arg(info.streakDays));
        return;
    }

    if (!AIManager::hasChatApiKey()) {
        showReportNotice(uiText("\u0041\u0049 \u672a\u914d\u7f6e"),
                         uiText("\u0041\u0049 \u670d\u52a1\u672a\u914d\u7f6e\uff0c\u6682\u65f6\u65e0\u6cd5\u751f\u6210\u5468\u62a5\u3002"));
        return;
    }

    if (!m_reportService->canGenerateReport(m_currentUserId)) {
        showReportNotice(uiText("\u6682\u65f6\u4e0d\u53ef\u7528"),
                         uiText("\u6682\u65f6\u65e0\u6cd5\u751f\u6210\u5468\u62a5\uff0c\u8bf7\u68c0\u67e5\u8ba1\u5212\u548c\u6253\u5361\u6570\u636e\u540e\u91cd\u8bd5\u3002"));
        return;
    }

    showReportStatus(uiText("\u6b63\u5728\u751f\u6210\u5065\u5eb7\u5468\u62a5\uff0c\u8bf7\u7a0d\u7b49..."));
    showLoadingBar(uiText("\u6b63\u5728\u751f\u6210\u5065\u5eb7\u5468\u62a5"));
    m_reportService->generateReport(m_currentUserId);
}


void MainWindow::onExportReport(int reportId, const QString &format)



{



    if (m_currentUserId > 0) {
        LifeBalanceAI::Models::ReportData report =
            DatabaseManager::instance().getReportById(reportId);
        QString role = DatabaseManager::instance().getUserRole(m_currentUserId);

        if (report.rid > 0 && report.uid != m_currentUserId && role != QStringLiteral("admin")) {
            AnimatedDialog::info(this, tr("提示"), tr("你没有权限导出这份报告。"));
            return;
        }
    }



    if (m_reportService) {
        showLoadingBar(tr("正在导出 PNG 图片"));
        m_reportService->exportReport(reportId, format);
    }



}







void MainWindow::onReportReady(int userId, const LifeBalanceAI::Models::ReportData &report)



{



    Q_UNUSED(userId);



    hideLoadingBar();
    showReportStatus(uiText("\u5065\u5eb7\u5468\u62a5\u5df2\u751f\u6210\u3002"));







    // Parse AI summary JSON for display



    QString summaryText;



    const auto parsedSummary =
        LifeBalanceAI::Services::AiResponseParser::parseReportSummary(report.aiSummary);

    if (parsedSummary.ok) {



        QJsonObject ai = parsedSummary.summaryObject;



        summaryText = tr("【总体评价】\n%1\n\n").arg(ai[QStringLiteral("summary")].toString());



        summaryText += tr("饮食评分: %1/10 | 运动评分: %2/10 | 完成率: %3%\n")



            .arg(ai[QStringLiteral("diet_score")].toInt())



            .arg(ai[QStringLiteral("exercise_score")].toInt())



            .arg(ai[QStringLiteral("completion_rate")].toInt());



        summaryText += tr("\n下周目标: %1").arg(ai[QStringLiteral("next_week_goal")].toString());



    } else {



        summaryText = report.aiSummary;



    }







    if (parsedSummary.ok) {
        const QJsonObject ai = parsedSummary.summaryObject;
        summaryText = QString::fromUtf8("总体评价\n%1\n\n").arg(ai[QStringLiteral("summary")].toString());
        summaryText += QString::fromUtf8("饮食评分：%1/10\n运动评分：%2/10\n完成率：%3%\n")
                           .arg(ai[QStringLiteral("diet_score")].toInt())
                           .arg(ai[QStringLiteral("exercise_score")].toInt())
                           .arg(ai[QStringLiteral("completion_rate")].toInt());
        summaryText += QString::fromUtf8("\n下周目标：%1").arg(ai[QStringLiteral("next_week_goal")].toString());
    }

    QDialog reportReadyDialog(this);
    reportReadyDialog.setObjectName(QStringLiteral("reportHistoryDialog"));
    reportReadyDialog.setWindowTitle(QString::fromUtf8("报告已生成"));
    reportReadyDialog.setAttribute(Qt::WA_StyledBackground, true);
    reportReadyDialog.setStyleSheet(QStringLiteral(
        "#reportHistoryDialog{"
        "  background:#FFFDF9;"
        "  border:1px solid #E8DED2;"
        "  border-radius:16px;"
        "}"
    ));
#ifdef Q_OS_ANDROID
    reportReadyDialog.setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
#endif

    auto *readyLayout = new QVBoxLayout(&reportReadyDialog);
    readyLayout->setContentsMargins(18, 16, 18, 16);
    readyLayout->setSpacing(10);

    auto *readyTitle = new QLabel(QString::fromUtf8("报告已生成"), &reportReadyDialog);
    readyTitle->setObjectName(QStringLiteral("reportHistoryTitle"));
    readyLayout->addWidget(readyTitle);

    auto *readyHint = new QLabel(QString::fromUtf8("周报已保存到历史记录，你也可以导出为 PNG 图片。"), &reportReadyDialog);
    readyHint->setObjectName(QStringLiteral("reportHistoryHint"));
    readyHint->setWordWrap(true);
    readyLayout->addWidget(readyHint);

    auto *readySummary = new QTextEdit(&reportReadyDialog);
    readySummary->setObjectName(QStringLiteral("reportDetailText"));
    readySummary->setReadOnly(true);
    readySummary->setPlainText(summaryText);
    readySummary->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
#ifdef Q_OS_ANDROID
    readySummary->setMinimumHeight(150);
    readySummary->setMaximumHeight(250);
#else
    readySummary->setMinimumHeight(190);
#endif
    readyLayout->addWidget(readySummary, 1);

    auto *readyButtons = new QHBoxLayout();
    readyButtons->setSpacing(10);
    auto *btnExportReady = UiFactory::primaryButton(QString::fromUtf8("导出 PNG"), &reportReadyDialog);
    auto *btnConfirmReady = UiFactory::secondaryButton(QString::fromUtf8("确定"), &reportReadyDialog);
    readyButtons->addWidget(btnExportReady);
    readyButtons->addWidget(btnConfirmReady);
    readyLayout->addLayout(readyButtons);

    bool exportReady = false;
    connect(btnExportReady, &QPushButton::clicked, &reportReadyDialog, [&]() {
        exportReady = true;
        reportReadyDialog.accept();
    });
    connect(btnConfirmReady, &QPushButton::clicked, &reportReadyDialog, &QDialog::accept);

#ifdef Q_OS_ANDROID
    {
        const QRect available = LifeBalanceAI::Ui::PlatformLayoutPolicy::dialogAvailableRect();
        reportReadyDialog.resize(qMin(available.width(), qMax(320, int(available.width() * 0.88))),
                                 qMin(available.height(), 360));
        LifeBalanceAI::Ui::PlatformLayoutPolicy::centerWidgetOnSafeArea(&reportReadyDialog);
    }
#endif

    reportReadyDialog.exec();

    if (exportReady)
        onExportReport(report.rid, QStringLiteral("png"));



}







void MainWindow::onReportError(int userId, const QString &error)
{
    Q_UNUSED(userId);
    hideLoadingBar();
    qWarning() << "Report generation failed:" << error;

    const QString message = AIManager::hasChatApiKey()
        ? uiText("\u5468\u62a5\u6682\u65f6\u4e0d\u53ef\u7528\uff0c\u53ef\u7a0d\u540e\u91cd\u8bd5\u3002")
        : uiText("\u0041\u0049 \u670d\u52a1\u672a\u914d\u7f6e\uff0c\u6682\u65f6\u65e0\u6cd5\u751f\u6210\u5468\u62a5\u3002");
    showReportStatus(message);
}


// Admin Panel (page_admin)



// 鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲鈺愨晲







void MainWindow::loadAdminPage()



{



    QWidget *pageAdmin = ui->stackedWidget->findChild<QWidget *>(QStringLiteral("page_admin"));



    QTableWidget *adminTable = pageAdmin ? pageAdmin->findChild<QTableWidget *>(QStringLiteral("adminUserTable")) : nullptr;







    if (pageAdmin && adminTable) {



        // Refresh table data



        LifeBalanceAI::Services::UserService svc;



        QVector<LifeBalanceAI::Models::UserInfo> users = svc.getAllUsers();



        adminTable->setRowCount(users.size());



        for (int i = 0; i < users.size(); ++i) {



            const auto &u = users[i];



            adminTable->setItem(i, 0, new QTableWidgetItem(QString::number(u.uid)));



            adminTable->setItem(i, 1, new QTableWidgetItem(u.phone));



            adminTable->setItem(i, 2, new QTableWidgetItem(



                u.role == QStringLiteral("Ascendant") ? tr("律行者") :
                u.role == QStringLiteral("admin") ? tr("管理员") :
                tr("探索者")));



            adminTable->setItem(i, 3, new QTableWidgetItem(QString::number(u.streakDays)));



            adminTable->setItem(i, 4, new QTableWidgetItem(QString::number(u.freezeCards)));



            adminTable->setItem(i, 5, new QTableWidgetItem(QString::number(u.totalCheckins)));



            adminTable->setItem(i, 6, new QTableWidgetItem(u.createdAt.toString(QStringLiteral("yyyy-MM-dd"))));



        }



        adminTable->setAlternatingRowColors(true);
        adminTable->verticalHeader()->setDefaultSectionSize(38);
        adminTable->resizeRowsToContents();
        adminTable->resizeColumnsToContents();
        adminTable->horizontalHeader()->setStretchLastSection(true);
        UiFactory::applyWarmPolish(pageAdmin);
        installKineticVerticalScroll(pageAdmin);
        return;



    }







    // Create admin page



    pageAdmin = new QWidget();



    pageAdmin->setObjectName(QStringLiteral("page_admin"));



    QVBoxLayout *mainLayout = new QVBoxLayout(pageAdmin);



    mainLayout->setContentsMargins(20, 16, 20, 18);



    mainLayout->setSpacing(12);







    QLabel *lblTitle = new QLabel(tr("管理员工作台"), pageAdmin);
    lblTitle->setObjectName(QStringLiteral("pageTitle"));



    mainLayout->addWidget(lblTitle);

    QLabel *lblSubtitle = new QLabel(tr("集中处理账号、权益、报告与模拟登录"), pageAdmin);
    lblSubtitle->setObjectName(QStringLiteral("warmSubtitle"));
    mainLayout->addWidget(lblSubtitle);







    QTableWidget *table = new QTableWidget(pageAdmin);



    table->setObjectName(QStringLiteral("adminUserTable"));



    table->setColumnCount(7);



    table->setHorizontalHeaderLabels({



        tr("ID"), tr("手机号"), tr("角色"),



        tr("连续天数"), tr("冻结卡"), tr("总打卡"), tr("注册日期")



    });



    table->setSelectionBehavior(QAbstractItemView::SelectRows);



    table->setSelectionMode(QAbstractItemView::SingleSelection);



    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    table->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);



    table->horizontalHeader()->setStretchLastSection(true);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->verticalHeader()->setDefaultSectionSize(38);
    table->verticalHeader()->setVisible(false);
    table->setAlternatingRowColors(true);
    table->setWordWrap(false);



    table->setMinimumHeight(340);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);



    mainLayout->addWidget(table);













    QGridLayout *actionGrid = new QGridLayout();
    actionGrid->setHorizontalSpacing(8);
    actionGrid->setVerticalSpacing(8);



    QPushButton *btnEdit = UiFactory::secondaryButton(tr("编辑账号"), pageAdmin);



    QPushButton *btnPwd = UiFactory::secondaryButton(tr("重置密码"), pageAdmin);



    QPushButton *btnDelete = UiFactory::dangerButton(tr("删除用户"), pageAdmin);



    QPushButton *btnRole = UiFactory::secondaryButton(tr("切换角色"), pageAdmin);



    for (auto *b : {btnEdit, btnPwd, btnDelete, btnRole}) {
        b->setMinimumHeight(38);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }



    QPushButton *btnStreak = UiFactory::secondaryButton(tr("修改打卡天数"), pageAdmin);



    QPushButton *btnFreeze = UiFactory::secondaryButton(tr("修改冻结卡"), pageAdmin);



    QPushButton *btnReport = UiFactory::primaryButton(tr("生成报告"), pageAdmin);



    QPushButton *btnSimLogin = UiFactory::secondaryButton(tr("模拟登录"), pageAdmin);



    for (auto *b : {btnStreak, btnFreeze, btnReport, btnSimLogin}) {
        b->setMinimumHeight(38);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    actionGrid->addWidget(btnEdit, 0, 0);
    actionGrid->addWidget(btnPwd, 0, 1);
    actionGrid->addWidget(btnRole, 1, 0);
    actionGrid->addWidget(btnDelete, 1, 1);
    actionGrid->addWidget(btnStreak, 2, 0);
    actionGrid->addWidget(btnFreeze, 2, 1);
    actionGrid->addWidget(btnReport, 3, 0);
    actionGrid->addWidget(btnSimLogin, 3, 1);
    actionGrid->setColumnStretch(0, 1);
    actionGrid->setColumnStretch(1, 1);
    mainLayout->addLayout(actionGrid);







    QPushButton *btnReturn = UiFactory::ghostButton(tr("返回登录"), pageAdmin);



    btnReturn->setMinimumHeight(40);



    mainLayout->addWidget(btnReturn);



    mainLayout->addStretch();







    // Connect buttons



    connect(btnEdit, &QPushButton::clicked, this, [this, table]() {



        int uid = -1;



        int row = table->currentRow();



        if (row < 0) { AnimatedDialog::info(this, tr("提示"), tr("请先选择用户")); return; }



        uid = table->item(row, 0)->text().toInt();



        QString curPhone = table->item(row, 1)->text();



        bool ok;



        QString newPhone = AnimatedInputDialog::getText(this, tr("编辑手机号"), tr("新手机号:"), curPhone, &ok);



        if (ok && !newPhone.isEmpty()) {



            LifeBalanceAI::Services::UserService svc;



            if (svc.updateUserPhone(uid, newPhone)) loadAdminPage();



            else AnimatedDialog::warn(this, tr("错误"), tr("更新失败"));



        }



    });







    connect(btnDelete, &QPushButton::clicked, this, [this, table]() {



        int row = table->currentRow();



        if (row < 0) { AnimatedDialog::info(this, tr("提示"), tr("请先选择用户")); return; }



        int uid = table->item(row, 0)->text().toInt();



        if (uid == m_currentUserId) { AnimatedDialog::warn(this, tr("错误"), tr("不能删除当前登录账号")); return; }



        if (AnimatedDialog::confirm(this, tr("确认删除"),
                                    tr("确定删除用户 %1 及其所有数据？此操作不可撤销。").arg(uid))) {



            LifeBalanceAI::Services::UserService svc;



            if (svc.deleteUser(uid)) loadAdminPage();



            else AnimatedDialog::warn(this, tr("错误"), tr("删除失败"));



        }



    });







    connect(btnRole, &QPushButton::clicked, this, [this, table]() {



        int row = table->currentRow();



        if (row < 0) { AnimatedDialog::info(this, tr("提示"), tr("请先选择用户")); return; }



        int uid = table->item(row, 0)->text().toInt();



        QString curRole = table->item(row, 2)->text();



        QString newRole = (curRole == tr("律行者")) ? QStringLiteral("Explorer") : QStringLiteral("Ascendant");



        if (DatabaseManager::instance().updateUserRole(uid, newRole)) loadAdminPage();



        else AnimatedDialog::warn(this, tr("错误"), tr("切换角色失败"));



    });







    connect(btnPwd, &QPushButton::clicked, this, [this, table]() {



        int row = table->currentRow();



        if (row < 0) { AnimatedDialog::info(this, tr("提示"), tr("请先选择用户")); return; }



        int uid = table->item(row, 0)->text().toInt();



        bool ok;



        QString newPwd = AnimatedInputDialog::getText(this, tr("重置密码"), tr("新密码(至少6位):"), QString(), &ok);



        if (ok && newPwd.length() >= 6) {



            LifeBalanceAI::Services::UserService svc;



            if (svc.resetPassword(uid, newPwd)) AnimatedDialog::info(this, tr("成功"), tr("密码已重置"));



            else AnimatedDialog::warn(this, tr("错误"), tr("重置密码失败"));



        } else if (ok) AnimatedDialog::warn(this, tr("错误"), tr("密码至少需要 6 位"));



    });







    connect(btnStreak, &QPushButton::clicked, this, [this, table]() {



        int row = table->currentRow();



        if (row < 0) { AnimatedDialog::info(this, tr("提示"), tr("请先选择用户")); return; }



        int uid = table->item(row, 0)->text().toInt();



        int curStreak = table->item(row, 3)->text().toInt();



        bool ok;



        QString streakText = AnimatedInputDialog::getText(this, tr("修改打卡天数"), tr("打卡天数:"), QString::number(curStreak), &ok);
        bool numberOk = false;
        int newStreak = streakText.toInt(&numberOk);



        if (ok && numberOk && newStreak >= 0 && newStreak <= 999 && DatabaseManager::instance().setStreakDays(uid, newStreak)) loadAdminPage();



        else if (ok) AnimatedDialog::warn(this, tr("错误"), tr("修改失败，请输入 0-999 的数字"));



    });







    connect(btnFreeze, &QPushButton::clicked, this, [this, table]() {



        int row = table->currentRow();



        if (row < 0) { AnimatedDialog::info(this, tr("提示"), tr("请先选择用户")); return; }



        int uid = table->item(row, 0)->text().toInt();



        int curFreeze = table->item(row, 4)->text().toInt();



        bool ok;



        QString freezeText = AnimatedInputDialog::getText(this, tr("修改冻结卡"), tr("冻结卡数量:"), QString::number(curFreeze), &ok);
        bool numberOk = false;
        int newFreeze = freezeText.toInt(&numberOk);



        if (ok && numberOk && newFreeze >= 0 && newFreeze <= 99 && DatabaseManager::instance().setFreezeCards(uid, newFreeze)) loadAdminPage();



        else if (ok) AnimatedDialog::warn(this, tr("错误"), tr("修改失败，请输入 0-99 的数字"));



    });







    connect(btnReport, &QPushButton::clicked, this, [this, table]() {



        int row = table->currentRow();



        if (row < 0) { AnimatedDialog::info(this, tr("提示"), tr("请先选择用户")); return; }



        int uid = table->item(row, 0)->text().toInt();



        if (m_reportService && m_reportService->canGenerateReport(uid, true)) {



            showLoadingBar(tr("正在为所选用户生成报告"));



            m_reportService->generateReport(uid);



            AnimatedDialog::info(this, tr("提示"), tr("报告生成请求已发送"));



        } else {



            AnimatedDialog::info(this, tr("提示"), tr("该用户今天已生成过报告，或暂无可用于生成报告的规划数据。"));



        }



    });







    connect(btnSimLogin, &QPushButton::clicked, this, [this, table]() {



        int row = table->currentRow();



        if (row < 0) { AnimatedDialog::info(this, tr("提示"), tr("请先选择用户")); return; }



        int uid = table->item(row, 0)->text().toInt();



        if (uid == m_currentUserId) { AnimatedDialog::info(this, tr("提示"), tr("已经是当前用户")); return; }



        m_adminUserId = m_currentUserId;



        m_currentUserId = uid;



        loadMainPage();



        updatePlanUI();



        navigateTo(AppRoute::Home, false);



        AnimatedDialog::info(this, tr("模拟登录"),



            tr("已模拟登录为用户 %1。点击页面底部的“返回管理员”按钮退出。").arg(uid));



    });







    connect(btnReturn, &QPushButton::clicked, this, [this]() {



        navigateTo(AppRoute::Login, false);



    });







    ui->stackedWidget->addWidget(pageAdmin);







    // Initial data load



    LifeBalanceAI::Services::UserService svc;



    QVector<LifeBalanceAI::Models::UserInfo> users = svc.getAllUsers();



    table->setRowCount(users.size());



    for (int i = 0; i < users.size(); ++i) {



        const auto &u = users[i];



        table->setItem(i, 0, new QTableWidgetItem(QString::number(u.uid)));



        table->setItem(i, 1, new QTableWidgetItem(u.phone));



        table->setItem(i, 2, new QTableWidgetItem(



            u.role == QStringLiteral("Ascendant") ? tr("律行者") :
            u.role == QStringLiteral("admin") ? tr("管理员") :
            tr("探索者")));



        table->setItem(i, 3, new QTableWidgetItem(QString::number(u.streakDays)));



        table->setItem(i, 4, new QTableWidgetItem(QString::number(u.freezeCards)));



        table->setItem(i, 5, new QTableWidgetItem(QString::number(u.totalCheckins)));



        table->setItem(i, 6, new QTableWidgetItem(u.createdAt.toString(QStringLiteral("yyyy-MM-dd"))));



    }



    table->resizeColumnsToContents();



}
