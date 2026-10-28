// Табло: рядок часу, двокрапка, зони/шрифти, ефекти входу/виходу циферблата, великий 2-поверховий, інфо-цикл.
// EN: Display: time string, colon, zones/fonts, clock-face in/out effects, big clock, information cycle (from 2.7.1).
// Перенесено з 2.7.1 функція за функцією (адреси оригіналу в коментарях).
#include "fw.h"

struct EffSpd { uint32_t eff; uint16_t spd; uint16_t _p; };
// @3ffbedd8: ефекти статичного показу пунктів (швидкість ×5)
EffSpd g_bedd8[16] = { {8, 7}, {10, 7}, {3, 7}, {19, 2}, {20, 2}, {21, 2}, {22, 2}, {13, 2}, {14, 2}, {2, 7}, {9, 10}, {31, 3}, {30, 3}, {35, 3}, {32, 3}, {6, 30} };
// @3ffbee50: випадкові ефекти циферблата (парні — спрайт)
EffSpd g_bee50[57] = { {6, 30}, {8, 20}, {6, 30}, {10, 20}, {6, 30}, {12, 7}, {6, 30}, {11, 20}, {6, 30}, {23, 20}, {6, 30}, {24, 20}, {6, 30}, {25, 20},
  {6, 30}, {26, 20}, {6, 30}, {19, 20}, {6, 30}, {20, 20}, {6, 30}, {21, 20}, {6, 30}, {22, 20}, {6, 30}, {13, 20}, {6, 30}, {14, 20}, {6, 30}, {17, 20},
  {6, 30}, {15, 20}, {6, 30}, {27, 20}, {6, 30}, {28, 20}, {6, 30}, {29, 45}, {6, 30}, {30, 35}, {6, 30}, {31, 30}, {6, 30}, {32, 25}, {6, 30}, {33, 16},
  {6, 30}, {34, 30}, {6, 30}, {35, 30}, {6, 30}, {36, 15}, {6, 30}, {37, 10}, {6, 30}, {38, 4}, {0xffffffff, 0xffff, 0xffff} };
char g_c347d[11];   // рядок часу
char g_c3472[11];   // його копія з бітом 7
char g_c27f9[0x400];// текст пункту для зони 0
char g_c27ef[10];   // короткий текст зони 3

#define TXT(p) ((char *)(p))
// таблиця гліфів секунд @3f4061b4
const uint8_t SEC_GLYPHS[10] = { 20, 21, 22, 23, 24, 25, 26, 27, 28, 29 };


// @400d8930: темп і кількість фаз двокрапки за pAn (g_c2eed) і шрифтом font_l (g_c349c)
void colon_timing(int8_t ph) {
  switch (g_c2eed) {
    case 0: break;
    case 1:
L1:   g_c2ee4 = 1; g_c349e = 1000; return;
    case 2:
      if (g_c349c < 6 && ((1 << (g_c349c & 0x1f)) & 0x2d) != 0) break;
      g_c2ee4 = 3;
      if (ph == 1) { g_c349e = 200; return; }
      if (ph != 3) { g_c349e = 0x96; return; }
      goto L500;
    case 3:
      if (5 < g_c349c || ((1 << (g_c349c & 0x1f)) & 0x2d) == 0) {
        g_c2ee4 = 5;
        if (ph != 0) { g_c349e = 100; return; }
        goto L500;
      }
      break;
    case 4:
      if (5 < g_c349c || ((1 << (g_c349c & 0x1f)) & 0x2d) == 0) goto L1;
      break;
    default: return;
  }
  g_c2ee4 = 1;
L500:
  g_c349e = 500;
}

// @400d84dc: рядок часу "%02d%c%02d" (+ секунди гліфами) з символом двокрапки за pAn і фазою
void time_string_format(char *out, int8_t ph) {
  switch (g_c2eed) {
    case 0: case 1:
L86a5:
      if (ph == 0) {
L8561:  g_c18e9 = 0x3a;
      } else if (ph == 1) g_c18e9 = 0x20;
      break;
    case 2:
      if (g_c349c < 6 && ((1 << (g_c349c & 0x1f)) & 0x2d) != 0) goto L86a5;
      if (ph == 1) g_c18e9 = 10;
      else {
        if (ph == 0 || ph == 2) goto L8561;
        if (ph == 3) g_c18e9 = 0xb;
      }
      break;
    case 3:
      if (g_c349c < 6 && ((1 << (g_c349c & 0x1f)) & 0x2d) != 0) goto L86a5;
      switch (ph) {
        case 0: case 4: g_c18e9 = 0xc; break;
        case 1: g_c18e9 = 0xd; break;
        case 2: g_c18e9 = 0xe; break;
        case 3: g_c18e9 = 0xf; break;
        case 5: goto L8561;
      }
      break;
    case 4:
      if (g_c349c < 6 && ((1 << (g_c349c & 0x1f)) & 0x2d) != 0) goto L86a5;
      if (ph == 0) g_c18e9 = 0x10;
      else if (ph == 1) g_c18e9 = 0x11;
      break;
  }
  int mn = minute();
  int hr = hour();
  uint32_t s1 = (second() / 10) & 0xff;
  uint8_t g1 = 0x20; if (s1 < 10) g1 = SEC_GLYPHS[s1];
  uint32_t s2 = (second() % 10) & 0xff;
  uint8_t g2 = 0x20; if (s2 < 10) g2 = SEC_GLYPHS[s2];
  bool secs;
  if (g_c349c == 5) secs = (uint8_t)(g_c3471 - 1) > 1;
  else secs = (5 < g_c34b9) && (g_c34ba == 4);
  if (g_c34ab != 0) {
    if (secs) sprintf(out, "%d%c%02d%c%c%c", hr, g_c18e9, mn, g_c18e9, g1, g2);
    else sprintf(out, "%d%c%02d", hr, g_c18e9, mn);
  } else {
    if (secs) sprintf(out, "%02d%c%02d%c%c%c", hr, g_c18e9, mn, g_c18e9, g1, g2);
    else sprintf(out, "%02d%c%02d", hr, g_c18e9, mn);
  }
}

