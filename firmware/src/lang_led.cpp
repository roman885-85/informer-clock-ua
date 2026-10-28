// Фрази табло: мовний пакет /lang/<id>.led (формат LED1) і англійські фрази за замовчуванням; назви днів і місяців.
// Перенесено з 2.7.1.
#include "fw.h"

// @400d53dc
void led_pack_path(String &out, uint8_t lang) { String t("/lang/"); t += (int)lang; t += ".led"; out = t; }

// @400f5e1c: розібрати заголовок пакета (LED1, ревізія u32, кількість u16, таблиця зсувів u16)
void led_pack_parse(int8_t lang) {
  if (g_bf040 == lang) return;
  g_c2e48 = 0; g_c2e40 = 0; g_c2e4c = 0;
  g_bf040 = lang;
  if (g_c2e44 != nullptr) { free(g_c2e44); g_c2e44 = nullptr; }
  if (lang == 1) return;
  String p; led_pack_path(p, lang);
  g_c2e34 = p;
  File f = SPIFFS.open(g_c2e34, "r");
  if (f) {
    uint32_t rev = 0; uint16_t cnt = 0; char mg[4];
    if (f.read((uint8_t *)mg, 4) == 4 && memcmp(mg, "LED1", 4) == 0 && f.read((uint8_t *)&rev, 4) == 4 &&
        f.read((uint8_t *)&cnt, 2) == 2 && ((cnt - 1) & 0xffff) < 0x200) {
      size_t sz = (uint32_t)cnt << 1;
      void *t = malloc(sz);
      if (t != nullptr) {
        if (f.read((uint8_t *)t, sz) == (int)((uint32_t)cnt << 1)) {
          f.close();
          g_c2e48 = cnt;
          g_c2e40 = (cnt + 5) * 2;
          g_c2e4c = rev;
          g_c2e44 = (uint16_t *)t;
          return;
        }
        free(t);
      }
    }
    f.close();
  }
}
// @400f5fb0: фраза з пакета; 0 — немає
int led_phrase_from_pack(uint8_t lang, uint32_t id, String &out) {
  led_pack_parse(lang);
  if (g_c2e44 == nullptr) return 0;
  uint32_t off;
  if (g_c2e48 <= (id & 0xff) || (off = g_c2e44[id & 0xff]) == 0xffff) return 0;
  File f = SPIFFS.open(g_c2e34, "r");
  if (!f) return 0;
  f.seek(off + g_c2e40, SeekSet);
  uint16_t len = 0;
  if (f.read((uint8_t *)&len, 2) != 2 || 0xfe < (uint16_t)(len - 1)) { f.close(); return 0; }
  char buf[0x100];
  uint32_t r = f.read((uint8_t *)buf, len);
  f.close();
  if (r != len) return 0;
  buf[r] = 0;
  out = String(buf);
  return 1;
}
// @400f613c: фраза за id (мова 1 — англійська вбудована)
void led_phrase(String &out, uint8_t lang, uint8_t id) {
  if (lang != 1) {
    String t("");
    if (led_phrase_from_pack(lang, id, t)) { out = t; return; }
  }
  const char *p;
  switch (id) {
    case 2: p = "Currency"; break;
    case 0x28: p = " EUR"; break;
    case 0x2e: p = " RON"; break;
    case 0x31: p = ""; break;
    case 0x32: p = " uah "; break;
    case 0x33: p = " B.rub "; break;
    case 0x34: p = " tenge "; break;
    case 0x35: p = " rub "; break;
    case 0x3d: p = "Northern"; break;
    case 0x3e: p = "NE"; break;
    case 0x3f: p = "Eastern"; break;
    case 0x40: p = "SE"; break;
    case 0x41: p = "South"; break;
    case 0x42: p = "SW"; break;
    case 0x43: p = "West"; break;
    case 0x44: p = "NW"; break;
    case 0x45: p = "Now"; break;
    case 0x46: p = " in "; break;
    case 0x47: p = " Humidity "; break;
    case 0x48: p = "Pressure "; break;
    case 0x49: p = "Wind "; break;
    case 0x4a: p = " m/s"; break;
    case 0x4b: p = " mm Hg "; break;
    case 0x4c: p = "Forecast for tomorrow: "; break;
    case 0x4d: p = "from "; break;
    case 0x50: p = "News"; break;
    case 0x5a: p = "Description"; break;
    case 0x5b: p = "Subscribers: "; break;
    case 0x5c: p = "Views: "; break;
    case 0x70: p = "DATE"; break;
    case 0x7c: p = " to "; break;
    case 0x84: p = "Quotes"; break;
    case 0x85: p = "Quote: "; break;
    case 0x8f: p = "of January"; break;
    case 0x90: p = "of February"; break;
    case 0x91: p = "of March"; break;
    case 0x92: p = "of April"; break;
    case 0x93: p = "of May"; break;
    case 0x94: p = "of June"; break;
    case 0x95: p = "of July"; break;
    case 0x96: p = "of August"; break;
    case 0x97: p = "of September"; break;
    case 0x98: p = "of October"; break;
    case 0x99: p = "of November"; break;
    case 0x9a: p = "of December"; break;
    case 0x9b: p = "Monday"; break;
    case 0x9c: p = "Tuesday"; break;
    case 0x9d: p = "Wednesday"; break;
    case 0x9e: p = "Thursday"; break;
    case 0x9f: p = "Friday"; break;
    case 0xa0: p = "Saturday"; break;
    case 0xa1: p = "Sunday"; break;
    case 0xa9: p = "Firmware Ver: "; break;
    case 0xaa: p = "    Actual: "; break;
    case 0xab: p = "      Let\'s start."; break;
    case 0xac: p = "Connected to: "; break;
    case 0xad: p = "Your IP: "; break;
    case 0xae: p = "No WI-FI connection. Please connect to the \'"; break;
    case 0xaf: p = "\' access point and insert 192.168.4.1 in your browser for configuration"; break;    default: out = ""; return;
  }
  out = String(p);
}
// @400f634c / @400f63c4
void month_name(String &out, int8_t m) {
  if ((uint8_t)(m - 1) > 11) { out = ""; return; }
  led_phrase(out, g_c34b0, 0x8f + (m - 1));
}
void weekday_name(String &out, int8_t wd) {
  static const uint8_t ID[7] = { 0xa1, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, 0xa0 };
  if ((uint8_t)(wd - 1) > 6) { out = ""; return; }
  led_phrase(out, g_c34b0, ID[wd - 1]);
}
// @400f6ac4
void weekday_text(String &out) { weekday_name(out, weekday()); }
// @400f641c: рядок дати: 1 — системна, 2 — остання NTP, 3 — RTC
void date_text(String &out, int8_t mode) {
  if (mode == 2) {
    String m; month_name(m, g_c345c);
    out = String(g_c345d, 10) + " " + m + " " + String(g_c3458);
  } else if (mode == 3) {
    DateTime rt = g_c346c.now();
    int wd = rt.dayOfTheWeek();
    uint8_t w = 1;
    if (wd != 7) w = (wd + 1) & 0xff;
    String wn; weekday_name(wn, w);
    String m; month_name(m, rt.month());
    out = wn + " " + String(rt.day(), 10) + " " + m + " " + String((uint8_t)(rt.year() - 2000) + 2000);
  } else if (mode == 1) {
    String wn; weekday_name(wn, weekday());
    String m; month_name(m, month());
    out = wn + " " + String(day()) + " " + m + " " + String(year());
  } else out = "";
}
