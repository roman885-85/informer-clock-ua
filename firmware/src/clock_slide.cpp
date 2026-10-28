// Годинник «зі зсувом цифр» (An=1): цифри малюються стовпцями через MD_MAX72XX::setColumn і «виїжджають» по пікселю.
// Перенесено з 2.7.1. Цифра — 16 байт: [0] поточний символ, [1] новий, [2] крок 0..8, [4..7] час кроку, [8] ширина, [9..] стовпці.
#include "fw.h"
#define DT(a, i) (*(uint32_t *)&(a)[(i) * 16 + 4])

// @400d7fe0: шрифт циферблата для MD_MAX72XX за font_l
void clock_font_mx() {
  const uint8_t *f;
  switch (g_c349c) {
    default: f = font_clock0_default; break;
    case 1: f = font_clock1_fat; break;
    case 2: f = font_clock2_digi_small; break;
    case 3: f = font_clock3_digi_big; break;
    case 4: f = font_clock4_cubic; break;
    case 5: f = font_clock5_diamond_sec; break;
    case 6: f = font_clock6_phase_shifter; break;
    case 7: f = font_clock7_ink_script; break;
    case 8: f = font_clock8_ink_stamp; break;
    case 9: f = font_clock9_street_doc; break;
    case 10: f = font_clock10_stars; break;
    case 11: f = font_clock11_stars_square; break;
    case 12: f = font_clock12_sorcery; break;
  }
  g_c34bc->setFont((MD_MAX72XX::fontType_t *)f);
}