// @400d87bc: системний шрифт зони 0 (fs; для ro/pl — свої)
void sys_font_apply() {
  if (g_c34a3 == g_c349b) return;
  g_c34a3 = g_c349b;
  const uint8_t *f = font_sys_lang_ro;
  if (g_c2eef != 4) {
    f = font_sys_lang_pl;
    if (g_c2eef != 6) {
      if (g_c349b == 1) f = font_sys1_small;
      else if (g_c349b == 0) f = font_sys0_default;
      else if (g_c349b == 2) f = font_sys2_fat;
      else return;
    }
  }
  P.setFont(0, (MD_MAX72XX::fontType_t *)f);
}
// @400d880c
void lang_sync() { g_c2eee = g_c2eef; g_c34b0 = g_c2eef; }

// @400d86b4: шрифт малого циферблата (font_l) у зону годинника
void clock_font_apply() {
  if (g_c34a4 == g_c349c) return;
  g_c34a4 = g_c349c;
  if (g_c34ba == 4) g_c18b0 = 0;
  else if (g_c34ba == 8) g_c18b0 = 1;
  g_c34a2 = 0; g_c34a1 = 0; g_c34a0 = 0;
  uint8_t a, b;
  if (g_c34ba == 4) { a = 0; b = g_c34b9 - 1; g_c34bc->clear(a, b); }
  else if (g_c34ba == 8) {
    if (g_c34b8 == 8) { b = 0xb; a = 8; if (g_c34b7 == 0) { b = 0xf; a = 0xc; } }
    else { b = 7; a = 4; }
    g_c34bc->clear(a, b);
  }
  const uint8_t *f;
  switch (g_c349c) {
    case 0: f = font_clock0_default; break;
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
    default: return;
  }
  P.setFont(g_c18b0, (MD_MAX72XX::fontType_t *)f);
  g_c34bc->setFont((MD_MAX72XX::fontType_t *)f);
}

// @400d8824: розмітка зон після застосування конфігурації
void display_zones_init() {
  if (g_c34ba != 8) {
    sys_font_apply();
    P.setInvert(false);
    P.setZone(0, 0, g_c34b9 - 1);
    goto done;
  }
  if (g_c34b8 == 4) {
    P.setZone(0, 0, 3); P.setZone(1, 4, 7); P.setZone(2, 8, 0xb); P.setZone(3, 0xc, 0xf);
  } else if (g_c34b8 == 8) {
    P.setZone(0, 0, 7); P.setZone(1, 0xc, 0xf); P.setZone(2, 8, 0xb); P.setZone(3, 0x10, 0x11);
  }
  clock_font_apply();
  sys_font_apply();
  P.setInvert(false);
done:
  g_c34b4 = 0x18;
  g_c34a3 = 0x12;
  g_c34a4 = 99;
  lang_sync();
}

// @400e4460
void display_clear_all() {
  P.displayClear(0); P.displayClear(1); P.displayClear(2); P.displayClear(3);
  g_c34bc->clear(0, 0xf);
}

// @400d89d0: оновлення часу на двох поверхах (великий циферблат)
void clock_tick_2zones() {
  if (fx_bigslide_tick()) return;                // нове: триває перелистування цифр (bigslide.cpp)
  P.displayAnimate();
  if (!P.getZoneStatus(0)) return;
  if (!P.getZoneStatus(1)) return;
  colon_timing(g_c34ae);
  if ((uint32_t)g_c349e <= millis() - g_c18c0) {
    g_c18c0 = millis();
    char prev[sizeof g_c347d];                   // нове: рядок, що зараз на табло
    memcpy(prev, g_c347d, sizeof prev);
    time_string_format(g_c347d, g_c34ae);
    strcpy80(g_c3472, g_c347d);
    g_c34ae = g_c34ae + 1;
    if (g_c2ee4 < g_c34ae) g_c34ae = 0;
    if (millis() - g_c18bc > 999) {
      g_c18bc = millis();
      g_c34a8 = g_c34a8 + 1;
      g_c349d = 0;
      g_c2f1f = 1;
    }
    if (fx_bigslide_begin(prev, g_c347d)) return;   // нове: цифри змінились — перелистування замість друку (An=1)
    P.displayReset(0);
    P.displayReset(1);
  }
}

// @400fd228: малий годинник (4 модулі)
void smallclock_tick() {
  clock_font_apply();
  uint8_t an = g_c34ad;
  if (g_c34ad == 0) {
    P.displayAnimate();
    if (P.getZoneStatus(0)) {
      colon_timing(g_c34ae);
      if ((uint32_t)g_c349e <= millis() - g_c18b8) {
        g_c18b8 = millis();
        time_string_format(g_c347d, g_c34ae);
        strcpy80(g_c3472, g_c347d);
        g_c34ae = g_c34ae + 1;
        if (g_c2ee4 < g_c34ae) g_c34ae = an;
        if (millis() - g_c18b4 > 999) {
          g_c18b4 = millis();
          g_c34a7 = g_c34a7 + 1;
          g_c349d = 0;
          g_c2f1f = 1;
        }
        P.displayReset(0);
      }
    }
  } else if (g_c34ad == 1) {
    clock_slide();
  }
}

