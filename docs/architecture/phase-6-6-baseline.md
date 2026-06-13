# LifeBalanceAI Phase 6.6 Baseline

Last updated: 2026-06-13

## Purpose

This baseline freezes the verified mobile governance work from phases 1 through
6.5 so the next feature or refactor pass starts from a known-good Android main
path.

This pass is not a feature change. It is a staging and verification checkpoint
for the mobile-first architecture work that has already been built and smoke
tested.

## Included Governance Work

- Mobile shell: route names, bottom navigation coordination, and Android layout
  policy are separated from raw page-index navigation.
- Auth: password login, auto-login session persistence, logout, and next-route
  decisions are behind service boundaries.
- Home plan: 12-slot plan mapping and AI plan parsing have pure service tests.
- AI failure isolation: deep analysis and report parsing use testable parser
  DTOs and keep AI parse failures away from navigation.
- Profile flow: profile validation, preferences build/parse, save routing, and
  Profile edit return behavior are handled by `ProfileFlowService`.
- Profile cleanup: old unreachable direct profile-save branches in `MainWindow`
  have been removed.

## Verification Baseline

The latest verified Android main path is:

- Login or auto-login reaches `Home` for a user with an existing profile.
- New users without a profile route to `ProfileSetup` and then `Home`.
- Bottom navigation opens `Home`, `Analysis`, `Report`, and `Profile`.
- Profile edit from "My" saves through the service and returns to `Home`.
- Logout clears the remembered session; relaunch after logout stays on `Login`.

APK checks expected for this baseline:

- `.env` is not packaged.
- `libcrypto_3.so` and `libssl_3.so` are packaged for arm64.

## Staging Boundary

Stage only the mobile governance source, tests, architecture docs, and required
configuration files. Do not stage repository-hygiene-only deletions such as
historical screenshots, root reports, local databases, build outputs, `.config`,
or IDE state.

Recommended commit message:

```text
chore: freeze mobile governance baseline
```

## Remaining Risks

- The worktree still contains many historical cleanup changes that should be
  committed separately or reviewed in a dedicated repository hygiene pass.
- `MainWindow` remains large. Future work should continue extracting one
  workflow at a time instead of rewriting it wholesale.
- Admin Android smoke still depends on a device database that contains an admin
  account.