// @400d8050: намалювати n цифр зі стовпця col
void slide_draw(int8_t n, uint8_t *dg, int8_t col) {
  g_c34bc->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  while (true) {
    n = n - 1;
    if (n < 0) break;
    uint8_t *d = dg + n * 16;
    int8_t c1 = (int8_t)d[8];
    int8_t c3 = c1 - 1;
    int8_t c4;
    while (true) {
      c1 = c1 - 1;
      c4 = (int8_t)((col + c3) - c1);
      if (c1 < 0) break;
      g_c34bc->setColumn(c4, d[c1 + 9]);
    }
    col = c4 + 1;
  }
  g_c34bc->control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

// спільний крок «виїзду» для двох цифр (секунди/хвилини)
static void slide_step2(uint8_t *D, int8_t first, int8_t stopAt, uint32_t now, int codeShift) {
  uint8_t *pb = D + first * 16;
  int8_t cnt = (first == 1 && stopAt == -1) ? 1 : 2;
  (void)cnt;
  for (int8_t k = first; k != stopAt; k--, pb -= 16) {
    uint8_t nx = pb[1];
    if (pb[0] != nx) {
      uint8_t buf[6]; memset(buf, 0, 6);
      g_c34bc->getChar(nx - codeShift, 6, buf);
      uint8_t w = pb[8];
      uint8_t *src = buf, *dst = pb;
      for (uint8_t j = 0; j != w; j++) {
        uint8_t old = dst[9];
        uint8_t b = (uint8_t)((int)(uint32_t)*src >> ((7 - pb[2]) & 0x1f));
        *src = b;
        dst[9] = b | (uint8_t)(old << 1);
        src++; dst++;
      }
      uint8_t s = pb[2];
      *(uint32_t *)(pb + 4) = now;
      pb[2] = s + 1;
      if (7 < (uint8_t)(s + 1)) pb[0] = pb[1];
    }
  }
}

// @400d80bc: секунди (гліфи 0x14..0x1d), крок 13 мс
void slide_seconds(uint32_t v, uint8_t col) {
  clock_font_mx();
  uint32_t now = millis();
  v &= 0xffff;
  uint8_t *D = g_c1938;
  if (g_c34a2 != 1) {
    if (g_c34a2 == 0) {
      D[16] = (char)(v % 10) + '0';
      D[0] = (char)(v / 10) - (char)((v / 10) / 10) * 10 + 0x30;
      D[24] = g_c34bc->getChar(v % 10 + 0x14, 6, &D[25]);
      D[8] = g_c34bc->getChar(D[0] - 0x1c, 6, &D[9]);
      slide_draw(2, D, col);
      g_c34a2 = 1;
      return;
    }
    if (g_c34a2 != 2) { g_c34a2 = 0; return; }
    if (g_c2c08 + 0xd <= now) {
      g_c2c08 = now;
      slide_step2(D, 1, -1, now, 0x1c);
      slide_draw(2, D, col);
      bool eq = true;
      for (uint32_t i = 0; (1 < i) < eq; i = (i + 1) & 0xff) eq = D[i * 16] == D[i * 16 + 1];
      if (eq) { g_c2c0c = 1; g_c34a2 = 1; }
      return;
    }
    goto tail;
  }
  {
    uint32_t u = v % 10 + 0x30;
    D[17] = (uint8_t)u;
    int8_t st = g_c34a2;
    if (D[16] != u) { D[18] = 0; DT(D, 1) = 0; st = 2; }
    uint32_t t = (v / 10) % 10 + 0x30;
    D[1] = (uint8_t)t;
    if (D[0] == t) {
      if (D[16] != u) g_c34a2 = st;
    } else {
      D[2] = 0; DT(D, 0) = 0; st = 2;
      g_c34a2 = st;
    }
    if (g_c34a2 != 1) return;
  }
tail:
  if (g_c2c0e == 0) return;
  slide_draw(2, D, col);
}

// @400d82d4: хвилини (звичайні цифри), крок 45 мс
void slide_minutes(uint32_t v, uint8_t col) {
  clock_font_mx();
  uint32_t now = millis();
  v &= 0xffff;
  uint8_t *D = g_c1918;
  if (g_c34a1 != 1) {
    if (g_c34a1 == 0) {
      int u = v % 10 + 0x30;
      D[0] = (char)(v / 10) - (char)((v / 10) / 10) * 10 + '0';
      D[16] = (uint8_t)u;
      D[24] = g_c34bc->getChar(u, 6, &D[25]);
      D[8] = g_c34bc->getChar(D[0], 6, &D[9]);
      slide_draw(2, D, col);
      g_c34a1 = 1;
      return;
    }
    if (g_c34a1 != 2) { g_c34a1 = 0; return; }
    if (g_c2c04 + 0x2d <= now) {
      g_c2c04 = now;
      slide_step2(D, 1, -1, now, 0);
      slide_draw(2, D, col);
      bool eq = true;
      for (uint32_t i = 0; (1 < i) < eq; i = (i + 1) & 0xff) eq = D[i * 16] == D[i * 16 + 1];
      if (eq) g_c34a1 = 1;
      return;
    }
    goto tail;
  }
  {
    uint32_t u = v % 10 + 0x30;
    D[17] = (uint8_t)u;
    int8_t st = g_c34a1;
    if (D[16] != u) { D[18] = 0; DT(D, 1) = 0; st = 2; }
    uint32_t t = (v / 10) % 10 + 0x30;
    D[1] = (uint8_t)t;
    if (D[0] == t) {
      if (D[16] != u) g_c34a1 = st;
    } else {
      D[2] = 0; DT(D, 0) = 0; st = 2;
      g_c34a1 = st;
    }
    if (g_c34a1 != 1) return;
  }
tail:
  if (g_c2c0e == 0) return;
  slide_draw(2, D, col);
}

// @400e205c: година однозначна при nz (без нуля попереду)
bool hour_single_digit() {
  int h = hour();
  bool r = false;
  if (g_c34ab != 0) {
    uint32_t t = h / 10;
    r = (t & 0xff) == 0;
    if ((uint32_t)g_c1740 != (t & 0xff)) { g_c1740 = (uint8_t)t; g_c2c00 = 3; }
  }
  return r;
}

// @400e20a4: години (1 або 2 цифри), крок 60 мс
void slide_hours(uint32_t param_1, uint8_t param_2) {
  uint8_t *H = g_c18f4;
  bool ch; int8_t cVar13; uint32_t v; uint8_t *p15;
  clock_font_mx();
  uint32_t now = millis();
  int8_t n = hour_single_digit() ? 1 : 2;
  if (g_c1914 == 1) {
    if (n == 2) {
      memcpy(&H[16], &H[0], 16);
      H[12] = 0; H[13] = 0; H[14] = 0;
      H[0] = 1;
      H[9] = 0; H[10] = 0; H[11] = 0;
      g_c34a0 = g_c1914;
    }
  } else if (g_c1914 == 2 && n == 1) {
    g_c2c0d = 1;
    g_c34a0 = 1;
  }
  int8_t cVar9 = g_c2c0d;
  g_c1915 = n;
  if (g_c2c0d != 0) g_c1915 = 2;
  int8_t cVar16 = g_c1915;
  g_c1914 = n;
  int8_t cVar7;
  if (g_c34a0 == 1) {
    ch = false;
    cVar13 = g_c1915 - 1;
    v = param_1 & 0xffff;
    cVar7 = g_c34a0;
    for (int8_t k = cVar13; -1 < k; k--) {
      int o = k * 16;
      uint32_t d = v % 10 + 0x30;
      H[o + 1] = (uint8_t)d;
      if (H[o] != d) { H[o + 2] = 0; DT(H, k) = 0; ch = true; cVar7 = 2; }
      v = v / 10;
    }
    if (ch) {
      g_c34a0 = cVar7;
      if (cVar9 != 0) goto L2218;
L2255:
      if (g_c34a0 != 1) goto L226c;
    } else if (cVar9 != 0) {
L2218:
      ch = false;
      H[1] = 1;
      cVar7 = g_c34a0;
      for (; -1 < cVar13; cVar13--) {
        int o = cVar13 * 16;
        if (H[o + 1] != H[o]) { H[o + 2] = 0; ch = true; cVar7 = 2; }
      }
      if (ch) g_c34a0 = cVar7;
      goto L2255;
    }
    if (g_c2c0e != 0) {
L228c:
      slide_draw(cVar16, H, param_2);
      return;
    }
  } else {
    if (g_c34a0 == 0) {
      int8_t c9 = g_c1915 - 1;
      v = param_1 & 0xffff;
      for (int8_t k = c9; -1 < k; k--) { H[k * 16] = (char)(v % 10) + '0'; v = v / 10; }
      for (; -1 < c9; c9--) { int o = c9 * 16; H[o + 8] = g_c34bc->getChar(H[o], 6, &H[o + 9]); }
      slide_draw(g_c1915, H, param_2);
      g_c34a0 = 1;
      return;
    }
    if (g_c34a0 != 2) { g_c34a0 = 0; return; }
L226c:
    if (now < g_c2bfc + 0x3c) {
      if (g_c2c0e != 0) { cVar16 = g_c1915; goto L228c; }
    } else {
      p15 = H;
      g_c2bfc = now;
      cVar7 = g_c1915;
      while (true) {
        cVar7 = cVar7 - 1;
        int k = cVar7;
        if (k < 0) break;
        int o = k * 16;
        int8_t nx = (int8_t)H[o + 1];
        if ((int8_t)H[o] != nx) {
          uint8_t buf[6]; memset(buf, 0, 6);
          if (nx != 1) g_c34bc->getChar(nx, 6, buf);
          int8_t w = (int8_t)H[o + 8];
          uint8_t *src = buf, *dst = &H[o];
          for (int8_t j = 0; j != w; j++) {
            uint8_t old = dst[9];
            uint8_t b = (uint8_t)((int)(uint32_t)*src >> ((7 - H[k * 16 + 2]) & 0x1f));
            *src = b;
            dst[9] = b | (uint8_t)(old << 1);
            src++; dst++;
          }
          uint8_t s = H[o + 2] + 1;
          H[o + 2] = s;
          DT(H, k) = now;
          if (7 < s) H[o] = H[o + 1];
        }
      }
      slide_draw(g_c1915, H, param_2);
      cVar7 = 0;
      while (true) {
        if (g_c1915 == cVar7) { g_c34a0 = 1; break; }
        int8_t a = (int8_t)p15[0];
        int8_t b = (int8_t)p15[1];
        cVar7++;
        p15 += 16;
        if (a != b) break;
      }
      if (g_c2c0d != 0 && H[0] == 1 && H[16] == H[17]) {
        g_c2c0d = 0;
        g_c1915 = 1;
        memcpy(&H[0], &H[16], 16);
        g_c34a0 = 1;
        return;
      }
    }
  }
}

// @400e23c4: плавне зміщення циферблата, коли година стає однозначною/двозначною
int8_t hour_shift(int8_t p1, uint8_t p2, uint8_t p3) {
  if (!hour_single_digit()) p1 = 0;
  if (g_bebec < 0) g_bebec = p1;
  if (g_c2c0d == 0) {
    if (g_bebec == p1) g_c2c0e = g_c2c0d;
    else {
      g_c2c0e = 1;
      if (millis() - g_c173c > 0x31) {
        g_c173c = millis();
        int8_t d = 1;
        if (p1 <= g_bebec) d = -1;
        g_bebec = d + g_bebec;
        g_c34bc->clear(p2, p3);
      }
    }
  }
  return g_bebec;
}
// @400e2454
int8_t hour_shift_all(int8_t p) { return hour_shift(p, 0, g_c34b9 - 1); }

// @400fc8d8: двокрапка (і друга — перед секундами) стовпцями за pAn і фазою
void slide_colon_draw(uint32_t c1, uint32_t c2, uint8_t ph) {
  uint32_t col; uint8_t val;
  c1 &= 0xff; c2 &= 0xff;
  switch (g_c2eed) {
    case 0: case 1:
      if (ph == 0) {
        if (g_c349c < 6 && ((1 << (g_c349c & 0x1f)) & 0x2d) != 0) {
          if (c1 != 200) g_c34bc->setColumn(c1, 0x24);
          val = 0x24; col = c2;
          goto L21;
        }
        if (c1 != 200) {
L943:
          g_c34bc->setColumn(c1, 0x36);
          g_c34bc->setColumn(c1 + 1, 0x36);
        }
        g_c34bc->setColumn(c2, 0x36);
        val = 0x36;
      } else {
        if (ph != 1) goto L29;
        if (c1 != 200) { g_c34bc->setColumn(c1, 0); g_c34bc->setColumn(c1 + 1, 0); }
        g_c34bc->setColumn(c2, 0);
        val = 0;
      }
      break;
    case 2:
      if (5 < g_c349c || ((1 << (g_c349c & 0x1f)) & 0x2d) == 0) {
        if (ph == 1) {
          g_c34bc->setColumn(c1, 0x14); g_c34bc->setColumn(c1 + 1, 0x22); g_c34bc->setColumn(c2, 0x14);
          val = 0x22;
        } else {
          if (ph == 0 || ph == 2) goto L943;
          if (ph != 3) goto L29;
          g_c34bc->setColumn(c1, 0x22); g_c34bc->setColumn(c1 + 1, 0x14); g_c34bc->setColumn(c2, 0x22);
          val = 0x14;
        }
        break;
      }
      config_set("pAn", "0");
LB8C:
      g_c2d28 = 1;
      config_save_atomic();
      g_c2eed = 0;
      goto L29;
    case 3:
      if (g_c349c < 6 && ((1 << (g_c349c & 0x1f)) & 0x2d) != 0) { config_set("pAn", "0"); goto LB8C; }
      if (5 < ph) goto L29;
      col = c2 + 1;
      switch (ph) {
        default:
          if (c1 != 200) { g_c34bc->setColumn(c1, 0x24); g_c34bc->setColumn(c1 + 1, 0x36); }
          val = 0x24; break;
        case 1:
          if (c1 != 200) { g_c34bc->setColumn(c1, 0x36); g_c34bc->setColumn(c1 + 1, 0x24); }
          g_c34bc->setColumn(c2, 0x36); val = 0x24; goto L21;
        case 2:
          if (c1 != 200) { g_c34bc->setColumn(c1, 0x36); g_c34bc->setColumn(c1 + 1, 0x12); }
          g_c34bc->setColumn(c2, 0x36); val = 0x12; goto L21;
        case 3:
          if (c1 != 200) { g_c34bc->setColumn(c1, 0x12); g_c34bc->setColumn(c1 + 1, 0x36); }
          val = 0x12; break;
        case 5:
          if (c1 != 200) { g_c34bc->setColumn(c1, 0x36); g_c34bc->setColumn(c1 + 1, 0x36); }
          val = 0x36; break;
      }
      g_c34bc->setColumn(c2, val);
      val = 0x36;
      goto L21;
    case 4:
      if (g_c349c < 6 && ((1 << (g_c349c & 0x1f)) & 0x2d) != 0) { config_set("pAn", "0"); goto LB8C; }
      if (ph == 0) {
        if (c1 != 200) { g_c34bc->setColumn(c1, 6); g_c34bc->setColumn(c1 + 1, 0x30); }
        g_c34bc->setColumn(c2, 6); val = 0x30;
      } else {
        if (ph != 1) goto L29;
        if (c1 != 200) { g_c34bc->setColumn(c1, 0x30); g_c34bc->setColumn(c1 + 1, 6); }
        g_c34bc->setColumn(c2, 0x30); val = 6;
      }
      break;
    default:
      return;
  }
  col = c2 + 1;
L21:
  g_c34bc->setColumn(col, val);
L29:
  g_c2c0c = 0;
}

// @400fcc6c: фаза двокрапки годинника зі зсувом
void slide_colon(uint8_t sec, uint8_t c1, uint8_t c2) {
  uint32_t now = millis();
  if (g_c2c0c == 1) {
    if (g_c2eed == 1 || g_c2eed == 4) g_c34ae = sec & 1;
    else g_c34ae = 0;
    g_c18f0 = 0;
    g_c2c0c = 0;
  }
  colon_timing(g_c34ae);
  if ((uint32_t)g_c349e <= now - g_c18f0) {
    g_c18f0 = now;
    slide_colon_draw(c1, c2, g_c34ae);
    if ((uint8_t)(g_c2eed - 2) < 2 || g_c2eed == 0) {
      g_c34ae = g_c34ae + 1;
      if (g_c2ee4 < g_c34ae) g_c34ae = 0;
    }
  }
  if (now - g_c18ec > 999) { g_c34a6 = g_c34a6 + 1; g_c18ec = now; }
  if (g_c2c0e == 0) return;
  slide_colon_draw(c1, c2, g_c34ae);
}

// @400fcd48: годинник зі зсувом цифр — розміщення за типом табло, font_l і c_position
void clock_slide() {
  int8_t sec = second();
  uint32_t mn = minute();
  uint32_t hr = hour();
  uint32_t u6, u7, nm; int i; int8_t s;
  int8_t i4;
  hour_single_digit();
  if (g_c2c00 == 3) g_c2c00 = g_c34ab;
  if (g_c34ba == 8) {
    if (!P.getZoneStatus(1)) return;
    if (1 < P.getZoneCount()) P.setTextEffect(1, PA_PRINT, PA_NO_EFFECT);
    if (g_c1739 != sec) { g_c2c0c = 1; g_c1739 = sec; }
    if (g_c34b8 != 4) {
      if (g_c34b8 != 8) return;
      if (g_c34b7 == 0) {
        if (g_c349c == 2) { i4 = hour_shift(2, 0xc, 0xf); u6 = i4 + 0x67; }
        else if (g_c349c == 3) { i4 = hour_shift(3, 0xc, 0xf); u6 = i4 + 99; }
        else if (g_c349c == 0) { i4 = hour_shift(3, 0xc, 0xf); u6 = i4 + 0x62; }
        else { i4 = hour_shift(3, 0xc, 0xf); u6 = i4 + 0x60; }
        u6 &= 0xff;
      } else if (g_c34b7 == 1) {
        if (g_c349c == 2) { i4 = hour_shift(2, 8, 0xb); u6 = i4 + 0x47; }
        else if (g_c349c == 3) { i4 = hour_shift(3, 8, 0xb); u6 = i4 + 0x43; }
        else if (g_c349c == 0) { i4 = hour_shift(3, 8, 0xb); u6 = i4 + 0x42; }
        else { i4 = hour_shift(3, 8, 0xb); u6 = i4 + 0x40; }
        u6 &= 0xff;
      } else u6 = 0x25;
      if (g_c349c == 2) {
        slide_hours(hr, (u6 + 10) & 0xff); slide_minutes(mn, u6); u7 = u6 + 8;
LFD6:
        u6 = 200;
      } else {
        if (g_c349c < 3) {
          if (g_c349c == 0) { slide_hours(hr, (u6 + 0x10) & 0xff); slide_minutes(mn, u6); u7 = u6 + 0xd; }
          else {
LFC2:
            slide_hours(hr, (u6 + 0x13) & 0xff); slide_minutes(mn, u6); u7 = u6 + 0xf;
          }
          goto LFD6;
        }
        if (g_c349c == 3) { slide_hours(hr, (u6 + 0xe) & 0xff); slide_minutes(mn, u6); u7 = u6 + 0xc; goto LFD6; }
        if (g_c349c != 5) goto LFC2;
        slide_hours(hr, (u6 + 0x16) & 0xff); slide_minutes(mn, (u6 + 10) & 0xff); slide_seconds(sec, u6);
        u7 = u6 + 0x14; u6 = (u6 + 8) & 0xff;
      }
      u7 &= 0xff;
      goto L215;
    }
    if (g_c349c == 2) {
      i4 = hour_shift(2, 4, 7);
      slide_hours(hr, (i4 + 0x31U) & 0xff);
      u6 = i4 + 0x27;
    } else if (g_c349c < 3) {
      if (g_c349c == 0) {
        i4 = hour_shift(3, 4, 7);
        slide_hours(hr, (i4 + 0x32U) & 0xff);
        u6 = i4 + 0x22;
      } else {
LE65:
        i4 = hour_shift(3, 4, 7);
        slide_hours(hr, (i4 + 0x33U) & 0xff);
        u6 = i4 + 0x20;
      }
    } else {
      if (g_c349c != 3) {
        if (g_c349c != 5) goto LE65;
        i4 = hour_shift(3, 4, 7);
        slide_hours(hr, (i4 + 0x36U) & 0xff);
        slide_minutes(mn, (i4 + 0x2aU) & 0xff);
        slide_seconds(sec, (i4 + 0x20U) & 0xff);
        u7 = i4 + 0x34;
        u6 = i4 + 0x28;
        goto L1ED;
      }
      i4 = hour_shift(3, 4, 7);
      slide_hours(hr, (i4 + 0x31U) & 0xff);
      u6 = i4 + 0x23;
    }
    slide_minutes(mn, u6 & 0xff);
    u6 = i4 + 0x2f;
  } else {
    if (!P.displayAnimate()) return;
    if (!P.getZoneStatus(0)) return;
    if (P.getZoneCount() != 0) P.setTextEffect(0, PA_PRINT, PA_NO_EFFECT);
    if (g_c1739 != sec) { g_c2c0c = 1; g_c1739 = sec; }
    if (g_c349c == 2) {
      nm = g_c34b9;
      i = (nm < 6) ? 0x12 : 0x1c;
      s = hour_shift_all(2);
      u7 = (((nm * 8 - i) >> 1) & 0xff) + s & 0xff;
      u6 = u7 + 8;
      if (5 < g_c34b9) {
        slide_hours(hr, (u7 + 0x14) & 0xff); slide_minutes(mn, (u7 + 10) & 0xff); slide_seconds(sec, u7);
        u7 = (u7 + 0x12) & 0xff; u6 &= 0xff;
        goto L215;
      }
      slide_hours(hr, (u7 + 10) & 0xff); slide_minutes(mn, u7);
    } else {
      if (2 < g_c349c) {
        if (g_c349c == 3) {
          nm = g_c34b9;
          i = (nm < 6) ? 0x1a : 0x24;
          s = hour_shift_all(3);
          u6 = (((nm * 8 - i) >> 1) & 0xff) + s & 0xff;
          if (g_c34b9 < 6) {
            slide_hours(hr, (u6 + 0xe) & 0xff); slide_minutes(mn, u6);
            u6 = u6 + 0xc;
            goto L210;
          }
          slide_hours(hr, (u6 + 0x18) & 0xff); slide_minutes(mn, (u6 + 10) & 0xff); slide_seconds(sec, u6);
          u7 = u6 + 0x16;
        } else {
          if (g_c349c != 5) goto L1AB;
          nm = g_c34b9;
          s = hour_shift_all(3);
          u6 = (((nm - 4) * 4) & 0xff) + s & 0xff;
          slide_hours(hr, (u6 + 0x16) & 0xff); slide_minutes(mn, (u6 + 10) & 0xff); slide_seconds(sec, u6);
          u7 = u6 + 0x14;
        }
        u6 = u6 + 8;
L1ED:
        u7 &= 0xff; u6 &= 0xff;
        goto L215;
      }
      if (g_c349c == 0) {
        nm = g_c34b9;
        i = (nm < 6) ? 0x1c : 0x28;
        s = hour_shift_all(3);
        u6 = (((int)(nm * 8 - i) >> 1) & 0xff) + s & 0xff;
        if (5 < g_c34b9) {
          slide_hours(hr, (u6 + 0x1c) & 0xff); slide_minutes(mn, (u6 + 0xc) & 0xff); slide_seconds(sec, u6);
          u7 = u6 + 0x19;
          goto L1EB;
        }
        slide_hours(hr, (u6 + 0x10) & 0xff); slide_minutes(mn, u6);
        u6 = u6 + 0xd;
      } else {
L1AB:
        nm = g_c34b9;
        i = (nm < 6) ? 0x20 : 0x2e;
        s = hour_shift_all(3);
        u6 = (((nm * 8 - i) >> 1) & 0xff) + s & 0xff;
        if (5 < g_c34b9) {
          slide_hours(hr, (u6 + 0x20) & 0xff); slide_minutes(mn, (u6 + 0xd) & 0xff); slide_seconds(sec, u6);
          u7 = u6 + 0x1c;
L1EB:
          u6 = u6 + 9;
          goto L1ED;
        }
        slide_hours(hr, (u6 + 0x13) & 0xff); slide_minutes(mn, u6);
        u6 = u6 + 0xf;
      }
    }
  }
L210:
  u7 = u6 & 0xff;
  u6 = 200;
L215:
  slide_colon(sec, u6, u7);
}
