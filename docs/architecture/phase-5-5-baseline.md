# LifeBalanceAI Phase 5.5 Baseline

Last updated: 2026-06-12

## Scope

Phase 5.5 is a closeout pass for Phase 5. It does not start a new large
refactor. It records the current authentication boundary, verification results,
and remaining risks before moving to profile-flow cleanup.

## Code Baseline

Phase 5 introduced and verified:

- `services/authflowservice.h`
- `services/authflowservice.cpp`
- `services/authsessionservice.h`
- `services/authsessionservice.cpp`
- `tests/test_authflowservice.cpp`
- `LifeBalanceAI_authflow_tests` in `CMakeLists.txt`

`MainWindow` now routes login/session success through
`handleAuthenticatedUser(const AuthFlowResult &)`.

The old `handleAuthenticatedUser(int userId, phone, role, fromAutoLogin)`
overload is removed.

Both side-drawer logout and profile-page logout call `logoutToLogin()`.

## Build Baseline

Desktop build directory:

`LifeBalanceAI_demo/build/Desktop_Qt_6_5_3_MinGW_64_bit-Debug`

Android build directory:

`LifeBalanceAI_demo/build/Android_Qt_6_5_3_Clang_arm64_v8a-Debug`

Validated on 2026-06-12:

- Desktop main target builds.
- Android arm64 target builds.
- Android debug APK is generated.
- APK does not contain `.env`.
- APK contains `lib/arm64-v8a/libcrypto_3.so`.
- APK contains `lib/arm64-v8a/libssl_3.so`.

## Test Baseline

CTest passed 4/4 on desktop:

- `UserServiceValidation`
- `HomePlanService`
- `AiResponseParser`
- `AuthFlowService`

## Android Smoke Baseline

Device:

- Model reported by adb: `2211133C`
- Device product: `fuxi`
- Test date: 2026-06-12

Validated:

- Existing remembered session can auto-login to Home.
- Manual normal-user login can enter Home.
- Remembered login creates a `DeviceSessions` row.
- Force-stop and relaunch restores the remembered session.
- Home, Analysis, Report, and Profile bottom navigation remain stable.
- Profile-page logout returns to Login.
- Force-stop and relaunch after logout stays on Login.
- Final `DeviceSessions` check after logout returned an empty result.

Smoke screenshots were captured under the Android build directory:

`LifeBalanceAI_demo/build/Android_Qt_6_5_3_Clang_arm64_v8a-Debug/smoke_phase5/`

These are build artifacts and should not be committed.

## Security Notes

- `.env` remains local-only and is not packaged into the APK.
- Temporary copied SQLite databases used during smoke verification were local
  build artifacts only, were removed after verification, and must not be
  committed.
- The real API key path should stay outside Qt resources.

## Current Repository State

The working tree is intentionally not clean at this baseline because previous
rounds included repository hygiene, archived verification files, and generated
test/build state.

Before creating a commit, separate at least these groups:

- Phase 1-4 architecture and navigation/layout/AI parsing changes.
- Phase 5 authentication/session changes.
- Documentation and archive movement.
- Ignored build, screenshot, log, and local database artifacts.

Do not revert unrelated user or previous-round changes just to make the diff
smaller.

## Next Recommended Round

Open Phase 6 around profile setup and user-profile routing:

- Extract a profile-flow boundary.
- Stabilize no-profile -> ProfileSetup -> Home.
- Stabilize existing-profile -> Home.
- Stabilize profile edit from the Profile page.
- Add focused tests for profile input validation and route decisions.