// @400e2474: ефект малого годинника над інфо-рядком (cl_effect_sm)
void small_effect_pick() {
  g_c27e2 = random(0, 0x17);
  switch (g_c3497) {
    case 0: g_bec08 = 3; g_bec06 = 0x14; break;
    case 1: g_bec08 = 2; g_bec06 = 0x14; break;
    case 2: g_bec08 = 9; g_bec06 = 0x28; break;
    case 3: g_bec08 = 0xb; g_bec06 = 0x14; break;
    case 4: g_bec08 = 0x16; g_bec06 = 0x14; break;
    case 5: g_bec08 = 0xc; g_bec06 = 7; break;
    case 6: g_bec08 = 8; g_bec06 = 0x14; break;
    case 7: g_bec08 = 6; g_bec06 = 0x19; break;
    default:
      g_c27e3 = random(0, 0x38);
      g_bec08 = g_bee50[g_c27e3].eff;
      g_bec06 = g_bee50[g_c27e3].spd;
      break;
    case 9: g_bec08 = 0x1d; g_bec06 = 0x2d; break;
    case 10: g_bec08 = 0x1e; g_bec06 = 0x23; break;
    case 11: g_bec08 = 0x1f; g_bec06 = 0x1e; break;
    case 12: g_bec08 = 0x20; g_bec06 = 0x19; break;
    case 13: g_bec08 = 0x21; g_bec06 = 0xc; break;
    case 14: g_bec08 = 0x22; g_bec06 = 0x1e; break;
    case 15: g_bec08 = 0x23; g_bec06 = 0x1e; break;
    case 16: g_bec08 = 0x24; g_bec06 = 0xf; break;
    case 17: g_bec08 = 1; g_bec06 = 0; break;
    case 18: g_bec08 = 0x25; g_bec06 = 0xc; break;
    case 19: g_bec08 = 0x26; g_bec06 = 0x16; break;
  }
}

// @400fd2fc: малий годинник у зоні 1 під час інфо-циклу (8 модулів, 2 поверхи)
void smallclock_zone1() {
  int8_t an = g_c34ad;
  if (g_c34a7 == 1) {
    if (g_c34ad == 0) {
      if (g_c34b6 == 1) { P.displayZoneText(1, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); g_c34b6 = an; }
    } else if (g_c34ad == 1) { clock_slide(); g_c34b6 = 0; }
    if (g_c34a6 == 10) { g_c34a7 = 2; g_c34a6 = 0; return; }
  } else if (g_c34a7 == 0) {
    if (g_c34b6 == 0) {
      small_effect_pick();
      time_string_format(g_c347d, g_c34ae == 0);
      strcpy80(g_c3472, g_c347d);
      size_t l = strlen(g_c347d);
      if ((l & 0xff) < (uint32_t)g_c1738) P.displayClear(1);
      g_c1738 = (uint8_t)l;
      P.displayZoneText(1, g_c347d, PA_CENTER, g_bec06, 0, (textEffect_t)g_bec08, PA_NO_EFFECT);
      if (g_bec08 == 6) P.setSpriteData(g_bed20[g_c27e2].data, g_bed20[g_c27e2].w, g_bed20[g_c27e2].frames, g_bed20[g_c27e2].data, g_bed20[g_c27e2].w, g_bed20[g_c27e2].frames);
      g_c34b6 = 1;
    }
    g_c34a7 = 1; g_c34a6 = 0; g_c34a2 = 0;
  } else if (g_c34a7 == 2) {
    time_string_format(g_c347d, g_c34ae == 0);
    strcpy80(g_c3472, g_c347d);
    P.displayZoneText(1, g_c347d, PA_CENTER, g_bec06, 0, PA_PRINT, (textEffect_t)g_bec08);
    if (g_bec08 == 6) P.setSpriteData(g_bed20[g_c27e2].data, g_bed20[g_c27e2].w, g_bed20[g_c27e2].frames, g_bed20[g_c27e2].data, g_bed20[g_c27e2].w, g_bed20[g_c27e2].frames);
    g_c34a6 = 0; g_c34a7 = 3;
  } else if (g_c34a7 == 3) {
    g_c34a7 = 0;
  }
}

