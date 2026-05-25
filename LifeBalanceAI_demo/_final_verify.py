import sys, re
sys.stdout.reconfigure(encoding='utf-8')

def verify_file(label, filepath):
    print("=" * 60)
    print("=== %s ===" % label)
    with open(filepath, "rb") as f:
        raw = f.read()
    has_bom = raw.startswith(b'\xef\xbb\xbf')
    text = raw.decode("utf-8-sig")

    # Check for garbled patterns
    pua = sum(1 for c in text if 0xE000 <= ord(c) <= 0xF8FF)
    fffd = text.count("\ufffd")
    total_cjk = sum(1 for c in text if 0x4E00 <= ord(c) <= 0x9FFF)

    print("  BOM: %s | PUA: %d | \\ufffd: %d | CJK chars: %d" % (has_bom, pua, fffd, total_cjk))

    # Show all lines with non-ASCII in QStringLiteral
    for i, line in enumerate(text.split("\n"), 1):
        if "QStringLiteral" in line or 'tr("' in line:
            # Check for garbled chars in this line
            has_garbled = any(
                (0xE000 <= ord(c) <= 0xF8FF) or ord(c) == 0xFFFD
                for c in line
            )
            if has_garbled:
                print("  GARBLED Line %d: %s" % (i, line.strip()[:100]))

    # For reportservice: search for item.name and item.content
    if "report" in label.lower():
        for i, line in enumerate(text.split("\n"), 1):
            if "item.name" in line or "item.content" in line:
                print("  Line %d: %s" % (i, line.strip()[:120]))

    print()

verify_file("userservice.cpp", r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\services\userservice.cpp")
verify_file("deepanalysisservice.cpp", r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\services\deepanalysisservice.cpp")
verify_file("reportservice.cpp", r"D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\services\reportservice.cpp")
