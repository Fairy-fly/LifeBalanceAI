# Phase 11 UI Polish Audit

## Summary

Phase 11 audits the UI polish changes left after the repository hygiene pass. The goal is to keep only low-risk mobile improvements and avoid reintroducing unstable dialog, navigation, or global style behavior.

## Kept

- Deep analysis dialog mobile sizing and touch scrolling: the result dialog is sized for Android and supports native touch scroll.
- Feedback dialog compact mobile sizing: the dialog remains modal, keeps readable Chinese labels, and reduces vertical pressure without shrinking action buttons.
- Completion feedback overlay centering: the success overlay is centered instead of covering the full parent.
- Profile side drawer compact spacing: drawer header and item rows are slightly shorter while preserving tappable row height.
- Home plan card refinements: plan card title styling and plan text height are retained to improve readability.

## Deferred Or Reverted

- Android AnimatedDialog and AnimatedInputDialog scale/fade animations were reverted to direct show() to avoid invisible or stuck dialogs.
- Bottom navigation drag guard was deferred because ADB coordinate smoke testing could not reliably validate tab taps on this device.
- Page switch fade was deferred for the same reason; the existing route animation remains unchanged.
- Dark theme broad stylesheet override was deferred to a dedicated dark-mode validation round.
- Dark-mode token color changes were reverted; only light-mode diet color adjustments remain.
- Global touch target shrinkage in QSS and UIFactory was reverted.
- welcome_success.png binary replacement was reverted until the new asset is explicitly reviewed.

## Validation

- Desktop build: passed.
- QtTest suite: passed, 7/7 tests.
- Android arm64 APK build: passed.
- APK content check: no `.env`, `libcrypto_3.so` and `libssl_3.so` present.
- Android smoke: partial. App launched after cover install, Home rendered with plan cards, the side drawer opened with visible logout entry, and the adjustment input dialog rendered visibly. ADB coordinate taps could not reliably validate bottom navigation or feedback button entry because the app bottom bar sits close to the Android gesture area on the test device.

## Remaining Risk

- Bottom navigation safe-area placement should be handled in a focused platform-layout round.
- Feedback dialog layout should be rechecked with manual touch input because ADB coordinate taps did not open it consistently.