// @400fd4a8: годинник у зоні 1 (8 модулів) — щохвилинні ефекти й двокрапка
void smallclock_8m() {
  if (!P.getZoneStatus(1)) return;
  clock_font_apply();
  smallclock_zone1();
  uint8_t an = g_c34ad;
  if (g_c34ad == 0 && (uint32_t)g_c349e <= millis() - g_c18e4) {
    colon_timing(g_c34ae);
    g_c18e4 = millis();
    time_string_format(g_c347d, g_c34ae);
    strcpy80(g_c3472, g_c347d);
    P.displayZoneText(1, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    g_c34ae = g_c34ae + 1;
    if (g_c2ee4 < g_c34ae) g_c34ae = an;
    g_c34b6 = 1;
    if (millis() - g_c18e0 > 999) { g_c18e0 = millis(); g_c34a6 = g_c34a6 + 1; }
    P.displayReset(1);
  }
}

// @400e2558: вхід циферблата з ефектом cl_effect (g_c19dc)
void clock_enter() {
  uint16_t spd; uint32_t eff;
  time_string_format(g_c347d, g_c34ae);
  strcpy80(g_c3472, g_c347d);
  switch (g_c19dc) {
    case 0: P.displayZoneText(0, g_c347d, PA_CENTER, 0x14, 0, PA_SCROLL_DOWN, PA_NO_EFFECT); eff = 2; spd = 0x14; break;
    case 1: P.displayZoneText(0, g_c347d, PA_CENTER, 0x14, 0, PA_SCROLL_UP, PA_NO_EFFECT); eff = 3; spd = 0x14; break;
    case 2: P.displayZoneText(0, g_c347d, PA_CENTER, 0x3c, 0, PA_FADE, PA_NO_EFFECT); eff = 9; spd = 0x3c; break;
    case 3: eff = 0xb; goto L264c;
    case 4: eff = 0x16; goto L264c;
    case 5: P.displayZoneText(0, g_c347d, PA_CENTER, 7, 0, PA_RANDOM, PA_NO_EFFECT); eff = 0xc; spd = 7; break;
    case 6: eff = 8;
L264c:
      P.displayZoneText(0, g_c347d, PA_CENTER, 0x14, 0, (textEffect_t)eff, PA_NO_EFFECT);
      spd = 0x14; break;
    case 7:
      if (g_c34ba != 4) goto end;
      P.displayZoneText(0, g_c347d, PA_CENTER, 0x19, 0, PA_SPRITE, PA_NO_EFFECT);
      P.displayZoneText(1, g_c347d, PA_CENTER, 0x19, 0, PA_SPRITE, PA_NO_EFFECT);
      g_c27e2 = g_c3495;
L2759:
      P.setSpriteData(g_bed20[g_c27e2].data, g_bed20[g_c27e2].w, g_bed20[g_c27e2].frames, g_bed20[g_c27e2].data, g_bed20[g_c27e2].w, g_bed20[g_c27e2].frames);
      goto end;
    case 8:
      if (g_c34ba != 4) goto end;
      if ((g_c27e3 & 1) != 1) { P.displayZoneText(0, g_c347d, PA_CENTER, 0x19, 0, PA_SPRITE, PA_NO_EFFECT); goto L2759; }
      P.displayZoneText(0, g_c347d, PA_CENTER, g_bee50[g_c27e3].spd, 0, (textEffect_t)g_bee50[g_c27e3].eff, PA_NO_EFFECT);
      spd = g_bee50[g_c27e3].spd; eff = g_bee50[g_c27e3].eff; break;
    case 9: P.displayZoneText(0, g_c347d, PA_CENTER, 0x2d, 0, (textEffect_t)0x1d, PA_NO_EFFECT); eff = 0x1d; spd = 0x2d; break;
    case 10: P.displayZoneText(0, g_c347d, PA_CENTER, 0x23, 0, (textEffect_t)0x1e, PA_NO_EFFECT); eff = 0x1e; spd = 0x23; break;
    case 11: eff = 0x1f; goto L2804;
    case 12: P.displayZoneText(0, g_c347d, PA_CENTER, 0x19, 0, (textEffect_t)0x20, PA_NO_EFFECT); eff = 0x20; spd = 0x19; break;
    case 13: eff = 0x21; goto L2886;
    case 14: eff = 0x22; goto L2804;
    case 15: eff = 0x23;
L2804:
      P.displayZoneText(0, g_c347d, PA_CENTER, 0x1e, 0, (textEffect_t)eff, PA_NO_EFFECT);
      spd = 0x1e; break;
    case 16: P.displayZoneText(0, g_c347d, PA_CENTER, 0xf, 0, (textEffect_t)0x24, PA_NO_EFFECT); eff = 0x24; spd = 0xf; break;
    case 17: P.displayZoneText(0, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); eff = 1; spd = 0; break;
    case 18: eff = 0x25;
L2886:
      P.displayZoneText(0, g_c347d, PA_CENTER, 0xc, 0, (textEffect_t)eff, PA_NO_EFFECT);
      spd = 0xc; break;
    case 19: P.displayZoneText(0, g_c347d, PA_CENTER, 0x16, 0, (textEffect_t)0x26, PA_NO_EFFECT); eff = 0x26; spd = 0x16; break;
    default: goto end;
  }
  P.displayZoneText(1, g_c347d, PA_CENTER, spd, 0, (textEffect_t)eff, PA_NO_EFFECT);
end:
  g_c2e94 = millis();
}

// @400e2a7c: вихід циферблата з ефектом (той самий код ефекту)
void clockface_effect_select() {
  uint16_t spd; uint32_t eff;
  g_c2e8c = 1;
  time_string_format(g_c347d, g_c34ae);
  strcpy80(g_c3472, g_c347d);
  switch (g_c19dc) {
    case 0: P.displayZoneText(0, g_c347d, PA_CENTER, 0x14, 0, PA_PRINT, PA_SCROLL_UP); eff = 3; spd = 0x14; break;
    case 1: P.displayZoneText(0, g_c347d, PA_CENTER, 0x14, 0, PA_PRINT, PA_SCROLL_DOWN); eff = 2; spd = 0x14; break;
    case 2: P.displayZoneText(0, g_c347d, PA_CENTER, 0x3c, 0, PA_PRINT, PA_FADE); eff = 9; spd = 0x3c; break;
    case 3: eff = 0xb; goto L2b62;
    case 4: eff = 0x16; goto L2b62;
    case 5: P.displayZoneText(0, g_c347d, PA_CENTER, 3, 0, PA_PRINT, PA_RANDOM); eff = 0xc; spd = 3; break;
    case 6: eff = 8;
L2b62:
      P.displayZoneText(0, g_c347d, PA_CENTER, 0x14, 0, PA_PRINT, (textEffect_t)eff);
      spd = 0x14; break;
    case 7:
      if (g_c34ba != 4) return;
      P.displayZoneText(0, g_c347d, PA_CENTER, 0x19, 0, PA_PRINT, PA_SPRITE);
      P.displayZoneText(1, g_c347d, PA_CENTER, 0x19, 0, PA_PRINT, PA_SPRITE);
      g_c27e2 = g_c3495;
L2c73:
      P.setSpriteData(g_bed20[g_c27e2].data, g_bed20[g_c27e2].w, g_bed20[g_c27e2].frames, g_bed20[g_c27e2].data, g_bed20[g_c27e2].w, g_bed20[g_c27e2].frames);
      return;
    case 8:
      if (g_c34ba != 4) return;
      if ((g_c27e3 & 1) != 1) { P.displayZoneText(0, g_c347d, PA_CENTER, 0x19, 0, PA_PRINT, PA_SPRITE); goto L2c73; }
      P.displayZoneText(0, g_c347d, PA_CENTER, g_bee50[g_c27e3].spd, 0, PA_PRINT, (textEffect_t)g_bee50[g_c27e3].eff);
      eff = g_bee50[g_c27e3].eff; spd = g_bee50[g_c27e3].spd; break;
    case 9: P.displayZoneText(0, g_c347d, PA_CENTER, 0x2d, 0, PA_PRINT, (textEffect_t)0x1d); eff = 0x1d; spd = 0x2d; break;
    case 10: P.displayZoneText(0, g_c347d, PA_CENTER, 0x23, 0, PA_PRINT, (textEffect_t)0x1e); eff = 0x1e; spd = 0x23; break;
    case 11: eff = 0x1f; goto L2d0d;
    case 12: P.displayZoneText(0, g_c347d, PA_CENTER, 0x19, 0, PA_PRINT, (textEffect_t)0x20); eff = 0x20; spd = 0x19; break;
    case 13: eff = 0x21; goto L2d8e;
    case 14: eff = 0x22; goto L2d0d;
    case 15: eff = 0x23;
L2d0d:
      P.displayZoneText(0, g_c347d, PA_CENTER, 0x1e, 0, PA_PRINT, (textEffect_t)eff);
      spd = 0x1e; break;
    case 16: P.displayZoneText(0, g_c347d, PA_CENTER, 0xf, 0, PA_PRINT, (textEffect_t)0x24); eff = 0x24; spd = 0xf; break;
    case 17: P.displayZoneText(0, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT); eff = 0; spd = 0; break;
    case 18: eff = 0x25;
L2d8e:
      P.displayZoneText(0, g_c347d, PA_CENTER, 0xc, 0, PA_PRINT, (textEffect_t)eff);
      spd = 0xc; break;
    case 19: P.displayZoneText(0, g_c347d, PA_CENTER, 0x16, 0, PA_PRINT, (textEffect_t)0x26); eff = 0x26; spd = 0x16; break;
    default: return;
  }
  P.displayZoneText(1, g_c347d, PA_CENTER, spd, 0, PA_PRINT, (textEffect_t)eff);
}

// @400e28e4: старт великого циферблата
void bigclock_start() {
  fx_bigslide_reset();                           // нове: перелистування цифр (bigslide.cpp) — з нуля
  P.displayClear(0); P.displayClear(1);
  P.displayReset(0); P.displayReset(1);
  P.setCharSpacing(1);
  time_string_format(g_c347d, 0);
  if (g_c34ba == 4) {
    g_c19dc = g_c3498;
    g_c27e3 = random(0, 0x38);
    g_c27e2 = random(0, 0x17);
    g_c34a4 = 0x79;
    sys_font_apply();
    goto enter;
  }
  if (g_c34ba != 8) { g_c34a6 = 0; g_c34a7 = 0; g_c34a8 = 0; return; }
  if (g_c34b8 == 8) {
    uint8_t a, b;
    if (g_c34b7 == 0) { P.setZone(0, 4, 7); P.setZone(1, 0xc, 0xf); P.setZone(2, 8, 0xb); b = 3; a = 0; }
    else { P.setZone(0, 0, 3); P.setZone(1, 8, 0xb); P.setZone(2, 0xc, 0xf); b = 7; a = 4; }
    P.setZone(3, a, b);
  }
  if (g_c3498 == 7) g_c19dc = random(0, 7);
  else g_c19dc = g_c3498;
  if (g_c3496 == 1) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_font_3f43d964); P.setFont(1, (MD_MAX72XX::fontType_t *)font_font_3f43da05); g_c34a4 = 0x7b; }
  else if (g_c3496 == 0) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_font_3f43daa6); P.setFont(1, (MD_MAX72XX::fontType_t *)font_font_3f43db47); g_c34a4 = 0x79; }
  else if (g_c3496 == 2) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_font_3f43d822); P.setFont(1, (MD_MAX72XX::fontType_t *)font_font_3f43d8c3); g_c34a4 = 0x7b; }
  g_c34a3 = 99;
