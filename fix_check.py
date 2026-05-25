path = r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp"
with open(path, "rb") as f:
    data = f.read()
lines = data.split(b"\n")
# Line 305
line = lines[304]
print(f"Line 305 hex: {line.hex()}")
# Find \ufffd bytes (ef bf bd)
count_ff = line.count(b"\xef\xbf\xbd")
print(f"U+FFFD bytes count in line 305: {count_ff}")
# Try decode
try:
    txt = line.decode("utf-8")
    print(f"Text: {txt[:120]}")
except Exception as e:
    print(f"Decode error: {e}")

# Also check line 309
line2 = lines[308]
print(f"\nLine 309 hex: {line2.hex()}")
count_ff2 = line2.count(b"\xef\xbf\xbd")
print(f"U+FFFD bytes count in line 309: {count_ff2}")
