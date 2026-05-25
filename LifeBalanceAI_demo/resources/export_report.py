# -*- coding: utf-8 -*-
"""
LifeBalance AI - Report Exporter (Image)
Usage: python export_report.py <json_data> <output_path> <format>
  format: png
"""
import sys, json, os
from datetime import datetime

def export_png(data, output_path):
    # print("[DBG] importing PIL...", flush=True)
    from PIL import Image, ImageDraw, ImageFont
    # print("[DBG] PIL imported", flush=True)

    W, H = 600, 800
    bg_color = (255, 255, 255)
    title_color = (46, 125, 81)
    text_color = (51, 51, 51)
    accent_color = (200, 230, 201)
    line_color = (224, 224, 224)

    # print("[DBG] creating image...", flush=True)
    img = Image.new('RGB', (W, H), bg_color)
    draw = ImageDraw.Draw(img)

    # Prefer bundled fonts so exported AI reports match the Qt UI on every device.
    resources_dir = os.path.dirname(os.path.abspath(__file__))
    bundled_regular = os.path.join(resources_dir, 'fonts', 'MiSans-Regular.otf')
    bundled_medium = os.path.join(resources_dir, 'fonts', 'MiSans-Medium.otf')
    bundled_bold = os.path.join(resources_dir, 'fonts', 'MiSans-Bold.otf')

    def load_font(path, size):
        try:
            if os.path.exists(path):
                return ImageFont.truetype(path, size)
        except Exception:
            return None
        return None

    font_title = load_font(bundled_bold, 28)
    font_body = load_font(bundled_medium, 18)
    font_small = load_font(bundled_regular, 14)

    # Fall back to common system CJK fonts when the bundled files are unavailable.
    font_paths = [
        'C:/Windows/Fonts/msyh.ttc',
        'C:/Windows/Fonts/simhei.ttf',
        'C:/Windows/Fonts/simsun.ttc',
        '/System/Library/Fonts/PingFang.ttc',
        '/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc',
    ]
    if font_title is None or font_body is None or font_small is None:
        for fp_font in font_paths:
            if os.path.exists(fp_font):
                try:
                    font_title = ImageFont.truetype(fp_font, 28)
                    font_body  = ImageFont.truetype(fp_font, 18)
                    font_small = ImageFont.truetype(fp_font, 14)
                    break
                except Exception:
                    pass

    if font_title is None or font_body is None or font_small is None:
        font_title = ImageFont.load_default()
        font_body  = ImageFont.load_default()
        font_small = ImageFont.load_default()

    y = 30

    # Title
    nickname = data.get('nickname', '用户')
    rtype = data.get('type', 'weekly')
    report_name = '周报' if rtype == 'weekly' else '月报'
    title = f"LifeBalance AI - {nickname}的{report_name}"
    bbox = draw.textbbox((0, 0), title, font=font_title)
    tw = bbox[2] - bbox[0]
    draw.text(((W - tw) // 2, y), title, fill=title_color, font=font_title)
    y += 45

    # Divider
    draw.line([(40, y), (W - 40, y)], fill=title_color, width=2)
    y += 20

    # Basic info
    # print("[DBG] building info_items...", flush=True)
    info_items = [
  f"年龄: {data.get('age', '-')}  身高: {data.get('height', '-')} 厘米  体重: {data.get('weight', '-')} 公斤",
        f"性别: {data.get('gender', '-')}",
        f"周期: {data.get('start_date', '-')} ~ {data.get('end_date', '-')}",
        f"生成时间: {data.get('created_at', '-')}",
    ]
    for item in info_items:
        max_chars = 32
        start = 0
        while start < len(item):
            end = min(start + max_chars, len(item))
            draw.text((50, y), item[start:end], fill=text_color, font=font_small)
            y += 20
            start = end

    y += 10

    # AI 分析摘要
    ai_str = data.get('ai_summary', '{}')
    try:
        ai = json.loads(ai_str)
    except:
        ai = {}

    # print("[DBG] AI summary...", flush=True)
    draw.text((50, y), 'AI 分析摘要', fill=title_color, font=font_body)
    # print("[DBG] About to parse ai summary, y="+str(y), flush=True)
    y += 28
    # print("[DBG] y after +=28:", y, flush=True)
    # print("[DBG] ai type:", type(ai), repr(ai)[:100], flush=True)
    summary = ai.get('summary', '-')
    # print("[DBG] summary type:", type(summary), repr(summary)[:100], flush=True)
    # Character-based wrapping (max ~32 Chinese chars per line)
    max_chars = 32
    start = 0
    while start < len(summary):
        end = min(start + max_chars, len(summary))
        chunk = summary[start:end]
        draw.text((50, y), chunk, fill=text_color, font=font_small)
        y += 20
        start = end
    y += 12

    # print("[DBG] scores box...", flush=True)

    # Scores box

    box_top = y
    score_texts = [
        f"饮食评分: {ai.get('diet_score','-')}/10    运动评分: {ai.get('exercise_score','-')}/10",
        f"完成率: {ai.get('completion_rate','-')}%",
        f"下周目标: {ai.get('next_week_goal','-')}"
    ]
    sy = y + 10
    for st in score_texts:
        max_chars = 32
        start = 0
        while start < len(st):
            end = min(start + max_chars, len(st))
            draw.text((65, sy), st[start:end], fill=text_color, font=font_small)
            sy += 20
            start = end
    draw.rectangle([(50, box_top), (W - 50, sy + 10)], outline=accent_color, width=2)
    y = sy + 15

    # 亮点
    highlights = ai.get('highlights', [])
    if highlights:
        draw.text((50, y), '亮点', fill=title_color, font=font_body)
        y += 28
        for h in highlights:
            # Wrap long highlight text
            max_chars = 32
            htext = f"+ {h}"
            start = 0
            while start < len(htext):
                end = min(start + max_chars, len(htext))
                draw.text((60, y), htext[start:end], fill=text_color, font=font_small)
                y += 20
                start = end
        y += 5

    # 改进建议
    suggestions = ai.get('suggestions', [])
    if suggestions:
        draw.text((50, y), '改进建议', fill=title_color, font=font_body)
        y += 28
        for s in suggestions:
            max_chars = 32
            stext = f"- {s}"
            start = 0
            while start < len(stext):
                end = min(start + max_chars, len(stext))
                draw.text((60, y), stext[start:end], fill=text_color, font=font_small)
                y += 20
                start = end

    # print("[DBG] footer...", flush=True)

    # Footer
    y = max(y + 30, H - 40)
    draw.text((50, y), '由 LifeBalance AI 生成', fill=(180, 180, 180), font=font_small)

    # Crop to content height
    img = img.crop((0, 0, W, min(y + 40, H)))
    # print("[DBG] saving...", flush=True)
    img.save(output_path, 'PNG')
    return True

def main():
    if len(sys.argv) < 4:
        print("Usage: python export_report.py <json> <output> <format>", file=sys.stderr)
        sys.exit(1)
    arg1 = sys.argv[1]
    if os.path.isfile(arg1):
        with open(arg1, "r", encoding="utf-8-sig") as f:
            data = json.load(f)
        os.remove(arg1)
    else:
        data = json.loads(arg1)
    output = sys.argv[2]
    fmt = sys.argv[3].lower()
    if fmt == 'png':
        ok = export_png(data, output)
    else:
        print(f"Unknown format: {fmt}", file=sys.stderr)
        sys.exit(1)
    print(f"Saved: {output}" if ok else "Failed")
    sys.exit(0 if ok else 1)

if __name__ == '__main__':
    main()