enter:
  clock_enter();
  g_c34a8 = 0; g_c34a7 = 0; g_c34a6 = 0;
}

// @400e4494: перезапуск інформаційного циклу (після старту/зміни налаштувань)
void display_restart_content() {
  g_c1a8e = 1;
  if ((uint8_t)(g_c2fc7 - 1) < 3) {
    digitalWrite(2, 1);
    delay(0x32);
    digitalWrite(2, 0);
    g_c1a95 = 0;
  }
  g_c1aa0 = 0; g_c1aa4 = 0;
  led_off();
  g_c2ee2 = 0; g_c1a8a = 0;
  P.setCharSpacing(1);
  g_c34a9 = 3;
  P.displayZoneText(0, "        ", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
  if (g_c34ba == 8) {
    uint8_t z, a, b;
    if (g_c34b8 == 4) { P.setZone(0, 0, 3); z = 1; a = g_c34b8; b = 7; }
    else {
      if (g_c34b8 != 8) goto after;
      if (g_c34b7 == 0) { P.setZone(0, 0, 7); P.setZone(1, 0xc, g_c34b8 * 2 - 1); P.setZone(3, 0, 0); a = g_c34b8; b = 0xb; }
      else { P.setZone(0, 0, 7); P.setZone(1, 8, 0xb); P.setZone(3, 0, 0); b = 0xf; a = 0xc; }
      z = 2;
    }
    P.setZone(z, a, b);
  }
after:
  display_clear_all();
  P.displayAnimate();
  P.displayReset();
  g_c34af = 0;
  g_c34a4 = 0x44;
  g_c34a3 = 99;
  if ((g_c2eef & 0xfd) == 4) P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_ro);
  {
    const uint8_t *f = nullptr;
    if (g_c349b == 1) f = font_sys1_small;
    else if (g_c349b == 0) f = font_sys0_default;
    else if (g_c349b == 2) f = font_sys2_fat;
    if (f) P.setFont(0, (MD_MAX72XX::fontType_t *)f);
  }
  g_c34b4 = 0x18;
  g_c34b3 = 0x16;
  uint8_t *m;
  if (g_c34ba == 4) m = &g_c3470;
  else if (g_c34ba == 8) m = &g_c3471;
  else return;
  if ((uint8_t)(*m - 1) < 2) bigclock_start();
}

