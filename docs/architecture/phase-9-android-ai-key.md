# LifeBalanceAI Phase 9 Android AI Key And Real Request Baseline

## Purpose

Phase 9 makes the Android DeepSeek chat flow testable without bundling or
committing a real `.env`. It focuses on real Home plan, Analysis, and Report AI
requests. It does not change schema, password policy, Qt Widgets architecture,
or payment/admin features.

## Key Boundary

- Chat requests use `DEEPSEEK_API_KEY`.
- `OPENAI_API_KEY` remains only for image generation; current report export
  uses deterministic local rendering and does not require OpenAI.
- `AIManager::chatApiKeyStatus()` reports whether the chat key is configured,
  the source type/path, checked paths, and a user-facing message. It never
  returns the actual key.
- Android debug builds keep `.env` outside Qt resources and APK contents.

## Android Debug Setup

Use a local untracked `.env` containing `DEEPSEEK_API_KEY`, then run:

```powershell
powershell -ExecutionPolicy Bypass -File tools/android_push_env.ps1 -EnvFile .env
```

The script writes only `DEEPSEEK_API_KEY` into app-private config locations via
`adb run-as`, does not print the key, and requires an app restart.

## Smoke Expectations

- Home can generate and save a three-day plan when a profiled user has no
  current plan.
- Analysis can issue a real request, parse the response, cache it, and show the
  result without breaking navigation.
- Report can issue a real request when business rules allow it. If rules block
  generation, the page shows a clear dialog reason.
- Removing or emptying the key restores the Phase 8 missing-key dialog behavior.

## 2026-06-14 Device Baseline

- Android arm64 APK builds successfully and does not contain `.env`.
- The APK still contains `libcrypto_3.so` and `libssl_3.so`.
- `tools/android_push_env.ps1` wrote `DEEPSEEK_API_KEY` into app-private config
  locations via `adb run-as` without printing the key.
- Analysis was verified on device with a real DeepSeek response and displayed
  the generated analysis result.
- Report was verified on device with a real DeepSeek response and displayed the
  generated report dialog.
- Home was verified on device after relaunching into the existing Ascendant
  smoke-test account. `AiRequestLog` recorded `planGenerated` with a non-empty
  response at `2026-06-14 02:22:11`.
- The generated Home plan was saved to `Plans` / `Daily_Items` with 12 items
  covering `2026-06-14` through `2026-06-16` across breakfast, lunch, dinner,
  and sports.
- A new Explorer account was also registered successfully for later profile
  setup testing. Earlier adb-driven login attempts failed local phone
  validation, which was treated as an automation limitation after auto-login
  and database verification completed the Home AI smoke path.
