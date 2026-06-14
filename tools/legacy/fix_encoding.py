import re

bak_path = r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp.bak"
corr_path = r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp"

with open(bak_path, "r", encoding="utf-8") as f:
    bak_text = f.read()

with open(corr_path, "r", encoding="utf-8") as f:
    corr_text = f.read()

# Find corrupted tr("...") strings and match with bak
pattern = re.compile(r'tr\("((?:[^"\\]|\\.)*)"\)')
fixes = {}
for m in pattern.finditer(corr_text):
    s = m.group(1)
    if "\ufffd" in s:
        ff_pos = s.index("\ufffd")
        prefix = s[:ff_pos]
        if len(prefix) >= 2:
            bak_pattern = re.compile(
                r'tr\("(' + re.escape(prefix) + r'(?:[^"\\]|\\.)*)"\)'
            )
            bak_m = bak_pattern.search(bak_text)
            if bak_m:
                correct = bak_m.group(1)
                if correct != s:
                    fixes[s] = correct

print("Strings to fix:", len(fixes))

# Count corrupted strings not matched
unmatched = 0
for m in pattern.finditer(corr_text):
    s = m.group(1)
    if "\ufffd" in s and s not in fixes:
        unmatched += 1
print("Unmatched corrupted strings:", unmatched)

# Do the replacement
for bad, good in fixes.items():
    # Escape for regex
    bad_esc = re.escape(bad)
    # Replace in tr("...") context only
    corr_text = corr_text.replace('tr("' + bad + '")', 'tr("' + good + '")')

# Also fix QStringLiteral
ql_pattern = re.compile(r'QStringLiteral\("((?:[^"\\]|\\.)*)"\)')
ql_fixes = {}
for m in ql_pattern.finditer(corr_text):
    s = m.group(1)
    if "\ufffd" in s:
        ff_pos = s.index("\ufffd")
        prefix = s[:ff_pos]
        if len(prefix) >= 2:
            bak_pattern = re.compile(
                r'QStringLiteral\("(' + re.escape(prefix) + r'(?:[^"\\]|\\.)*)"\)'
            )
            bak_m = bak_pattern.search(bak_text)
            if bak_m:
                correct = bak_m.group(1)
                if correct != s:
                    ql_fixes[s] = correct

print("QStringLiteral fixes:", len(ql_fixes))

for bad, good in ql_fixes.items():
    corr_text = corr_text.replace('QStringLiteral("' + bad + '")', 'QStringLiteral("' + good + '")')

# Write fixed file
with open(corr_path, "w", encoding="utf-8") as f:
    f.write(corr_text)

# Verify
rem = corr_text.count("\ufffd")
print("Remaining U+FFFD:", rem)
print("Done!")
