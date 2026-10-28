#!/usr/bin/env python3
"""preview.py — як виглядатиме стан запуску (status.cpp) на табло: 2 поверхи × 4 модулі (двоповерховий годинник) або 1 поверх.
Шрифти — справжні (fonts_2_7_1.h, значки — з status.cpp), текст перекодовує справжня utf8_to_led() (u2l.cpp).
Виводить анімований GIF (кадр = крок прокрутки) і PNG-стрічку з кількох кадрів.
Використання: preview.py <вид 0..8> <вихід.gif> [--rows 1|2] [--font sys0|sys1|sys2] [--speed N]"""
import re, sys, os, subprocess, struct, zlib, tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
SRC = os.path.join(HERE, '..', '..', '..', 'firmware', 'src')

def parse_array(text, name):
    m = re.search(name + r'\[\]\s*=\s*\{(.*?)\};', text, re.S)
    body = re.sub(r'//.*', '', m.group(1))
    out = []
    for tok in re.findall(r"'.'|0x[0-9a-fA-F]+|\b\d+\b", body):
        out.append(ord(tok[1]) if tok.startswith("'") else int(tok, 0))
    return out

def load_font(data):
    if data[0] == ord('F'):
        first, last, p = data[2], data[3], 5
    else:
        first, last, p = 0, 255, 0
    g = {}
    for c in range(first, last + 1):
        if p >= len(data): break
        w = data[p]; g[c] = data[p + 1:p + 1 + w]; p += 1 + w
    return g

fonts_h = open(os.path.join(SRC, 'fonts_2_7_1.h'), encoding='utf-8').read()
status_c = open(os.path.join(SRC, 'status.cpp'), encoding='utf-8').read()
ICONF = load_font(parse_array(status_c, 'font_fx_status'))
ICONS = [tuple(int(x, 0) for x in t) for t in re.findall(r'\{ (0x[0-9a-f]+), (\d+), (\d+) \}', status_c)]
TEXTS = re.findall(r'\{ (\d), "([^"]*)", "([^"]*)", "([^"]*)" \}', status_c)

def u2l(s):
    b = os.path.join(tempfile.gettempdir(), 'fx_u2l')
    if not os.path.exists(b):
        fn = os.path.join(tempfile.gettempdir(), 'fx_u2l_fn.cpp')
        src = open(os.path.join(SRC, 'text_led.cpp'), encoding='utf-8').read()
        f = re.search(r'^void utf8_to_led.*?^}', src, re.S | re.M).group(0)
        u = open(os.path.join(HERE, 'u2l.cpp'), encoding='utf-8').read()
        st = re.search(r'^struct String.*?^};', u, re.S | re.M).group(0)
        open(fn, 'w', encoding='utf-8').write('#include <cstdint>\n#include <string>\n' + st + '\n' + f + '\n')
        subprocess.run(['c++', '-std=c++17', '-w', '-o', b, os.path.join(HERE, 'u2l.cpp'), fn], check=True)
    return [int(x) for x in subprocess.run([b, s], capture_output=True, text=True).stdout.split()]

def text_cols(codes, font, spacing=1):
    cols = []
    for i, c in enumerate(codes):
        cols += list(font.get(c, []))
        if i != len(codes) - 1: cols += [0] * spacing
    return cols

