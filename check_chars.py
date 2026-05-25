path = r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\mainwindow.cpp"
with open(path, "rb") as f:
    data = f.read()

# "确定" in UTF-8: E7 A1 AE E5 AE 9A
ok_utf8 = bytes([0xe7, 0xa1, 0xae, 0xe5, 0xae, 0x9a])
ok_gbk = bytes([0xc8, 0xb7, 0xb6, 0xa8])
print("确定 UTF-8 found:", data.find(ok_utf8))
print("确定 GBK  found:", data.find(ok_gbk))

# "立即注册" - check both encodings
ljzc_utf8 = bytes([0xe7, 0xab, 0x8b, 0xe5, 0x8d, 0xb3, 0xe6, 0xb3, 0xa8, 0xe5, 0x86, 0x8c])
ljzc_gbk = bytes([0xc1, 0xa2, 0xbc, 0xb4, 0xd7, 0xa2, 0xb2, 0xe1])
print("立即注册 UTF-8 found:", data.find(ljzc_utf8))
print("立即注册 GBK  found:", data.find(ljzc_gbk))

# Find hamburger area
idx = data.find(b"btnHamburger")
print("\nHamburger area bytes:", repr(data[idx:idx+90]))

# Check for question marks near fromUtf8
utf8_calls = []
pos = 0
while True:
    pos = data.find(b"fromUtf8", pos)
    if pos < 0:
        break
    # Get 30 bytes after
    chunk = data[pos:pos+60]
    # Check if contains ? in the string
    qm = chunk.find(b'?"')
    if qm > 0:
        utf8_calls.append((pos, chunk))
    pos += 1

print(f"\nfromUtf8 calls with ?: {len(utf8_calls)}")
for pos, chunk in utf8_calls[:5]:
    print(f"  {pos}: {repr(chunk)}")
