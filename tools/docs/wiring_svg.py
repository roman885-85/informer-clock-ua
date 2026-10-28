#!/usr/bin/env python3
"""wiring_svg.py — схема підключення Informer Clock UA для ESP32 DevKit V1 (30 виводів) → docs/images/wiring-esp32-<мова>.svg.
Стиль «мітки кіл»: біля кожного задіяного виводу плати — кольорова мітка (куди він іде), модулі — картками з тими самими
кольорами (без перехрещень проводів). Виводи — з коду прошивки (firmware/src).
EN: wiring diagram generator (net-label style); pins are taken from the firmware source.
Run: python3 tools/docs/wiring_svg.py uk|en"""
import os, sys

L = sys.argv[1] if len(sys.argv) > 1 else 'uk'
U = L == 'uk'
def tr(uk, en): return uk if U else en

# кола: колір, мітка біля виводу
NET = {
    'DIN': ('#8e44ad', 'DIN · MAX7219'), 'CLK': ('#8e44ad', 'CLK · MAX7219'), 'CS': ('#8e44ad', 'CS · MAX7219'),
    'SDA': ('#1d70b8', tr('SDA · датчики I²C', 'SDA · I²C sensors')), 'SCL': ('#2b9348', tr('SCL · датчики I²C', 'SCL · I²C sensors')),
    'DFRX': ('#e09f3e', tr('→ RX DFPlayer (1 кОм)', '→ RX DFPlayer (1 kΩ)')), 'DFTX': ('#e09f3e', '← TX DFPlayer'),
    'MHRX': ('#c77d00', '→ RX MH-Z19B'), 'MHTX': ('#c77d00', '← TX MH-Z19B'),
    'BUZ': ('#b5838d', tr('зумер/LED (через C945)', 'buzzer/LED (via C945)')),
    'PHOTO': ('#2a9d8f', tr('фоторезистор', 'photoresistor')), 'DS': ('#2a9d8f', 'DS18B20'),
    'LED': ('#3a86ff', tr('WS2812B (через 220 Ом)', 'WS2812B (via 220 Ω)')), 'DHT': ('#2a9d8f', 'DHT11/22'),
    '5V': ('#d62828', tr('5 В (живлення)', '5 V (power in)')), 'GND': ('#333333', 'GND'), '3V3': ('#e76f51', tr('3,3 В → датчики', '3.3 V → sensors')),
}
LEFT = [('EN', None), ('VP·36', None), ('VN·39', None), ('D34', 'PHOTO'), ('D35', None), ('D32', None), ('D33', None),
        ('D25', None), ('D26', 'DS'), ('D27', 'LED'), ('D14', 'DHT'), ('D12', None), ('D13', None), ('GND', 'GND'), ('VIN', '5V')]
RIGHT = [('D23', 'DIN'), ('D22', 'SCL'), ('TX0', None), ('RX0', None), ('D21', 'SDA'), ('D19', None), ('D18', 'CLK'),
         ('D5', 'CS'), ('TX2·17', 'DFRX'), ('RX2·16', 'DFTX'), ('D4', 'MHRX'), ('D2', 'BUZ'), ('D15', 'MHTX'), ('GND', 'GND'), ('3V3', '3V3')]

W, H = 1500, 1010
BX, BY, BW, PH = 640, 120, 220, 34
out = []
def add(s): out.append(s)
def esc(s): return s.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')
def text(x, y, s, size=14, anchor='start', weight='normal', fill='var(--fg)'):
    add(f'<text x="{x}" y="{y}" font-size="{size}" text-anchor="{anchor}" font-weight="{weight}" fill="{fill}">{esc(s)}</text>')
def tag(x, y, net, side):
    c, s = NET[net]
    w = 11 + 7.4 * len(s)
    x0 = x if side == 'r' else x - w
    add(f'<line x1="{x - (0 if side == "r" else 0)}" y1="{y}" x2="{x0 if side == "r" else x0 + w}" y2="{y}" stroke="{c}" stroke-width="3"/>')
    add(f'<rect x="{x0}" y="{y - 11}" width="{w}" height="22" rx="11" fill="{c}"/>')
    text(x0 + w / 2, y + 5, s, 12.5, 'middle', 'bold', '#ffffff')

