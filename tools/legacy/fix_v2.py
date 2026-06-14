import re

bak_path = r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp.bak"
corr_path = r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp"

# Read both as raw bytes
with open(bak_path, "rb") as f:
    bak_raw = f.read()
with open(corr_path, "rb") as f:
    corr_raw = f.read()

# Fix line endings in corrupted file
corr_raw = corr_raw.replace(b"\r\r\n", b"\n")
corr_raw = corr_raw.replace(b"\r\n", b"\n")
corr_raw = corr_raw.replace(b"\r", b"\n")

# Fix line endings in bak
bak_raw = bak_raw.replace(b"\r\r\n", b"\n")
bak_raw = bak_raw.replace(b"\r\n", b"\n")
bak_raw = bak_raw.replace(b"\r", b"\n")

# Decode as UTF-8
corr_text = corr_raw.decode("utf-8")
bak_text = bak_raw.decode("utf-8")

# Step 1: Extract all Chinese tr("...") strings from bak
bak_strings = set()
for m in re.finditer(r'tr\("((?:[^"\\]|\\.)*)"\)', bak_text):
    s = m.group(1)
    if any(ord(c) > 127 for c in s) and len(s) >= 2:
        bak_strings.add(s)

for m in re.finditer(r'QStringLiteral\("((?:[^"\\]|\\.)*)"\)', bak_text):
    s = m.group(1)
    if any(ord(c) > 127 for c in s) and len(s) >= 2:
        bak_strings.add(s)

# Step 2: Find all corrupted strings and fix them
# Pattern: find tr("...") where the content has \ufffd?
def fix_strings(text, func_name):
    prefix = func_name + '("'
    result = list(text)
    i = 0
    fixes = 0
    while i < len(text):
        idx = text.find(prefix, i)
        if idx < 0:
            break
        # Find end of string
        j = idx + len(prefix)
        esc = False
        found_end = False
        while j < len(text):
            c = text[j]
            if esc:
                esc = False
            elif c == '\\':
                esc = True
            elif c == '"':
                found_end = True
                break
            j += 1
        
        if found_end and j > idx + len(prefix):
            s = text[idx + len(prefix):j]
            if "\ufffd" in s:
                # Try to find match in bak
                ff_pos = s.index("\ufffd")
                prefix_str = s[:ff_pos]
                
                matched = None
                for bs in bak_strings:
                    if bs.startswith(prefix_str) and bs != s:
                        matched = bs
                        break
                
                if matched:
                    # Replace the whole tr("...") with tr("matched")
                    old = prefix + s + '")'
                    new = prefix + matched + '")'
                    text = text.replace(old, new, 1)
                    fixes += 1
                else:
                    # No match - just fix the structure
                    # Replace \ufffd? with a closing " 
                    # But only if \ufffd is followed by ? and then )
                    old = prefix + s + '")'
                    # The string is broken: "prefix\ufffd?")); 
                    # We need: "prefix")  or  "prefix...")
                    # Find \ufffd?
                    ffq = s.index("\ufffd")
                    if ffq + 1 < len(s) and s[ffq + 1] == '?':
                        # Replace with just the prefix + closing "
                        fixed_content = s[:ffq]  
                        new = prefix + fixed_content + '")'
                        text = text.replace(old, new, 1)
                        fixes += 1
        i = j + 1 if found_end else len(text)
    
    return text, fixes

# Also handle bare string literals (not in tr()) that have \ufffd
# Find all "...ufffd..." patterns
def fix_bare_strings(text):
    # Find all "string" literals containing \ufffd
    fixes = 0
    i = 0
    while i < len(text):
        # Find next "
        q = text.find('"', i)
        if q < 0:
            break
        # Check if it's inside tr() or QStringLiteral - skip those
        before = text[max(0, q-20):q]
        if 'tr(' in before or 'QStringLiteral(' in before or 'setText(' in before or 'setPlaceholderText(' in before or 'setWindowTitle(' in before:
            i = q + 1
            continue
        
        # Find closing "
        j = q + 1
        esc = False
        while j < len(text):
            c = text[j]
            if esc:
                esc = False
            elif c == '\\':
                esc = True
            elif c == '"':
                break
            j += 1
        if j >= len(text):
            i = q + 1
            continue
        
        s = text[q+1:j]
        if "\ufffd" in s:
            # Check if this is in a code context (not after certain functions)
            # Replace \ufffd? with just closing string
            old = '"' + s + '"'
            # Find first \ufffd
            ff = s.index("\ufffd")
            fixed = s[:ff]
            new = '"' + fixed + '"'
            text = text.replace(old, new, 1)
            fixes += 1
        i = j + 1
    
    return text, fixes

corr_text, tr_fixes = fix_strings(corr_text, "tr")
print(f"tr() fixes: {tr_fixes}")

corr_text, ql_fixes = fix_strings(corr_text, "QStringLiteral")
print(f"QStringLiteral fixes: {ql_fixes}")

corr_text, bare_fixes = fix_bare_strings(corr_text)
print(f"Bare string fixes: {bare_fixes}")

# Count remaining \ufffd
remaining = corr_text.count("\ufffd")
print(f"Remaining U+FFFD: {remaining}")

# Write
with open(corr_path, "w", encoding="utf-8") as f:
    f.write(corr_text)

# Quick sanity check - look at line 305
lines = corr_text.split("\n")
print(f"\nLine 305: {lines[304][:100]}")
print(f"Line 309: {lines[308][:100]}")
print("Done!")
