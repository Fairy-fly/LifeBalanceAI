# LifeBalanceAI Profile Flow

Last updated: 2026-06-13

## Current Boundary

Profile setup and editing are now split between UI coordination in `MainWindow`
and workflow decisions in `ProfileFlowService`.

- `MainWindow` reads and writes widgets, shows dialogs, triggers nickname
  generation, refreshes pages, and calls `navigateTo(AppRoute, bool)`.
- `ProfileFlowService` validates profile input, saves profile data, builds and
  parses preferences, decides the next route, and reports whether an initial
  plan is needed.
- `DatabaseManager` remains the persistence layer and does not own UI routing.

## Routes

- New normal user without profile: `Login -> ProfileSetup -> Home`.
- Existing normal user with profile: `Login` or auto-login -> `Home`.
- Profile edit from "My": `Profile -> ProfileSetup edit mode -> Home`.
- Admin profile save: `ProfileSetup -> Admin`.

All profile-save routing is expressed through `ProfileSaveResult::nextRoute`.
The profile UI should not add numeric page indexes or duplicate role/profile
branching.

## Preferences

New saves write preferences with normal labels:

- `饮食:`
- `运动:`
- `目标:`

Parsing remains backward compatible with legacy mojibake labels so old local
device data can still populate Profile fields.

## Next Refactor Candidate

The next larger step should be a dedicated Profile UI/controller boundary that
owns profile page construction and edit-mode transitions. That work is separate
from this baseline and should not change database schema or authentication
policy.
