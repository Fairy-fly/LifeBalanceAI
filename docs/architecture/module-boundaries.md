# LifeBalanceAI Module Boundaries

Last updated: 2026-06-12

## UI Shell

Owns route selection, bottom navigation, and mobile shell layout.

- `AppRoute`: route names for user-visible app destinations.
- `MobileShellController`: maps routes to stacked pages, owns bottom-nav visibility, active tab state, and nav positioning.
- `PlatformLayoutPolicy`: central place for Android screen size, bottom nav height, touch scrolling, and horizontal-scroll normalization.

Do not put business rules or database queries here.

## Main Window

`MainWindow` is still the integration surface. It currently owns too much: login, profile editing, home plan rendering, feedback, AI plan generation, report setup, admin pages, animations, and mobile layout details.

First-pass rule: keep existing behavior but route new navigation through `AppRoute`. Future work should move one workflow at a time out of `MainWindow`.

## Services

The service layer contains workflow-level operations that already sit below the UI:

- `CheckinService`: daily check-in and streak behavior.
- `FreezeCardService`: freeze-card refill and streak protection.
- `DeepAnalysisService`: AI analysis request/parse/save flow.
- `ReportService`: report generation, image generation, export, and history.
- `UserService`: profile, user, upgrade, and admin operations.
- `AuthFlowService`: password-login validation, database lookup, password
  verification, and next-route decisions.
- `AuthSessionService`: remembered-device session persistence and validation.
- `ProfileFlowService`: profile input validation, profile save orchestration,
  preferences build/parse, and profile-save route decisions.

Services may call `DatabaseManager` and `AIManager`, but should not directly own widget behavior.

Auth services may call `DatabaseManager`, but should not show dialogs, access
widgets, trigger animations, or save UI state.

Profile flow code may call `DatabaseManager` and `UserService`, but should not
show dialogs, access widgets, trigger animations, or own bottom navigation.

## Data And AI

- `DatabaseManager` owns SQLite schema creation and persistence helpers.
- `AIManager` owns network calls and API-key loading.
- `models/dto.h` contains shared DTO structs.

First-pass rule: avoid adding new UI branching or page knowledge to these classes.

## Resources And Configuration

- `resources/` contains QSS, assets, fonts, and export helpers.
- `.env` is local-only and ignored.
- `.env.example` documents supported local variables.
- CMake no longer embeds `.env` into Qt resources.

## Archive And Tooling

- `docs/archive/verification/` stores historical screenshots, logs, and old phase reports.
- `docs/specs/` stores project Word documents.
- `tools/legacy/` stores temporary repair scripts that should not live in the source root.