def main():
    a = sys.argv[1:]
    kind, out = int(a[0]), a[1]
    rows = int(a[a.index('--rows') + 1]) if '--rows' in a else 2
    fname = {'sys0': 'font_sys0_default', 'sys1': 'font_sys1_small', 'sys2': 'font_sys2_fat'}[a[a.index('--font') + 1] if '--font' in a else 'sys2']
    speed = int(a[a.index('--speed') + 1]) if '--speed' in a else 40
    spd = max(10, 0x46 - speed)
    tfont = load_font(parse_array(fonts_h, fname))
    icon_i, uk = int(TEXTS[kind][0]), TEXTS[kind][1]
    first, n, ims = ICONS[icon_i]
    tc = text_cols(u2l(uk), tfont)
    W = 32
    total = len(tc) + W                                  # PA_SCROLL_LEFT: увійти справа й вийти ліворуч
    frames = []
    for k in range(total):
        t = k * spd
        bottom = [0] * W
        for x in range(W):                               # колонка x табло = колонка тексту (k - (W - x))
            j = k - (W - x)
            if 0 <= j < len(tc): bottom[x] = tc[j]
        top = [0] * W
        if rows == 2:
            g = list(ICONF[first + (t // ims) % n]); x0 = (W - len(g)) // 2
            for i, c in enumerate(g): top[x0 + i] = c
        frames.append((top, bottom))
    write_gif(out, frames, rows, spd)
    write_strip(os.path.splitext(out)[0] + '.png', frames[::max(1, len(frames) // 8)][:8], rows)
    print(f'{out}: {len(frames)} кадрів по {spd} мс; текст «{uk}», {len(tc)} колонок')

S, R = 10, 4      # клітинка й радіус світлодіода
def raster(top, bottom, rows):
    gap = 6
    W = 32; H = 8 * rows * S + (gap if rows == 2 else 0) + 2 * S
    img = [[0] * (W * S + 2 * S) for _ in range(H)]
    lines = [top, bottom] if rows == 2 else [bottom]
    for ri, cols in enumerate(lines):
        oy = S + ri * (8 * S + gap)
        for x in range(W):
            for y in range(8):
                on = (cols[x] >> y) & 1
                cx, cy = S + x * S + S // 2, oy + y * S + S // 2
                for yy in range(cy - R, cy + R + 1):
                    for xx in range(cx - R, cx + R + 1):
                        if (xx - cx) ** 2 + (yy - cy) ** 2 <= R * R + 1:
                            img[yy][xx] = 2 if on else 1
    return img

PAL = [(14, 14, 14), (48, 20, 16), (255, 72, 32), (0, 0, 0)]
def lzw(data, mcs=2):
    clear, end = 1 << mcs, (1 << mcs) + 1
    dic = {(i,): i for i in range(clear)}
    nxt, width = end + 1, mcs + 1
    out, acc, nb = bytearray(), 0, 0
    def emit(code):
        nonlocal acc, nb
        acc |= code << nb; nb += width
        while nb >= 8: out.append(acc & 255); acc >>= 8; nb -= 8
    emit(clear)
    w = ()
    for px in data:
        wc = w + (px,)
        if wc in dic: w = wc; continue
        emit(dic[w])
        if nxt < 4096:
            dic[wc] = nxt; nxt += 1
            if nxt > (1 << width) and width < 12: width += 1
        else:
            emit(clear); dic = {(i,): i for i in range(clear)}; nxt, width = end + 1, mcs + 1
        w = (px,)
    if w: emit(dic[w])
    emit(end)
    if nb: out.append(acc & 255)
    return bytes(out)

def write_gif(path, frames, rows, spd):
    imgs = [raster(t, b, rows) for t, b in frames]
    h, w = len(imgs[0]), len(imgs[0][0])
    o = bytearray(b'GIF89a' + struct.pack('<HHBBB', w, h, 0xf1, 0, 0))
    for c in PAL: o += bytes(c)
    o += b'\x21\xff\x0bNETSCAPE2.0\x03\x01\x00\x00\x00'
    for im in imgs:
        o += b'\x21\xf9\x04\x00' + struct.pack('<H', max(2, round(spd / 10))) + b'\x00\x00'
        o += b'\x2c' + struct.pack('<HHHHB', 0, 0, w, h, 0)
        data = lzw([p for row in im for p in row])
        o += b'\x02'
        for i in range(0, len(data), 255):
            blk = data[i:i + 255]; o += bytes([len(blk)]) + blk
        o += b'\x00'
    o += b'\x3b'
    open(path, 'wb').write(o)

def write_strip(path, frames, rows):
    imgs = [raster(t, b, rows) for t, b in frames]
    h, w = len(imgs[0]), len(imgs[0][0])
    raw = b''
    for im in imgs:
        for row in im: raw += b'\x00' + bytes(v for p in row for v in PAL[p])
        raw += (b'\x00' + bytes(PAL[3]) * w) * 4
    H = len(imgs) * (h + 4)
    def chunk(t, d): return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
    open(path, 'wb').write(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, H, 8, 2, 0, 0, 0)) +
                           chunk(b'IDAT', zlib.compress(raw, 9)) + chunk(b'IEND', b''))

main()
