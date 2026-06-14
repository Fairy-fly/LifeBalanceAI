# Verification Archive

This directory stores historical verification artifacts that used to live in the repository root.

## Layout

- `screenshots-and-logs/`: Android, phone, and LifeBalance screenshot/log captures from previous debugging sessions.
- `reports/`: previous UI/design/dark-mode/phase completion reports.
- `misc/`: accidental root files that were kept for traceability.
- `../source-backups/`: source snapshots that used to live inside the app tree.

## Policy

- Keep only current validation artifacts near the active work.
- Move older screenshots and logs here when they are still useful for diagnosis.
- Do not add new build outputs, `.env`, local databases, or IDE user files to git.
