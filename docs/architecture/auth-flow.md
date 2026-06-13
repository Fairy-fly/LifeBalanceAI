# LifeBalanceAI Auth Flow Boundary

Last updated: 2026-06-12

## Purpose

This document records the Phase 5 authentication boundary after the login,
auto-login, and logout cleanup pass.

The goal is to keep Android's mobile main path stable while avoiding another
large rewrite of `MainWindow`.

## Ownership

### `AuthFlowService`

Owns password-login workflow decisions:

- Validate phone and password input.
- Query `Users` by phone.
- Verify the password with the existing `DatabaseManager::hashPassword()`.
- Return an `AuthFlowResult`.
- Resolve the next route with `resolveNextRoute(role, hasProfile)`.

It must not:

- Save or clear remembered sessions.
- Show dialogs.
- Read widgets.
- Trigger animations.
- Change database schema or password storage.

### `AuthSessionService`

Owns remembered-device session state:

- `currentDeviceId()`
- `saveSession(userId, role, remember)`
- `clearSession()`
- `tryLoadValidSession()`

Session data is split between `QSettings` and `DeviceSessions`.
Administrators are intentionally not eligible for remembered login.
When `remember=false` or `role=admin`, the local session is cleared.

### `MainWindow`

Owns UI interaction only:

- Read phone/password/remember checkbox from widgets.
- Show login failure dialogs.
- Save the session after a successful manual login.
- Call `handleAuthenticatedUser(AuthFlowResult)`.
- Route the authenticated user to the result route.
- Run welcome animation for manual login only.
- Use `logoutToLogin()` for every logout entry point.

`MainWindow` should not reimplement password checks, device-session validation,
or profile/admin route decisions.

## Route Rules

`AuthFlowService::resolveNextRoute(role, hasProfile)` is the source of truth:

- `role == "admin"` -> `AppRoute::Admin`
- non-admin with profile -> `AppRoute::Home`
- non-admin without profile -> `AppRoute::ProfileSetup`
- failed login/session -> `AppRoute::Login`

Auto-login reuses the same route resolver through `AuthSessionService`.

## DTO Contract

`AuthFlowResult` keeps the Phase 2 fields:

- `ok`
- `userId`
- `phone`
- `role`
- `nextRoute`
- `message`
- `fromAutoLogin`

Failure results must keep `ok=false`, `userId=-1`, and `nextRoute=Login`.

## Current Test Coverage

`LifeBalanceAI_authflow_tests` covers:

- empty phone
- empty password
- invalid phone
- failed DTO defaults
- admin route
- user-with-profile route
- user-without-profile route

The tests intentionally avoid database fixtures for successful password login.
Successful login and remembered-login behavior are covered by Android smoke
testing.

## Known Gaps

- Android admin login smoke was not completed because the current device
  database only had a normal user.
- Register and forgot-password flows still live mostly in `MainWindow`.
- Profile setup/edit routing is the next high-value boundary to isolate.