add(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" width="{W}" height="{H}" font-family="Helvetica, Arial, sans-serif">')
add('<style>:root{--bg:#ffffff;--fg:#1d1d1f;--muted:#555;--card:#f7f7f9;--line:#d0d0d6;--board:#1f3b57}'
    '@media (prefers-color-scheme: dark){:root{--bg:#0d1117;--fg:#e6edf3;--muted:#9aa4ae;--card:#161b22;--line:#30363d;--board:#24476b}}</style>')
add(f'<rect width="{W}" height="{H}" fill="var(--bg)"/>')
text(W / 2, 42, tr('Informer Clock UA — схема підключення', 'Informer Clock UA — wiring diagram'), 24, 'middle', 'bold')
text(W / 2, 68, tr('ESP32 DevKit V1 (30 виводів). Мітка біля виводу — куди він підключається; кольори — як у картках модулів нижче.',
                   'ESP32 DevKit V1 (30 pins). The label next to a pin shows where it goes; colours match the module cards below.'),
     14, 'middle', fill='var(--muted)')
# плата з виводами
bh = len(LEFT) * PH + 30
add(f'<rect x="{BX}" y="{BY}" width="{BW}" height="{bh}" rx="14" fill="var(--board)"/>')
add(f'<rect x="{BX + 60}" y="{BY + 14}" width="{BW - 120}" height="70" rx="6" fill="#9aa7b4" opacity="0.45"/>')
text(BX + BW / 2, BY + 55, 'ESP32-WROOM-32', 13, 'middle', 'bold', '#ffffff')
text(BX + BW / 2, BY + bh - 14, 'USB', 13, 'middle', 'bold', '#ffffff')
for i, (n, net) in enumerate(LEFT):
    y = BY + 110 + i * PH * 0.82
    add(f'<rect x="{BX - 9}" y="{y - 7}" width="18" height="14" rx="3" fill="#c9a227"/>')
    text(BX + 16, y + 5, n, 13, fill='#ffffff')
    if net: tag(BX - 9, y, net, 'l')
for i, (n, net) in enumerate(RIGHT):
    y = BY + 110 + i * PH * 0.82
    add(f'<rect x="{BX + BW - 9}" y="{y - 7}" width="18" height="14" rx="3" fill="#c9a227"/>')
    text(BX + BW - 16, y + 5, n, 13, 'end', fill='#ffffff')
    if net: tag(BX + BW + 9, y, net, 'r')

# картки модулів
cards = [
    ('#8e44ad', tr('Табло MAX7219 (FC16): 4–16 модулів 8×8', 'MAX7219 display (FC16): 4–16 modules 8×8'),
     [tr('VCC → 5 В   GND → GND', 'VCC → 5 V   GND → GND'), 'DIN → GPIO23   CLK → GPIO18   CS → GPIO5',
      tr('2 поверхи по 4 або 8: DIN 2-го ← DOUT 1-го; CLK, CS, 5 В — паралельно', '2 floors of 4 or 8: DIN of 2nd ← DOUT of 1st; CLK, CS, 5 V in parallel')]),
    ('#d62828', tr('Живлення 5 В, 2 А', 'Power 5 V, 2 A'),
     [tr('5 В → VIN плати, табло, стрічка, DFPlayer', '5 V → board VIN, display, strip, DFPlayer'),
      tr('1000 мкФ 6,3 В між 5 В і GND — на вході', '1000 µF 6.3 V across 5 V and GND at the input'),
      tr('усі GND — разом; датчики — від 3,3 В плати', 'all GND common; sensors from the board 3.3 V')]),
    ('#2a9d8f', tr('Фоторезистор 5528 (яскравість)', 'Photoresistor 5528 (brightness)'),
     [tr('3,3 В → фоторезистор → GPIO34', '3.3 V → photoresistor → GPIO34'), tr('GPIO34 → 10 кОм → GND', 'GPIO34 → 10 kΩ → GND')]),
    ('#3a86ff', tr('Стрічка WS2812B (до 30 світлодіодів)', 'WS2812B strip (up to 30 LEDs)'),
     [tr('DIN ← 220 Ом ← GPIO27; +5 В, GND', 'DIN ← 220 Ω ← GPIO27; +5 V, GND'), tr('кількість задається в «LED підсвічування»', 'the count is set on the “LED Light” page')]),
    ('#1d70b8', tr('Датчики I²C (необов.)', 'I²C sensors (optional)'),
     [tr('SDA → GPIO21   SCL → GPIO22   VCC → 3,3 В', 'SDA → GPIO21   SCL → GPIO22   VCC → 3.3 V'), 'RTC DS3231/DS1307, BME280/BMP280/BMP180,', 'AHT10, HDC1080, AM2320, SGP30, CCS811, SCD40/41']),
    ('#2a9d8f', tr('DS18B20 · DHT11/DHT22 (необов.)', 'DS18B20 · DHT11/DHT22 (optional)'),
     [tr('DS18B20: дані → GPIO26, 4,7–10 кОм до 3,3 В', 'DS18B20: data → GPIO26, 4.7–10 kΩ to 3.3 V'), tr('DHT: дані → GPIO14, 10 кОм до 3,3 В', 'DHT: data → GPIO14, 10 kΩ to 3.3 V')]),
    ('#e09f3e', tr('DFPlayer Mini + динамік (голос, будильник)', 'DFPlayer Mini + speaker (voice, alarm)'),
     [tr('VCC → 5 В; RX ← 1 кОм ← GPIO17; TX → GPIO16', 'VCC → 5 V; RX ← 1 kΩ ← GPIO17; TX → GPIO16'),
      tr('SPK1/SPK2 → динамік 3 Вт; файли — на microSD', 'SPK1/SPK2 → 3 W speaker; files on microSD')]),
    ('#c77d00', tr('MH-Z19B, CO₂ (необов.)', 'MH-Z19B, CO₂ (optional)'),
     [tr('VIN → 5 В; TX → GPIO15; RX ← GPIO4', 'VIN → 5 V; TX → GPIO15; RX ← GPIO4'),
      tr('разом із DFPlayer не працює (спільний UART1)', 'not together with DFPlayer (shared UART1)')]),
    ('#b5838d', tr('Зумер і світлодіод (необов.)', 'Buzzer and LED (optional)'),
     [tr('GPIO2 → 220 Ом → база C945 (NPN), емітер → GND', 'GPIO2 → 220 Ω → C945 base (NPN), emitter → GND'),
      tr('зумер і LED (з 220 Ом) — між 5 В і колектором', 'buzzer and LED (with 220 Ω) — between 5 V and collector')]),
    ('#888888', tr('Кнопка', 'Button'),
     [tr('GPIO0 — це кнопка BOOT на платі', 'GPIO0 is the BOOT button on the board'),
      tr('зовнішня — паралельно BOOT: GPIO0 ↔ GND', 'external one in parallel to BOOT: GPIO0 ↔ GND')]),
]
cw, ch, gx, gy = 470, 108, 18, 16
x0, y0 = (W - 3 * cw - 2 * gx) / 2, 690
for k, (c, title, lines) in enumerate(cards):
    col, row = k % 3, k // 3
    if k == 9: col, row = 1, 3
    x, y = x0 + col * (cw + gx), y0 + row * (ch + gy) - (0 if row < 3 else 0)
    if row >= 2: y = y0 + 2 * (ch + gy) + (row - 2) * (ch + gy)
    add(f'<rect x="{x}" y="{y}" width="{cw}" height="{ch}" rx="12" fill="var(--card)" stroke="var(--line)"/>')
    add(f'<rect x="{x}" y="{y}" width="8" height="{ch}" rx="4" fill="{c}"/>')
    text(x + 22, y + 26, title, 15, weight='bold')
    for i, s in enumerate(lines): text(x + 22, y + 50 + i * 19, s, 13, fill='var(--muted)')
text(W / 2, H - 12, tr('Виводи — з коду прошивки (firmware/src). Розташування модулів у корпусі — на оригінальній схемі автора: docs/images/original-esp32-schematic.png',
                      'Pins come from the firmware source (firmware/src). Physical layout — see the original schematic by the author: docs/images/original-esp32-schematic.png'),
     12.5, 'middle', fill='var(--muted)')
add('</svg>')
# висота: 4 ряди карток
rows = 4
need = y0 + rows * (ch + gy) + 30
svg = '\n'.join(out).replace(f'viewBox="0 0 {W} {H}" width="{W}" height="{H}"', f'viewBox="0 0 {W} {int(need)}" width="{W}" height="{int(need)}"')
svg = svg.replace(f'<rect width="{W}" height="{H}"', f'<rect width="{W}" height="{int(need)}"').replace(f'y="{H - 12}"', f'y="{int(need) - 12}"')
dst = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'docs', 'images', f'wiring-esp32-{L}.svg')
open(dst, 'w', encoding='utf-8').write(svg)
print(os.path.normpath(dst), int(need))
