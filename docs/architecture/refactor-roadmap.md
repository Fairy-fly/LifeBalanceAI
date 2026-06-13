# LifeBalanceAI Gradual Refactor Roadmap

Last updated: 2026-06-12

## Phase 1: Mobile Shell And Repository Hygiene

Status: implemented in this pass.

- Move historical verification artifacts out of the repository root.
- Remove build outputs, local config, IDE state, logs, and local databases from git tracking.
- Add architecture docs for the mobile main path and module boundaries.
- Introduce `AppRoute`, `MobileShellController`, and `PlatformLayoutPolicy`.
- Remove `.env` resource embedding from CMake.

Acceptance:

- `git ls-files LifeBalanceAI_demo/build` returns no tracked files.
- New mobile navigation code can use `navigateTo(AppRoute, bool)`.
- Bottom navigation visibility and active tab state are handled through `MobileShellController`.

## Phase 2 / 5: Login And User State

Status: implemented for login, auto-login, logout, and session restore.

- Extracted auto-login/session persistence into `AuthSessionService`.
- Extracted password-login validation and next-route decisions into `AuthFlowService`.
- Replaced login success routing with `AuthFlowResult`.
- Unified profile-page and side-drawer logout through `logoutToLogin()`.
- Added `LifeBalanceAI_authflow_tests`.

Acceptance:

- `MainWindow` asks an auth workflow for the next `AppRoute`.
- Login, auto-login, logout, and profile setup all use route names, not numeric indexes.

Remaining:

- Admin Android smoke needs a device database with an admin account.
- Register and forgot-password flows still need their own boundary pass.

## Phase 3: Home Plan And Feedback

Goal: reduce risk when changing plan display, feedback, and adjustment behavior.

- Extract plan loading/rendering coordination from `MainWindow`.
- Keep `DatabaseManager` as persistence, but put plan UI state in a home-page controller.
- Centralize the 12 slot mappings and remove duplicated slot-specific button code.
- Add smoke tests for slot-key resolution and adjusted/done state transitions.

Acceptance:

- Feedback and adjustment entry points no longer depend on scattered widget-name logic.
- Home refresh after navigation is controlled by one workflow.

## Phase 6 / 6.5: Profile Setup And User Profile

Status: implemented for profile validation, profile save routing, preferences
compatibility, and unreachable legacy save-code cleanup.

- Extracted profile validation/save decisions into `ProfileFlowService`.
- Added `ProfileInput`, `ProfileSaveResult`, `ProfilePreferenceParts`, and
  `ProfileSaveMode`.
- Profile setup now routes through `ProfileSaveResult::nextRoute`.
- Profile edit from "My" saves through the service and returns to `Home`.
- New saves use `饮食:` / `运动:` / `目标:` preferences labels while parsing
  remains compatible with legacy mojibake labels.

Acceptance:

- `MainWindow` no longer contains the old unreachable direct profile-save
  branches after the service-based save path.
- New user `ProfileSetup -> Home`, existing profile auto-login -> `Home`, and
  `Profile -> edit -> Home` remain stable on Android.

Remaining:

- Profile page construction and edit-mode transitions still live in
  `MainWindow`; a future UI/controller split can handle that without changing
  schema or auth policy.

## Phase 4: AI Plan, Analysis, And Reports

Goal: isolate AI request/parse failures from UI navigation failures.

- Extract prompt construction and JSON parsing into testable helpers.
- Add fallback/error DTOs for AI failures.
- Split report page construction from report generation.
- Keep network calls in `AIManager`; keep persistence in services.

Acceptance:

- AI parsing can be tested without launching the UI.
- Report and analysis pages can be opened without triggering network calls.
