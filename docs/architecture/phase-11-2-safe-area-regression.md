# Phase 11.2: Android Safe-Area Regression Baseline

## Summary

Phase 11.2 extends the Android bottom safe-area fix from the bottom navigation bar to mobile dialogs and scrollable pages. The goal is to keep navigation, dialogs, and bottom actions visible above the Android gesture area while preserving the existing Qt Widgets flow.

## Implementation Notes

- `PlatformLayoutPolicy` now owns safe content geometry for Android:
  - full screen geometry remains the window baseline;
  - Android `WindowInsets` is used for top and bottom insets when available;
  - screen `geometry()` / `availableGeometry()` differences remain the fallback;
  - fixed inset values are only fallback behavior.
- Dialog sizing and centering now use the shared safe-area helpers for Android paths:
  - lightweight `AnimatedDialog` / `AnimatedInputDialog`;
  - `FeedbackDialog`;
  - `DeepAnalysisDialog`;
  - yesterday review and report history/detail dialogs.
- Bottom-navigation pages continue to use `bottomNavPagePadding()` for content avoidance:
  - Home keeps the medical disclaimer above the navigation bar;
  - Analysis and Report reserve bottom space through the shared policy;
  - Profile was visually checked to keep logout and profile actions above the bar.

## Verification

- Desktop main target built successfully.
- All QtTest targets passed:
  - `LifeBalanceAI_demo_tests`
  - `LifeBalanceAI_homeplan_tests`
  - `LifeBalanceAI_airesponse_tests`
  - `LifeBalanceAI_aikeystatus_tests`
  - `LifeBalanceAI_authflow_tests`
  - `LifeBalanceAI_accountflow_tests`
  - `LifeBalanceAI_profileflow_tests`
- Android arm64 APK built successfully.
- APK check:
  - `.env` not packaged;
  - `libcrypto_3.so` packaged;
  - `libssl_3.so` packaged.
- Android device `99069a3f` was connected for smoke testing.
- Smoke-tested on device:
  - Home bottom navigation and medical disclaimer placement;
  - Home feedback dialog;
  - Analysis bottom navigation route;
  - Report bottom navigation route;
  - Profile bottom navigation route and logout button placement;
  - lightweight info dialog from yesterday review empty state.

## Remaining Manual Checks

- Login, registration, forgot-password, and ProfileSetup should be rechecked when it is acceptable to leave the current signed-in session.
- Report history/detail dialogs should be rechecked on an account with report history.
- Deep analysis result dialog should be rechecked after a real analysis response is available.
