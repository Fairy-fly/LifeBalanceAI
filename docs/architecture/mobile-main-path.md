# LifeBalanceAI Mobile Main Path

Last updated: 2026-06-12

## Current Shape

LifeBalanceAI is currently a Qt Widgets application. Android and desktop share the same executable target in `LifeBalanceAI_demo/CMakeLists.txt`; Android behavior is selected through `Q_OS_ANDROID` branches rather than a separate mobile app.

The mobile runtime starts in `main.cpp`, initializes fonts, QSS, the database singleton, then creates `MainWindow`. On Android the window is shown maximized.

## Primary Android Flow

1. `main.cpp`
   - Sets `ANDROID_OPENSSL_SUFFIX`.
   - Loads embedded fonts and `:/style.qss`.
   - Initializes `DatabaseManager`.
   - Shows `MainWindow`.

2. `MainWindow` construction
   - Creates `CheckinService`, `FreezeCardService`, `DeepAnalysisService`, and `ReportService`.
   - Creates `BottomNavBar`, `SideDrawer`, and `MobileShellController`.
   - Registers `AppRoute` indexes for Login, Register, ProfileSetup, Home, Profile, and Payment.
   - Applies Android layout policy and schedules auto-login.

3. Login and auto-login
   - Manual login uses `AuthFlowService::loginWithPassword()`.
   - Remembered-device restore uses `AuthSessionService::tryLoadValidSession()`.
   - Manual login and auto-login both route through `handleAuthenticatedUser(AuthFlowResult)`.
   - Existing profile users go to `AppRoute::Home`; users without a profile go to `AppRoute::ProfileSetup`; admins go to the admin page.
   - Auto-login skips the welcome transition and routes directly to avoid blocking the Android launch path.

4. Home
   - `loadMainPage()` loads user, profile, streak, and current plan state.
   - `updatePlanUI()` fills the plan widgets for the active day tab.
   - Feedback and adjustment actions stay in `MainWindow` for now.

5. Bottom navigation
   - `BottomNavBar::currentChanged` is handled by `MobileShellController`.
   - Tabs map to `AppRoute::Home`, `AppRoute::Analysis`, `AppRoute::Report`, and `AppRoute::Profile`.
   - `MobileShellController` owns nav visibility, active tab sync, and nav positioning.

6. Analysis and report pages
   - `setupAnalysisPage()` and `setupReportPage()` still build dynamic pages inside `MainWindow`.
   - Their page indexes are registered back into `MobileShellController`.

7. Profile and payment
   - Profile entry uses `AppRoute::Profile`.
   - Upgrade entry uses `AppRoute::Payment`.
   - Successful payment returns to `AppRoute::Home`.

## First-Pass Stability Rules

- New mobile navigation should use `navigateTo(AppRoute, bool)`.
- New bottom-nav routes should be registered in `syncShellRouteIndexes()`.
- Do not introduce new hard-coded stacked-widget indexes except inside the route mapping layer.
- Android layout fixes should go through `PlatformLayoutPolicy` first.
- Login, auto-login, and logout changes should preserve the `AuthFlowResult`
  boundary described in `auth-flow.md`.
