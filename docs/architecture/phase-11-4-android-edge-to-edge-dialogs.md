# Phase 11.4: Android Edge-To-Edge And Dialog Geometry

## Summary

Phase 11.4 moves Android from a maximized-but-system-framed window to an edge-to-edge surface. The app background now extends under the visible status bar and gesture area, while page content, bottom navigation text, and dialogs continue to use safe-area insets.

## Implementation Notes

- `PlatformLayoutPolicy` owns Android edge-to-edge setup:
  - transparent status bar;
  - app-colored navigation bar;
  - disabled navigation-bar contrast enforcement on Android 10+;
  - `decorFitsSystemWindows(false)` on Android 11+;
  - stable fullscreen/navigation layout flags as fallback.
- `MainWindow` reapplies Android insets on first layout, after the decor view settles, and after resize events.
- The central layout gets top safe-area padding so page content does not sit under the visible status bar.
- Bottom navigation remains anchored to the physical window bottom and keeps its text/indicator above the bottom inset.
- `AnimatedDialog` uses content-driven Android sizing:
  - short alert messages stay compact;
  - longer messages expand within a bounded height;
  - dialogs are re-centered after Android finishes showing the native dialog window.
- `AnimatedInputDialog` keeps the keyboard-aware placement from Phase 11.3 and uses a separate input-dialog size policy.

## Verification

- Desktop main target built successfully.
- Android arm64 APK built successfully.
- Android smoke on device `99069a3f`:
  - Login background extends behind the visible status bar.
  - Bottom gesture area no longer renders as a black/white strip; it matches the app page background.
  - Home bottom navigation remains readable above the gesture handle.
- Alert dialog sizing was changed from fixed large sizing to `sizeHint()`-driven sizing with Android min/max bounds. ADB coordinate input was unreliable for repeatedly opening the same report dialog after reinstall, so the exact alert screenshot should be manually rechecked once the device is in the target page.

## Remaining Risk

- Different Android skins may report gesture insets differently; the policy still falls back to screen geometry and bounded fixed values.
- Manual visual recheck is recommended for `My Reports`, `conditions insufficient`, `registration success`, and `profile saved` alert dialogs to confirm the content-driven size feels right on the physical device.
- Input dialogs remain intentionally higher than center when the keyboard is expected, prioritizing button reachability over visual centering.
