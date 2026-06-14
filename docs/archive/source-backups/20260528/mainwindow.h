#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QPropertyAnimation>
#include <QDate>
#include <functional>
#include "services/checkinservice.h"
#include "services/freezecardservice.h"
#include "services/deepanalysisservice.h"
#include "services/reportservice.h"
#include "bottomnavbar.h"
#include "sidedrawer.h"

class QTextEdit;       // forward declaration
class QLabel;
class QPushButton;
class QCheckBox;
class LoadingOverlay;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    // Page switching
    void on_btnGoRegister_clicked();
    void on_btnGoLogin_clicked();

    // Registration
    void on_btnRegister_clicked();

    // Simulate verification code
    void on_btnSendVerifyCode_clicked();

    // Login
    void on_btnLogin_clicked();
    void onForgotPassword();

    // Save profile
    void on_btnSaveProfile_clicked();

    // ── AI plan generation ────────────────────────────────────────
    void onPlanGenerated(const QString &jsonResult);

    // Helper: triggers AI plan generation using current user's profile
    void requestAIPlan();

    // Called when any of the 12 feedback buttons on page_4 is clicked
    void onFeedbackButtonClicked(QTextEdit *textEdit, const QString &slotTitle);

    // ── Partial update slot ───────────────────────────────────────
    void onPartialUpdateGenerated(const QString &jsonResult);

    // ── Yesterday view ────────────────────────────────────────────
    void onViewYesterdayClicked();

    // ── Adjust plan (new feature) ─────────────────────────────────
    void onAdjustPlanClicked(QTextEdit *textEdit, const QString &slotKey, const QString &slotTitle);

    // ── Tab change ────────────────────────────────────────────────
    void onTabChanged(int index);

    // Page 5 (Personal Center)
    void on_btnSaveProfile5_clicked();
    void populateProfileEditFields(const LifeBalanceAI::Models::ProfileData &profile);
    void on_btnBackToMain_clicked();

    // Page 6 (Payment / Upgrade)
    void on_btnPay6_clicked();
    void on_btnBack6_clicked();

    // Deep analysis
    void onDeepAnalysisTriggered();
    void onDeepAnalysisReady(int userId, const LifeBalanceAI::Models::DeepAnalysisResult &result);
    void onDeepAnalysisError(int userId, const QString &error);
    void onNicknameGenerated(const QString &nickname);

    // Report
    void onGenerateReport();
    void onExportReport(int reportId, const QString &format);
    void onReportReady(int userId, const LifeBalanceAI::Models::ReportData &report);
    void onReportError(int userId, const QString &error);

