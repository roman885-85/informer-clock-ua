// Зони 2 і 3 двоповерхового циферблата (8 модулів у два ряди, режим g_c34a9 = 3): напис/дата й показання датчиків.
// EN: Zones 2 and 3 of the two-floor clock face (8 modules in two rows): caption/date and sensor readings (from 2.7.1).
// Перенесено з 2.7.1 (адреси оригіналу в коментарях).
#include "fw.h"

static void z3_label(String &out, uint32_t ms, const char *a, const char *b) {
  g_c3488 = ms;
  out = String(a) + String(b);
}
static void z3_temp(String &out, uint32_t ms, float t) {   // гліфи: термометр, °, C
  g_c3488 = ms;
  out = String("\x1b ") + String(t, 1) + String("\x1c") + String("\x1d");
}
static void z3_hum(String &out, uint32_t ms, float h) {    // гліф: крапля
  g_c3488 = ms;
  out = String("\x1e ") + String(h, 0) + String("%");
}
static void z3_press(String &out, uint32_t ms, float p) {  // гліфи: барометр, «мм»
  g_c3488 = ms;
  out = String("\x12") + String(p, 0) + String("\x14");
}
// @400ed90c: текст пункту i зони 3 (3–17; інакше порожньо), тривалість показу — g_c3488
void zone3_sensor_text(int8_t i, String &out) {
  if (g_c34a9 == 3 && g_c34ba == 8 && g_c34b8 == 8) {
    switch (i) {
      case 3:
        if (g_c32e8 != 0) {
          sensor_ds18b20();
          if (g_c3300 != -127.0) { z3_label(out, 3000, "DS", "\x09"); return; }
        }
        break;
      case 4:
        if (g_c32e8 != 0 && g_c3300 != -127.0) { z3_temp(out, 7000, g_c3300); return; }
        break;
      case 5:
        if (g_c32e5 != 0) {
          sensor_bosch();
          if (g_c2ee5 == 0) {
            if (g_c3314 != 0.0 && g_c330c != 0.0 && g_c3310 != 0.0) { z3_label(out, 1500, "BME", " \x13"); return; }
          } else if (g_c3314 != 0.0 && g_c330c != 0.0) { z3_label(out, 2000, "BMP", " \x13"); return; }
        }
        break;
      case 6:
        if (g_c32e5 != 0) {
          if (g_c2ee5 == 0) {
            if (g_c3314 != 0.0 && g_c330c != 0.0 && g_c3310 != 0.0) { z3_temp(out, 3000, g_c3314); return; }
          } else if (g_c3314 != 0.0 && g_c330c != 0.0) { z3_temp(out, 4000, g_c3314); return; }
        }
        break;
      case 7:
        if (g_c32e5 != 0 && g_c2ee5 == 0 && g_c3314 != 0.0 && g_c330c != 0.0 && g_c3310 != 0.0) {
          g_c3488 = 3000;
          out = String("\x1e ") + String(round(g_c3310), 0) + String("%");
          return;
        }
        break;
      case 8:
        if (g_c32e5 != 0) {
          if (g_c2ee5 == 0) {
            if (g_c3314 != 0.0 && g_c330c != 0.0 && g_c3310 != 0.0) { z3_press(out, 2500, g_c330c); return; }
          } else if (g_c3314 != 0.0 && g_c330c != 0.0) { z3_press(out, 4000, g_c330c); return; }
        }
        break;
      case 9:
        if (g_c32e7 != 0) {
          sensor_dht();
          if (String(g_c3308, 0) != String("nan")) { z3_label(out, 2000, "DHT ", "\x0d"); return; }
        }
        break;
      case 10:
        if (g_c32e7 != 0 && String(g_c3308, 0) != String("nan")) { z3_temp(out, 4000, g_c3308); return; }
        break;
      case 11:
        if (g_c32e7 != 0 && String(g_c3308, 0) != String("nan")) { z3_hum(out, 4000, g_c3304); return; }
        break;
      case 12:
        if (g_c32e2 != 0) {
          sensor_other();
          if (g_c32e3 == 1) {
            if (g_c32fc != 0.0 && g_c32fc < 100.0 && g_c32f8 != 0.0) { z3_label(out, 2000, "HDC ", "\x13"); return; }
          } else if (g_c32e3 == 0) {
            if (String(g_c32fc, 0) != String("nan")) { z3_label(out, 2000, "AM ", "\x0d"); return; }
          } else if (g_c32e3 == 2) {
            if (String(g_c32fc, 0) != String("nan")) { z3_label(out, 2000, "AHT ", "\x13"); return; }
          }
        }
        break;
      case 13:
        if (g_c32e2 != 0) {
          if (g_c32e3 == 1) {
            if (g_c32fc != 0.0 && g_c32fc < 100.0 && g_c32f8 != 0.0) { z3_temp(out, 4000, g_c32fc); return; }
          } else if (g_c32e3 == 0 || g_c32e3 == 2) {
            if (String(g_c32fc, 0) != String("nan")) { z3_temp(out, 4000, g_c32fc); return; }
          }
        }
        break;
      case 14:
        if (g_c32e2 != 0) {
          if (g_c32e3 == 1) {
            if (g_c32fc != 0.0 && g_c32fc < 100.0 && g_c32f8 != 0.0) { z3_hum(out, 4000, g_c32f8); return; }
          } else if (g_c32e3 == 0 || g_c32e3 == 2) {
            if (String(g_c32fc, 0) != String("nan")) { z3_hum(out, 4000, g_c32f8); return; }
          }
        }
        break;
      case 15:
        if (g_c32e1 != 0) {
          sensor_air();
          if (g_c32e0 == 0) {
            if (g_c1ba4 != 0) { z3_label(out, 2000, "SGP30 ", "\x13"); return; }
          } else if (g_c32e0 < 5) {
            return;   // інші датчики повітря: зона не змінюється
          }
        }
        break;
      case 16:
        if (g_c32e1 != 0) {
          if (g_c32e0 != 0) {
            if (g_c32e0 < 5) return;
            break;
          }
          if (g_c1ba4 == 0) return;
          g_c3488 = 4000;
          out = String("TVOC ") + String(g_c32f4, 10) + String("ppb");
          return;
        }
        break;
      case 17:
        if (g_c32e1 != 0) {
          if (g_c32e0 != 0) {
            if (g_c32e0 < 5) return;
            break;
          }
          if (g_c1ba4 != 0) {
            g_c3488 = 4000;
            out = String("eCO2 ") + String(g_c32f0, 10) + String("ppm");
            return;
          }
        }
        break;
    }
  }
  g_c3488 = 0;
  out = "";
}
// @400f7e7c: текст зони 2 (0 — напис-фраза, 1 — дата ДД.ММ.РР), тривалість — g_c348c
void zone2_text(int8_t which, String &out) {
  if (g_c34a9 == 3 && g_c34ba == 8 && g_c34b8 == 8) {
    if (which == 0) {
      g_c348c = 2000;
      String p; led_phrase(p, g_c34b0, 0x70);
      out = String("\x11") + p;
      return;
    }
    if (which == 1) {
      g_c348c = 7000;
      String d, m, y;
      day2_text(d);
      month2_text(m);
      year2_text(y);
      out = d + "." + m + "." + y;
      return;
    }
  }
  g_c348c = 0;
  out = "";
}
// @400f8034: зона 2
void zone2_tick() {
  String t("");
  static String last;
  if (g_c3471 == 2) {
    zone2_text(0, t);
    if (t != last) {
      last = t;
      utf8_to_led(t, g_c27e5, 10);
      P.setFont(2, (MD_MAX72XX::fontType_t *)font_small3px);
      P.displayZoneText(2, g_c27e5, (textPosition_t)1, 0, 0, (textEffect_t)1, (textEffect_t)0);
      P.displayReset(2);
    }
  } else if (P.getZoneStatus(2)) {
    zone2_text(g_c27e0, t);
    utf8_to_led(t, g_c27e5, 10);
    P.setFont(2, (MD_MAX72XX::fontType_t *)font_small3px);
    P.displayZoneText(2, g_c27e5, (textPosition_t)1, 0x1e, (uint16_t)(g_c348c + 0x32), (textEffect_t)3, (textEffect_t)2);
    P.displayReset(2);
    if (g_c27e0 < 4) g_c27e0++;
    else g_c27e0 = 0;
  }
}
// @400f81ac: зона 3 (і зона 2)
void zone3_info() {
  String t("");
  static String last;
  zone2_tick();
  if (g_c3471 == 1) {
    if (P.getZoneStatus(3)) {
      zone3_sensor_text(g_c27e1, t);
      if (t != "") {
        utf8_to_led(t, g_c27ef, 10);
        P.setFont(3, (MD_MAX72XX::fontType_t *)font_small3px);
        P.displayZoneText(3, g_c27ef, (textPosition_t)1, 0x1e, (uint16_t)(g_c3488 - 0xfa), (textEffect_t)2, (textEffect_t)3);
        P.displayReset(3);
      }
      if (g_c27e1 < 0x12) g_c27e1++;
      else g_c27e1 = 0;
    }
  } else if (g_c3471 == 2) {
    zone2_text(1, t);
    if (t != last) {
      last = t;
      P.setFont(3, (MD_MAX72XX::fontType_t *)font_small3px);
      utf8_to_led(t, g_c27ef, 10);
      P.displayZoneText(3, g_c27ef, (textPosition_t)1, 0, 0, (textEffect_t)1, (textEffect_t)0);
      P.displayReset(3);
    }
  }
}
