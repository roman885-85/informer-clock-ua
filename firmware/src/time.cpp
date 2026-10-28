// Час: NTP, RTC DS3231, ручне встановлення, тексти дат (перенесено з 2.7.1)
#include "fw.h"

static String two(uint8_t v) { return v < 10 ? String("0") + String(v, 10) : String(v, 10); }
// рядок «ГГ:ХХ:СС - ДД.ММ.РРРР» з полів останньої синхронізації NTP (повторюється в кількох функціях оригіналу)
static String ntp_stamp() {
  String t; time_hm_text(t, 2);
  return t + ":" + String(g_c3460 / 10) + String((uint32_t)g_c3460 % 10) + " - " + two(g_c345d) + "." + two(g_c345c) + "." + String(g_c3458);
}
static String rtc_stamp(const DateTime &rt) {
  String t; time_hm_text(t, 3);
  uint8_t ss = rt.second(), dd = rt.day(), mm = rt.month(), yo = rt.year() - 2000;
  return t + ":" + String(ss / 10) + String((uint32_t)ss % 10) + " - " + two(dd) + "." + two(mm) + "." + String(yo + 2000);
}

// @400db554: запит NTP (configTime) і очікування до 5 с
void ntp_request() {
  float f = g_c3430 * 3600.0;
  int dst = 0;
  if (g_c32e4 != 0) dst = 0xe10;
  int off = (int)f;
  const char *srv = "pool.ntp.org";
  if (g_c3434.length() != 0) srv = g_c3434.c_str();
  g_c22b5 = 0;
  g_c3464 = 0;
  configTime(dst + off, 0, srv, "time.nist.gov", 0);
  time_t t = time(nullptr);
  uint32_t t0 = millis();
  while (true) {
    if (0x5fee65ff < t) {
      g_c3464 = t + dst + off;
      dbg_ws_print(String("NTP server - data received, unix=") + String((long)g_c3464, 10));
      g_c22b5 = 1;
      g_c2eb4 = 1;
      return;
    }
    if (millis() - t0 > 4999 || g_c2d29 == 0 || g_c3319 != 0) {
      dbg_ws_print(String("NTP server error"));
      return;
    }
    delay(100);
    t = time(nullptr);
  }
}
// @400db6e8 / @400db7c4 / @400db890: день, місяць (з нулем), рік (2 цифри)
void day2_text(String &out) {
  if (g_c342c == 1 || 9 < day()) out = String(day());
  else out = String("0") + String(day());
}
void month2_text(String &out) {
  if (month() < 10) out = String("0") + String(month());
  else out = String(month());
}
void year2_text(String &out) {
  String t = String(year());
  out = t.substring(2, 4);
}
// @400db8f0: розібрати ручний час t_m (ГГ:ХХ) і дату d_m (РРРР-ММ-ДД)
void manual_time_print() {
  g_c22c0 = g_c344c.substring(0, g_c344c.length() - 3).toInt();
  g_c22bf = g_c344c.substring(3, 5).toInt();
  g_c22be = g_c3440.substring(8, 10).toInt();
  g_c22bd = g_c3440.substring(5, 7).toInt();
  g_c22b8 = g_c3440.substring(0, 4).toInt();
  dbg_ws_print(String("Manual Time: ") + String(g_c22c0, 10) + String(":") + String(g_c22bf, 10) + String("  Date: ") + String(g_c22be, 10) +
               String(".") + String(g_c22bd, 10) + String(".") + String(g_c22b8));
}
// @400dbc40 / @400dbcf0: дати нагадувань d1_rem / d2_rem
void rem1_date_parse() {
  g_c2ee1 = g_c2ec8.substring(8, 10).toInt();
  g_c2ee0 = g_c2ec8.substring(5, 7).toInt();
  g_c2edc = g_c2ec8.substring(0, 4).toInt();
}
void rem2_date_parse() {
  g_c2ed9 = g_c2ebc.substring(8, 10).toInt();
  g_c2ed8 = g_c2ebc.substring(5, 7).toInt();
  g_c2ed4 = g_c2ebc.substring(0, 4).toInt();
}
// @400dbd9c: встановити час вручну (і в RTC, якщо є)
void manual_time_set() {
  manual_time_print();
  g_c22d0 = g_c344c;
  g_c22c4 = g_c3440;
  setTime(g_c22c0, g_c22bf, 0, g_c22be, g_c22bd, g_c22b8);
  if (g_c3468 != 0) {
    DateTime d(g_c22b8 & 0xffff, g_c22bd, g_c22be, g_c22c0, g_c22bf, 0);
    g_c346c.adjust(d);
  }
  g_c3408 = String(g_c22c0, 10) + ":" + String(g_c22bf, 10) + ":00" + " - " + String(g_c22be, 10) + "." + String(g_c22bd, 10) + "." + String(g_c22b8);
  dbg_ws_print(String(g_c3468 == 0 ? "=== Manual time set in SYS ===" : "=== Manual time set in RTC ==="));
}
// @400dc0f4: «ГГ:ХХ»: 1 — системний час, 2 — останній NTP, 3 — RTC
void time_hm_text(String &out, int8_t mode) {
  if (mode == 2) {
    out = String(g_c345e / 10) + String((uint32_t)g_c345e % 10) + ":" + String(g_c345f / 10) + String((uint32_t)g_c345f % 10);
  } else if (mode == 3) {
    DateTime rt = g_c346c.now();
    uint8_t h = rt.hour(), m = rt.minute();
    out = String(h / 10) + String((uint32_t)h % 10) + ":" + String(m / 10) + String((uint32_t)m % 10);
  } else if (mode == 1) {
    out = String(hour() / 10) + String(hour() % 10) + ":" + String(minute() / 10) + String(minute() % 10);
  } else out = "";
}
// @400dc43c: чи зараз у проміжку [from, to) хвилин (з переходом через північ)
bool time_in_range(int32_t from, int32_t to) {
  String t; time_hm_text(t, 1);
  int h = t.substring(0, 2).toInt();
  int m = t.substring(3).toInt();
  int now = h * 0x3c + m;
  bool r = false;
  if (now != to && (r = true, now != from)) {
    if ((to < now && from < now) || (now < to && now < from)) r = to < from;
    else r = now <= to || from <= now;
  }
  return r;
}
// @400dc524: RTC ← NTP, якщо не збігаються
void rtc_sync_from_ntp() {
  DateTime rt = g_c346c.now();
  if (g_c18ac == 0) { g_c22d0 = g_c344c; g_c22c4 = g_c3440; g_c18ac = 1; }
  if (g_c3468 == 0) {
    if (g_c22b4 == 0 && g_c22b5 != 0) {
      g_c33fc = ntp_stamp();
      g_c22b4 = 1; g_c22b5 = 0; g_c22bc = 1;
    }
  } else if ((uint8_t)(rt.year() - 2000) + 2000 < 0x874) {
    if (g_c22b4 == 0 && g_c22b5 != 0) {
      if ((int8_t)rt.hour() == (int8_t)g_c345e && g_c345f == (int8_t)rt.minute() && g_c3460 == rt.second() &&
          g_c345d == rt.day() && g_c345c == rt.month() && (uint8_t)(rt.year() - 2000) + 2000 == g_c3458) {
        g_c22bc = 1;
        dbg_ws_print(String("=== RTC & NTP already in sync ==="));
      } else {
        DateTime d(g_c3458 & 0xffff, g_c345c, g_c345d, g_c345e, g_c345f, g_c3460);
        g_c346c.adjust(d);
        g_c3414 = ntp_stamp();
        dbg_ws_print(String("=== RTC synced from NTP ==="));
        g_c22bc = 1;
      }
      g_c22b5 = 0;
      g_c22b4 = 1;
    } else g_c22bc = 1;
  }
}
// @400dce60: RTC втратив живлення / робота без мережі за RTC (кожні interval хв)
void rtc_lost_power_sync(int32_t interval) {
  DateTime rt = g_c346c.now();
  uint32_t t0 = g_c18a0;
  if (g_c2d29 == 0 || g_c3319 != 0) g_c18a4 = 0;
  if (g_c18a4 <= millis() - t0) {
    g_c18a0 = millis();
    if (g_c346c.lostPower()) {
      dbg_ws_print(String("=== RTC lost power (battery) ==="));
      if (g_c22b5 != 0) {
        dbg_ws_print(String("=== RTC lost power — NTP packet received, setting RTC from NTP ==="));
        DateTime d(g_c3458 & 0xffff, g_c345c, g_c345d, g_c345e, g_c345f, g_c3460);
        g_c346c.adjust(d);
        g_c3414 = ntp_stamp();
      }
    }
    if (g_c2d29 == 0 || g_c3319 != 0) {
      uint8_t yo = rt.year() - 2000;
      if ((uint16_t)(yo - 2) < 0x61 && rt.hour() < 0x18) {
        if (g_c22bc == 1) {
          dbg_ws_print(String("=== SYS synced from RTC (offline) ==="));
          setTime(rt.hour(), rt.minute(), rt.second(), rt.day(), rt.month(), yo + 2000);
          g_c18a4 = interval * 60000;
        } else g_c18a4 = 0;
      }
      g_c22b4 = 0;
      g_c22b5 = 0;
      g_c3408 = rtc_stamp(rt);
    } else g_c18a4 = 0;
  }
}
// @400dd7c8: синхронізація системного часу з NTP
void sys_sync_ntp() {
  if (g_c2d29 == 0 || g_c3319 != 0) { dbg_ws_print(String("NTP        : offline mode, skipping time sync")); return; }
  if (g_c30c5 != 0) {
    for (int i = 0; i < 12; i++) g_c2628.unsubscribe(g_c2500[i].c_str());
    if (WiFi.isConnected() && g_c3319 == 0) g_bed1c = 1;
    g_c2628.disconnect(true);
  }
  ntp_request();
  if (g_c22b5 == 0) return;
  g_c3458 = year(g_c3464);
  g_c345c = month(g_c3464);
  g_c345d = day(g_c3464);
  g_c345e = hour(g_c3464);
  g_c345f = minute(g_c3464);
  g_c3460 = second(g_c3464);
  dbg_ws_print(String("=== SYS synced from NTP ==="));
  setTime(g_c345e, g_c345f, g_c3460, g_c345d, g_c345c, g_c3458);
  if (g_c3468 != 0) rtc_lost_power_sync(1);
  rtc_sync_from_ntp();
  g_c33fc = ntp_stamp();
  g_c3414 = ntp_stamp();
}
// @400de100: періодична синхронізація (t_syn хв, за замовчуванням 60)
void ntp_periodic(int32_t minutes) {
  uint32_t t0 = g_c18a8;
  int m = 0x3c;
  if (0 < minutes) m = minutes;
  if ((uint32_t)(m * 60000) <= millis() - t0) {
    g_c18a8 = millis();
    g_c22b4 = 0; g_c22b5 = 0;
    sys_sync_ntp();
  }
}
// @400de138: виявлення RTC при старті й періодична синхронізація
void rtc_detect_sync() {
  if (g_c18ad == 0) {
    if (g_c33fc == "") sys_sync_ntp();
    if (g_c3468 == 0) dbg_ws_print(String("=== RTC not connected ==="));
    else {
      DateTime rt = g_c346c.now();
      dbg_ws_print(String("=== RTC connected ==="));
      uint8_t yo = rt.year() - 2000;
      if ((uint16_t)(yo - 2) < 0x61 && rt.hour() < 0x18 && (g_c3319 != 0 || g_c2d29 == 0)) {
        setTime(rt.hour(), rt.minute(), rt.second(), rt.day(), rt.month(), yo + 2000);
        g_c3408 = rtc_stamp(rt);
        dbg_ws_print(String("=== SYS synced from RTC ==="));
      }
    }
    g_c18ad = 1;
  }
  ntp_periodic(g_c3461);
  rtc_sync_from_ntp();
  if (g_c3468 != 0) rtc_lost_power_sync(5);
}