// @400e2f68: після пункту — повернення до великого циферблата
void to_bigclock() {
  if (g_c34b8 == 8) g_c34af = g_c34af + 1;
  g_c19db = 0;
  if (!P.getZoneStatus(0)) return;
  if (g_c349a == 2) { g_c3471 = g_c349a; bigclock_start(); g_c34a8 = 1; return; }
  if (g_c349a == 3) {
    if (g_c2e72 != 0) goto L1;
    if (g_c3471 != 2) bigclock_start();
    g_c3471 = 2;
    return;
  }
  if (g_c349a != 1) return;
L1:
  g_c3471 = 1;
  bigclock_start();
}
// @400e2fe8
void to_bigclock_mark() { to_bigclock(); g_c19db = 1; }
// @400e3240
void to_smallclock_4m() {
  g_c19db = 0;
  if (!P.getZoneStatus(0)) return;
  if (g_c3499 != 0 && g_c3499 != 1) return;
  bigclock_start();
  g_c3470 = 2;
}
// @400e3274
void to_smallclock_4m_mark() { to_smallclock_4m(); g_c19db = 1; }
// @400e34c8
void smallclock_switch_4m() {
  if (g_c3499 == 1) {
    if (g_c2e72 == 0) { g_c3470 = g_c3499; bigclock_start(); }
  } else if (g_c3499 == 2) {
    bigclock_start();
    g_c3470 = 1;
    g_c34a7 = 1;
  }
}
// @400e2ebc: широкий режим (8 модулів в ряд) — відновлення зон після годинника
void wide_zones_restore() {
  if (g_c2e8c != 0 && g_c34b8 == 8 && g_c34ba == 8) {
    P.displayClear(3);
    g_c34a2 = 0;
    P.setZone(0, 0, 7);
    P.setZone(3, 0x10, 0x11);
    uint8_t a, b;
    if (g_c34b7 == 0) { P.setZone(1, 0xc, 0xf); b = 0xb; a = 8; }
    else { P.setZone(1, 8, 0xb); b = 0xf; a = 0xc; }
    P.setZone(2, a, b);
    g_c2e8c = 0;
  }
}
// @400e2f50
void lang_check() { if (g_c2eee != g_c2eef) lang_sync(); }
// @400e203c
bool every_ms_1744(uint32_t ms) {
  uint32_t t = g_c1744;
  bool r = ms <= millis() - t;
  if (r) g_c1744 = millis();
  return r;
}

// @400e2de8: чи є що показувати в інфо-циклі
bool content_any_enabled() {
  if (g_c2eeb == 0 && g_c32e8 == 0 && g_c32e7 == 0 && g_c32e5 == 0 && g_c32e2 == 0 && g_c32e1 == 0) {
    if (g_c2d29 == 0 || g_c3319 != 0) return false;
    if ((g_c30c4 != 1 || g_c30b8 == "" || g_c30ac == "" || g_c30a0 == "" || g_c3094 == "") &&
        g_c3139 != 1 && g_c3157 == 0 && g_c2eea == 0 && g_c30f8 != 1 && g_c2ffc != 1) {
      return g_c2efc == 1;
    }
  }
  return true;
}

// @4011aabc: показати поточний пункт (8 модулів)
void content_show_slot_8m() {
  if (g_c34b8 == 4) {
    if (!P.getZoneStatus(0)) return;
  } else if (g_c34b8 == 8) {
    if (!P.getZoneStatus(0) || !P.getZoneStatus(3)) return;
    wide_zones_restore();
  }
  {
    String t("");
    content_slot_text(g_c34af, t);
    if (t == "") P.displayClear(0);
    else {
      utf8_to_led(t, g_c27f9, 0x400);
      g_c27e4 = random(0, 0xf);
      if (g_c2eef == 4) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_ro); g_c34a3 = 4; }
      else if (g_c2eef == 6) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_pl); g_c34a3 = 4; }
      else { g_c18e8 = 0; sys_font_apply(); }
      if (g_c34af == 0x2f) {
        if (g_c30c8.equals("ro")) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_ro); g_c34a3 = 4; }
        if (g_c30c8.equals("pl")) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_pl); g_c34a3 = 4; }
      } else { g_c18e8 = 0; sys_font_apply(); }
      uint32_t eff; int16_t spd; uint16_t pause; textPosition_t al;
      if (g_c3494 == 0) {
        if (g_c18e8 == 0) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_small3px); g_c18e8 = 1; }
        g_c34a3 = 10;
        eff = g_bedd8[g_c27e4].eff;
        spd = (int16_t)g_bedd8[g_c27e4].spd * 5;
        al = PA_CENTER;
        pause = g_c3490;
      } else {
        spd = 0x46 - (uint16_t)g_bf04d;
        eff = 4;
        al = PA_LEFT;
        pause = 0;
      }
      P.displayZoneText(0, g_c27f9, al, spd, pause, (textEffect_t)eff, (textEffect_t)eff);
      P.displayReset(0);
      if (g_c34a9 == 3) g_fx_booting = false;   // нове: перший звичайний вміст — запуск закінчено (status.cpp)
    }
  }
  if (g_c34af < 0x3c) g_c34af = g_c34af + 1;
  else g_c34af = 0;
}

