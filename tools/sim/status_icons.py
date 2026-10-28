#!/usr/bin/env python3
"""status_icons.py — кадри значків стану запуску для табло (шрифт MD_MAX72XX формату 'F' v1).
Друкує C-масив для src/status.cpp і (з --png файл) малює всі кадри збільшено для перевірки оком.
Кадри: спінер «завантаження» (8), годинник зі стрілкою (8), глобус, що обертається (4), стрілка в лоток (4)."""
import sys

W, H = 7, 8

def blank():
    return [[0] * W for _ in range(H)]

def put(g, pts):
    for x, y in pts:
        if 0 <= x < W and 0 <= y < H:
            g[y][x] = 1
    return g

RING = [(3, 0), (5, 1), (6, 3), (5, 5), (3, 6), (1, 5), (0, 3), (1, 1)]   # 8 точок по колу, за годинниковою
CIRCLE = [(2, 0), (3, 0), (4, 0), (1, 1), (5, 1), (0, 2), (6, 2), (0, 3), (6, 3), (0, 4), (6, 4), (1, 5), (5, 5),
          (2, 6), (3, 6), (4, 6)]                                          # коло 7×7, центр (3,3)
HAND = [[(3, 2), (3, 1)], [(4, 2), (5, 1)], [(4, 3), (5, 3)], [(4, 4), (5, 5)],
        [(3, 4), (3, 5)], [(2, 4), (1, 5)], [(2, 3), (1, 3)], [(2, 2), (1, 1)]]

frames = []
# коло по годинниковій стрілці від верхньої точки (16 пікселів кола 7×7)
CW = [(3, 0), (4, 0), (5, 1), (6, 2), (6, 3), (6, 4), (5, 5), (4, 6), (3, 6), (2, 6), (1, 5), (0, 4), (0, 3), (0, 2),
      (1, 1), (2, 0)]
# спінер: дуга з 6 пікселів біжить по колу (крок 2 пікселі)
for k in range(8):
    frames.append(('спінер', put(blank(), [CW[(2 * k - i) % 16] for i in range(6)])))
# годинник: коло, центр і хвилинна стрілка, що обертається
for k in range(8):
    frames.append(('годинник', put(blank(), CIRCLE + [(3, 3)] + HAND[k])))
# глобус: коло, екватор і меридіан-еліпс, що повертається (лінія → вузький → широкий → вузький)
for d in (0, 1, 2, 1):
    g = put(blank(), CIRCLE + [(x, 3) for x in range(7)])
    if d == 0: put(g, [(3, y) for y in range(1, 6)])
    elif d == 1: put(g, [(2, 1), (2, 2), (2, 4), (2, 5), (4, 1), (4, 2), (4, 4), (4, 5)])
    else: put(g, [(1, 2), (1, 4), (5, 2), (5, 4), (2, 1), (4, 1), (2, 5), (4, 5)])
    frames.append(('глобус', g))
# стрілка вниз у лоток: лоток стоїть, стрілка з гострим вістрям опускається (вістря в рядках 3..6)
for t in (3, 4, 5, 6):
    g = put(blank(), [(0, 5), (0, 6), (6, 5), (6, 6)] + [(x, 7) for x in range(7)])
    arrow = [(3, t), (2, t - 1), (4, t - 1), (1, t - 2), (5, t - 2)] + [(3, y) for y in range(t - 4, t)]
    put(g, [(x, y) for x, y in arrow if 0 <= y < 7])
    frames.append(('завантаження', g))

def cols(g):
    return [sum(g[y][x] << y for y in range(H)) for x in range(W)]

if '--png' in sys.argv:
    import zlib, struct
    out = sys.argv[sys.argv.index('--png') + 1]
    S, P = 22, 10
    kinds = []
    for name, _ in frames:
        if name not in kinds: kinds.append(name)
    per = max(sum(1 for n, _ in frames if n == k) for k in kinds)
    iw, ih = per * (W * S + P) + P, len(kinds) * (H * S + P) + P
    px = [[(20, 20, 20)] * iw for _ in range(ih)]
    col = {k: 0 for k in kinds}
    for i, (name, g) in enumerate(frames):
        ox = P + col[name] * (W * S + P); col[name] += 1
        oy = P + kinds.index(name) * (H * S + P)
        for y in range(H):
            for x in range(W):
                c = (255, 60, 30) if g[y][x] else (60, 28, 22)
                cx, cy, r = ox + x * S + S / 2, oy + y * S + S / 2, S / 2 - 2
                for yy in range(int(cy - r) - 1, int(cy + r) + 2):
                    for xx in range(int(cx - r) - 1, int(cx + r) + 2):
                        if (xx + .5 - cx) ** 2 + (yy + .5 - cy) ** 2 <= r * r:
                            px[yy][xx] = c
    raw = b''.join(b'\x00' + bytes(v for p_ in row for v in p_) for row in px)
    def chunk(t, d): return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xffffffff)
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', iw, ih, 8, 2, 0, 0, 0)) + \
          chunk(b'IDAT', zlib.compress(raw, 9)) + chunk(b'IEND', b'')
    open(out, 'wb').write(png)
    print('png:', out)
else:
    print("static const uint8_t PROGMEM font_fx_status[] = {")
    print(f"  'F', 1, 0x01, {len(frames):#04x}, 8,")
    for i, (name, g) in enumerate(frames):
        print(f"  {W}, " + ', '.join(f'{c:#04x}' for c in cols(g)) + f",  // {i + 1:#04x} {name}")
    print("};")
    for name, g in frames[:0]:
        pass
