import re

bak_path = r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp.bak"
corr_path = r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp"

with open(bak_path, "r", encoding="utf-8") as f:
    bak_text = f.read()

with open(corr_path, "r", encoding="utf-8") as f:
    corr_text = f.read()

# Function to extract all tr("...") strings from text
def extract_strings(text, func_name):
    results = {}
    # Find all tr("...") occurrences with their positions
    start = 0
    prefix = func_name + '("'
    while True:
        idx = text.find(prefix, start)
        if idx < 0:
            break
        # Find the closing ")
        i = idx + len(prefix)
        esc = False
        while i < len(text):
            c = text[i]
            if esc:
                esc = False
            elif c == '\\':
                esc = True
            elif c == '"':
                break
            i += 1
        if i < len(text):
            s = text[idx + len(prefix):i]
            results[s] = idx
        start = i + 1
    return results

bak_tr = extract_strings(bak_text, "tr")
corr_tr = extract_strings(corr_text, "tr")

# Build mapping: corrupted -> correct
fix_map = {}
for bad_s in corr_tr:
    if "\ufffd" in bad_s:
        # Find prefix before first \ufffd
        ff = bad_s.index("\ufffd")
        prefix = bad_s[:ff]
        if len(prefix) >= 2:
            # Find in bak_tr strings that start with this prefix
            for good_s in bak_tr:
                if good_s.startswith(prefix) and good_s != bad_s:
                    fix_map[bad_s] = good_s
                    break

print(f"tr() fixes found: {len(fix_map)}")

# Count how many corrupted strings we can't fix
unfixed = sum(1 for s in corr_tr if "\ufffd" in s and s not in fix_map)
print(f"tr() strings still corrupted: {unfixed}")

# Apply fixes
for bad, good in fix_map.items():
    corr_text = corr_text.replace('tr("' + bad + '")', 'tr("' + good + '")')

# For remaining corrupted strings, replace \ufffd? patterns with ASCII placeholders
# to make the code valid
import unicodedata

# Replace each \ufffd with a safe ASCII placeholder
# Strategy: replace \ufffd + following ? with nothing (just remove corruption)
# But we need to keep strings syntactically valid

# Actually, let me just replace \ufffd with '' and the following ? if present
# This will break the Chinese text but make the code compile
fixed_count = 0
while "\ufffd" in corr_text:
    pos = corr_text.index("\ufffd")
    # If next char is ?, remove both
    if pos + 1 < len(corr_text) and corr_text[pos + 1] == '?':
        corr_text = corr_text[:pos] + corr_text[pos + 2:]
    else:
        corr_text = corr_text[:pos] + corr_text[pos + 1:]
    fixed_count += 1

print(f"Replaced {fixed_count} remaining U+FFFD chars")

# Write
with open(corr_path, "w", encoding="utf-8") as f:
    f.write(corr_text)

# Verify
with open(corr_path, "r", encoding="utf-8") as f:
    final = f.read()
print(f"Final U+FFFD count: {final.count(chr(0xfffd))}")
print("Done!")