// @4011ad18: показати поточний пункт (4 модулі)
void content_show_slot_4m() {
  if (!P.getZoneStatus(0)) return;
  {
    String t("");
    content_slot_text(g_c34af, t);
    if (t == "") P.displayClear(0);
    else {
      utf8_to_led(t, g_c27f9, 0x400);
      g_c27e3 = random(0, 0x38);
      g_c27e2 = random(0, 0x17);
      g_c27e4 = random(0, 0xf);
      if (g_c2eef == 4) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_ro); g_c34a3 = 4; }
      else if (g_c2eef == 6) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_pl); g_c34a3 = 4; }
      else { g_c18c4 = 0; sys_font_apply(); }
      if (g_c34af == 0x2f) {
        if (g_c30c8.equals("ro")) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_ro); g_c34a3 = 4; }
        if (g_c30c8.equals("pl")) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_sys_lang_pl); g_c34a3 = 4; }
      } else { g_c18c4 = 0; sys_font_apply(); }
      uint32_t eff; int16_t spd; uint16_t pause; textPosition_t al;
      if (g_c3494 == 0) {
        if (g_c18c4 == 0) { P.setFont(0, (MD_MAX72XX::fontType_t *)font_small3px); g_c18c4 = 1; }
        g_c34a3 = 10;
        eff = g_bedd8[g_c27e4].eff;
        spd = (int16_t)g_bedd8[g_c27e4].spd * 5;
        al = PA_CENTER;
        pause = g_c3490;
      } else {
        spd = 0x46 - (uint16_t)g_bf04d;
        eff = 4;
        al = PA_LEFT;
        pause = 0;
      }
      P.displayZoneText(0, g_c27f9, al, spd, pause, (textEffect_t)eff, (textEffect_t)eff);
      P.displayReset(0);
      if (g_c34a9 == 3) g_fx_booting = false;   // нове: перший звичайний вміст — запуск закінчено (status.cpp)
    }
  }
  if (g_c34af < 0x3c) g_c34af = g_c34af + 1;
  else g_c34af = 0;
}

// @400e2ff8: великий циферблат між пунктами (8 модулів) — після пунктів певних груп
void content_bigclock_between_8m() {
  int s = g_c34af - 1;
  const String *k;
  if (s == 0x2a) {
    if (g_c2d29 == 0 || g_c3319 != 0) return;
    if (g_c3157 == 2) k = &g_c313c;
    else { if (g_c3157 != 3 && g_c3157 != 1) return; k = &g_c3148; }
  } else if (s < 0x2b) {
    if (s == 0x1c) {
      if (g_c32e8 == 1 || g_c32e8 == 2) to_bigclock_mark();
      if (g_c32e7 == 1 || g_c32e7 == 2) to_bigclock_mark();
      if ((uint8_t)(g_c32e5 - 1) < 2) to_bigclock_mark();
      if (1 < (uint8_t)(g_c32e2 - 1)) return;
      goto go;
    }
    if (0x1c < s) {
      if (s == 0x23) {
        if (g_c2d29 == 0 || g_c3319 != 0 || g_c30c4 != 1) return;
        if (g_c30b8 == "" || g_c30ac == "" || g_c30a0 == "" || g_c3094 == "") return;
      } else {
        if (s != 0x26) return;
        if (g_c2d29 == 0 || g_c3319 != 0 || g_c3139 != 1) return;
      }
      goto go;
    }
    if (g_c34af == 1) {
      if (g_c19da != 0) return;
      g_c19da = 1;
      goto go;
    }
    if (s != 0xb) return;
    if (g_c2eeb == 0) return;
    goto go;
  } else if (s == 0x33) {
    if (g_c2d29 == 0 || g_c3319 != 0 || g_c2ffc != 1) return;
    if (g_c2fcc == "" || g_c2fd8 == "") return;
    k = &g_c2fe4;
  } else if (s < 0x34) {
    if (s == 0x2d) {
      if (g_c2d29 == 0 || g_c3319 != 0) return;
      if (g_c2eea == 0) return;
      goto go;
    }
    if (s != 0x30) return;
    if (g_c2d29 == 0 || g_c3319 != 0) return;
    k = &g_c30e0;
    if (g_c30f8 != 1) return;
  } else {
    if (s != 0x39) {
      if (s != 0x3c) return;
      if (g_c19db == 0) to_bigclock();
      return;
    }
    if (g_c2d29 == 0 || g_c3319 != 0 || g_c2efc != 1) return;
    if (g_c2f0c == "") return;
    k = &g_c2f00;
  }
  if (*k == "") return;
go:
  to_bigclock_mark();
}

