# LifeBalanceAI Phase 8 Account And AI Feedback Baseline

## Purpose

Phase 8 freezes the Phase 7 account-entry governance and the Phase 7.5 AI
feedback fixes into a clean mobile baseline commit. It does not add product
features, change database schema, bundle real API keys, or continue broader UI
refactoring.

## Account Flow Boundary

- `AccountFlowService` owns registration, mock verification code validation,
  register role normalization, and password reset decisions.
- `MainWindow` reads and clears widgets, shows dialogs, and performs route
  changes. It no longer owns duplicate SQL, password validation, or mock code
  checks for registration and password reset.
- Registration still returns to `Login` after success. It does not auto-login.
- Password reset keeps the existing mock code `1234` and existing password hash
  behavior through `UserService` and `DatabaseManager`.

## AI Feedback Boundary

- `AIManager::hasChatApiKey()` exposes a read-only key availability check for
  UI preflight decisions.
- Home plan auto-generation shows page-level status when AI is unavailable.
  It does not show blocking dialogs because it can run automatically after
  login or profile save.
- Analysis and Report user-triggered actions show both an inline page status
  and an `AnimatedDialog` for immediate failures such as missing login state,
  missing service, unmet eligibility, no plan data, or missing API key.
- Asynchronous AI/network failures remain non-blocking page-level status
  updates so navigation remains stable.

## Verification Baseline

- Desktop main target builds successfully.
- QtTest coverage includes `UserServiceValidation`, `HomePlanService`,
  `AiResponseParser`, `AuthFlowService`, `ProfileFlowService`, and
  `AccountFlowService`.
- Android arm64 APK builds successfully.
- APK must not contain `.env`.
- APK must still contain `libcrypto_3.so` and `libssl_3.so`.
- Android smoke expectations: Home, Analysis, Report, and Profile navigation
  remain stable; Analysis and Report missing-key clicks show visible dialogs;
  logout clears session and relaunch stays on Login.

## Deferred Work

- Real Android API key provisioning and end-to-end AI success testing are
  deferred to a later round.
- Repository hygiene for historical screenshots, build products, old reports,
  and tool archives remains separate from this baseline commit.