private:
    // Helper: concatenate checked checkbox texts from a group box,
    //         then append the custom line edit content (if not empty).
    QString collectCheckedTexts(QObject *groupBox, const QString &editName) const;

    // Convenience wrappers for each group box
    QString getDietString()   const;
    QString getSportString()  const;
    QString getTargetString() const;

    // Build a structured user profile string from DB for AI consumption
    QString buildUserProfileString() const;

    // Populate page_4 UI with data from the database
    void loadMainPage();

    // Load daily items for a given day offset (0=today, 1=tomorrow, 2=day-after)
    // and fill the corresponding QTextEdit widgets
    void updatePlanUI();

    // Show/hide the loading bar in the status bar when AI is generating
    void showLoadingBar(const QString &message = QString());
    void hideLoadingBar();

    // ── New helpers ───────────────────────────────────────────────
    /** Programmatically add "查看昨日规划" button, all-done label, and 12 adjust buttons */
    void setupPage4Widgets();

    /** Update the "今日已全部打卡" label based on today's is_done status */
    void updateAllDoneLabel();

    /** Enable/disable all feedback+adjust buttons for the currently active tab */
    void updateButtonStatesForTab();

    /** Load profile data into page_5 fields */
    void loadPage5();
    void loadAdminPage();
    void showReportHistory();
    int setupAnalysisPage();
    int setupReportPage();

    /** Determine slot key and title from a textEdit pointer (shared helper) */
    void resolveSlotInfo(QTextEdit *textEdit, QString &outSlotKey, QString &outSlotTitle) const;

    LifeBalanceAI::Services::CheckinService *m_checkinService = nullptr;
    LifeBalanceAI::Services::FreezeCardService *m_freezeCardService = nullptr;
    LifeBalanceAI::Services::DeepAnalysisService *m_deepAnalysisService = nullptr;
    LifeBalanceAI::Services::ReportService *m_reportService = nullptr;
    Ui::MainWindow *ui;

    bool m_isRequestPending = false;
    int m_currentUserId = -1;   // stores the logged-in user's id
    bool m_profileEditFromPage5 = false;  // track if editing profile from page_5
    bool m_reprofilePromptPending = false;
    int m_adminUserId = -1;          // stores admin id during simulate-login
    QDate m_lastRefreshDate;       // tracks last date for cross-day refresh detection
    QString m_pendingAdjustSlotKey; // slot key of the pending adjustment (for post-AI marking)
    QString m_pendingAdjustDate;    // date of the pending adjustment
    QLabel *m_loadingLabel = nullptr;
    LoadingOverlay *m_loadingOverlay = nullptr;
    QPropertyAnimation *m_breathAnim = nullptr;
    QTimer *m_typewriterTimer = nullptr;
    QString m_typewriterFullText;
    int m_typewriterPos = 0;

    // Bottom navigation bar
    int m_analysisPageIndex = -1;
    int m_reportPageIndex = -1;
    int m_prePaymentPageIndex = 4;  // track where we came from before payment page
    BottomNavBar *m_bottomNav = nullptr;
    SideDrawer *m_sideDrawer = nullptr;
    void setupSideDrawer();
    void applyWarmVisualPolish();
    void addRememberLoginControl();
    QCheckBox *rememberLoginCheckBox() const;
    QString currentDeviceId() const;
    void saveAutoLoginSessionIfNeeded(int userId, const QString &role);
    void clearAutoLoginSession();
    bool tryAutoLogin();
    void handleAuthenticatedUser(int userId, const QString &phone, const QString &role, bool fromAutoLogin);
    void showLoginSuccessTransition(const QString &welcomeText, std::function<void()> routeAction);
    void updateGoalCollapseState();
    void animatePageSwitch(int fromIndex, int toIndex, bool forward);
    void positionBottomNav();
    void updateBottomNavVisibility();
    void updateBottomNavVisibility(int pageIndex);
    BottomNavBar *createAndroidPageNav(int activeIndex, QWidget *parent);
    void handleBottomNavTab(int tab);
    void navigateTo(int toIndex, bool animate = true);

    // ── New widgets on page_4 ────────────────────────────────────
    QLabel       *m_lblAllDone        = nullptr;    // "🎉 今日已全部打卡，自律的一天！"
    QPushButton  *m_btnViewYesterday  = nullptr;    // "查看昨日规划"
    QPushButton  *m_btnGoalToggle      = nullptr;
    bool          m_goalExpanded       = false;
    QString       m_goalText;               // cached goal text
    QTextEdit    *m_goalContentEdit    = nullptr; // runtime replacement for lblGoalContent (QLabel word-wrap bug)

    // 12 "调整规划" buttons (one per slot, same count as feedback buttons)
    QPushButton  *m_btnAdjustTodayMorning   = nullptr;
    QPushButton  *m_btnAdjustTodayLunch     = nullptr;
    QPushButton  *m_btnAdjustTodayDinner    = nullptr;
    QPushButton  *m_btnAdjustTodayExercise  = nullptr;
    QPushButton  *m_btnAdjustTomorrowMorning   = nullptr;
    QPushButton  *m_btnAdjustTomorrowLunch     = nullptr;
    QPushButton  *m_btnAdjustTomorrowDinner    = nullptr;
    QPushButton  *m_btnAdjustTomorrowExercise  = nullptr;
    QPushButton  *m_btnAdjustDayAfterMorning   = nullptr;
    QPushButton  *m_btnAdjustDayAfterLunch     = nullptr;
    QPushButton  *m_btnAdjustDayAfterDinner    = nullptr;
    QPushButton  *m_btnAdjustDayAfterExercise  = nullptr;

    // Internal mapping: list of slot names for iteration
    QList<QPushButton *> m_allAdjustButtons;
    QList<QPushButton *> m_allFeedbackButtons;
};

#endif // MAINWINDOW_H
