# Phase 11.3: Remaining Android Form And Dialog Flows

## Summary

Phase 11.3 completes the mobile smoke coverage left open by Phase 11.2. The scope stayed limited to Android form pages, safe-area placement, keyboard avoidance, and result/failure dialogs. No business flow, database schema, AI service contract, auth logic, or profile persistence rule was changed.

## Implementation Notes

- `AnimatedInputDialog` now uses Android safe-area placement instead of simple parent centering.
- On Android, the input dialog is placed higher in the available safe rectangle so its action buttons stay above the soft keyboard.
- A short delayed placement pass rechecks the dialog after the keyboard settles, covering the forgot-password phone, code, and new-password steps.
- Desktop keeps the existing centered animated input-dialog behavior.

## Android Smoke Results

- Device: `99069a3f`, physical size `1080x2400`.
- Login page:
  - login form rendered inside the safe area;
  - forgot-password entry was visible and tappable.
- Forgot password:
  - phone input dialog was visible above the keyboard;
  - verification-code input dialog was visible above the keyboard;
  - new-password input dialog was visible above the keyboard;
  - the test cancelled before submitting a password reset.
- Registration:
  - created ordinary Explorer test account `13906129996 / 123456` with mock code `1234`;
  - success dialog was visible and dismissible;
  - when the keyboard covered the lower page, vertical scrolling exposed the register button.
- Initial ProfileSetup:
  - new account routed to ProfileSetup after login;
  - height and weight fields were editable with the keyboard open;
  - the bottom save button could be reached by scrolling and stayed above the gesture area;
  - save success dialog was visible and dismissible;
  - after save, the app routed to Home.
- Home:
  - the new profile entered Home;
  - the current device key configuration allowed a real three-day plan to generate and display;
  - bottom navigation stayed above the Android gesture area.
- Analysis:
  - Explorer account showed the expected upgrade-gated state;
  - no result dialog was generated because the account does not have analysis entitlement.
- Report:
  - report page opened through the bottom navigation;
  - `My Reports` empty-state dialog was visible and dismissible;
  - report generation showed an explicit insufficient-conditions dialog instead of a silent button click.
- Profile:
  - profile page opened through the bottom navigation;
  - profile data and logout action stayed above the bottom safe area;
  - the app was left in a usable logged-in state with the new test account.

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
- `git diff --check` passed with only existing LF/CRLF warnings.

## Remaining Risk

- DeepAnalysis result-dialog sizing still needs a follow-up check with an account that has analysis entitlement.
- Report detail dialog still needs a follow-up check with an account that already has report history or can generate a valid weekly report.
- Registration with the soft keyboard open depends on scroll reachability rather than automatically moving the submit button above the keyboard; this passed on the test device, so no code change was made in this round.