// @400e3284: те саме для 4 модулів
void content_bigclock_between_4m() {
  int s = g_c34af - 1;
  const String *k;
  if (s == 0x2a) {
    if (g_c2d29 == 0 || g_c3319 != 0) return;
    if (g_c3157 == 2) k = &g_c313c;
    else { if (g_c3157 != 3 && g_c3157 != 1) return; k = &g_c3148; }
  } else if (s < 0x2b) {
    if (s == 0x1c) {
      if ((uint8_t)(g_c32e8 - 1) < 2) to_smallclock_4m_mark();
      if ((uint8_t)(g_c32e7 - 1) < 2) to_smallclock_4m_mark();
      if ((uint8_t)(g_c32e5 - 1) < 2) to_smallclock_4m_mark();
      if (1 < (uint8_t)(g_c32e2 - 1)) return;
      goto go;
    }
    if (0x1c < s) {
      if (s == 0x23) {
        if (g_c2d29 == 0 || g_c3319 != 0 || g_c30c4 != 1) return;
        if (g_c30b8 == "" || g_c30ac == "" || g_c30a0 == "" || g_c3094 == "") return;
      } else {
        if (s != 0x26) return;
        if (g_c2d29 == 0 || g_c3319 != 0 || g_c3139 != 1) return;
      }
      goto go;
    }
    if (g_c34af == 1) {
      g_c19db = 0;
      if (g_c19da != 0) return;
      g_c19da = 1;
      goto go;
    }
    if (s != 0xb) return;
    if (g_c2eeb == 0) return;
    goto go;
  } else if (s == 0x33) {
    if (g_c2d29 == 0 || g_c3319 != 0 || g_c2ffc != 1) return;
    if (g_c2fcc == "" || g_c2fd8 == "") return;
    k = &g_c2fe4;
  } else if (s < 0x34) {
    if (s == 0x2d) {
      if (g_c2d29 == 0 || g_c3319 != 0) return;
      if (g_c2eea == 0) return;
      goto go;
    }
    if (s != 0x30) return;
    if (g_c2d29 == 0 || g_c3319 != 0) return;
    k = &g_c30e0;
    if (g_c30f8 != 1) return;
  } else {
    if (s != 0x39) {
      if (s != 0x3b) return;
      if (g_c19db != 0) return;
      goto go;
    }
    if (g_c2d29 == 0 || g_c3319 != 0 || g_c2efc != 1) return;
    if (g_c2f0c == "") return;
    k = &g_c2f00;
  }
  if (*k == "") return;
go:
  to_smallclock_4m_mark();
}

// @4011af60: інформаційний цикл
void content_cycle_default() {
  lang_check();
  if (g_c34a9 == 2) { g_c34af = 0; g_c34a9 = 3; }
  if (P.displayAnimate()) {
    if (g_c34ba == 8) { content_show_slot_8m(); smallclock_8m(); zone3_info_tick(); }
    else if (g_c34ba == 4) content_show_slot_4m();
  }
  if (g_c34a9 == 3) {
    if (!P.getZoneStatus(0)) return;
    if (g_c34ba == 8) { content_bigclock_between_8m(); return; }
    if (g_c34ba == 4) { content_bigclock_between_4m(); smallclock_switch_4m(); return; }
  }
}

// @4011bd74: тільки великий циферблат (8 модулів)
void content_cycle_8m() {
  clock_tick_2zones();
  if (every_ms_1744(0x19)) {
    sensors_read_all();
    if (g_c34b8 == 8) zone3_info();
    button_tick();
  }
  if (g_c34a9 != 3) goto end;
  if (g_c349a == 0) {
    g_c34a8 = g_c349a;
    g_c3471 = g_c349a;
    if (g_c34b8 == 8) {
      uint8_t a, b;
      if (g_c34b7 == 0) { P.setZone(0, 0, 7); P.setZone(1, 0xc, g_c34b8 * 2 - 1); P.setZone(3, 0, 0); b = 0xb; a = 8; }
      else { P.setZone(0, 0, 7); P.setZone(1, 8, 0xb); P.setZone(3, 0, 0); b = 0xf; a = 0xc; }
      P.setZone(2, a, b);
    }
    display_clear_all();
    g_c34a3 = 0xc;
  } else if (g_c349a == 1) {
    g_c3471 = g_c349a;
    g_c34a8 = 0;
    g_c34a3 = 0xc;
  } else if (g_c349a == 3 && g_c2e72 != 0) {
    g_c3471 = 1;
    g_c34a3 = 0xc;
  }
  if (g_c34a8 == 1) {
    P.displayZoneText(0, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayZoneText(1, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    g_c34a8 = 2;
  }
end:
  g_c2e94 = millis();
}

// @4011bcac: великий циферблат time_show секунд, потім інфо-цикл (8 модулів, «Змішаний»)
void content_cycle_8m_mixed(uint32_t ms) {
  if (millis() - g_c2e94 < ms) {
    clock_tick_2zones();
    if (every_ms_1744(0x19)) {
      sensors_read_all();
      if (g_c34b8 == 8) zone3_info();
      button_tick();
    }
    if ((g_c349a & 0xfd) == 1 && g_c34a8 == 1) {
      P.displayZoneText(0, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      P.displayZoneText(1, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      g_c34a8 = 2;
    }
  } else {
    clockface_effect_select();
    if (!P.getZoneStatus(0)) {
      g_c2e94 = millis();
      g_c34a3 = 0xc;
      g_c34a2 = 0; g_c34a1 = 0; g_c34a0 = 0;
      g_c3471 = 0;
    }
  }
}

// @4011bc2c: тільки малий годинник (4 модулі)
void content_cycle_4m() {
  smallclock_tick();
  button_tick();
  sensors_read_all();
  uint8_t sc = g_c3499;
  if (g_c34a9 == 3) {
    if (g_c3499 == 0) { display_clear_all(); g_c3470 = sc; g_c34a3 = 0xc; }
    else if (g_c3499 == 1 && g_c2e72 != 0) { display_clear_all(); g_c3470 = 0; g_c34a3 = 0xc; }
  }
  if (g_c34a7 == 1) {
    P.displayZoneText(0, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
    g_c34a7 = 2;
  }
  g_c2e94 = millis();
}

// @4011bb9c: малий годинник time_show секунд, потім інфо-цикл (4 модулі)
void content_cycle_4m_mixed(uint32_t ms) {
  if (millis() - g_c2e94 < ms || (g_c3498 == 0x11 && !content_any_enabled())) {
    sensors_read_all();
    smallclock_tick();
    button_tick();
    if (g_c34a7 == 1) {
      P.displayZoneText(0, g_c347d, PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
      g_c34a7 = 2;
    }
  } else {
    clockface_effect_select();
    if (!P.getZoneStatus(0)) {
      g_c2e94 = millis();
      g_c34a3 = 0xc;
      g_c3470 = 0;
    }
  }
}

// @400f8344
void zone3_info_tick() { if (g_c34b8 == 8) zone3_info(); }
