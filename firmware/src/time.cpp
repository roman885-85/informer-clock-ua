// Час: NTP, RTC DS3231, ручне встановлення, тексти дат (перенесено з 2.7.1)
// EN: Time: NTP, RTC DS3231, manual setting, date texts (from 2.7.1; NTP now waits for a fresh reply).
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

// @400db554: запит NTP; g_c3464 — місцевий час (секунди), g_c22b5 = 1 — отримано.
// Виправлено (1.1.0): автор вважав «отриманим від NTP» будь-який системний час після 2021 р. — а системний час переживає
// програмний перезапуск (іде від неточного RC-генератора RTC), тож після кожного перезапуску годинник одразу брав
// старий час за синхронізований і до наступної синхронізації (t_syn хв) показував його з похибкою; потім — стрибок
// (скарги користувачів: «время прыгает на 10–20 минут»).
// Виправлено ще раз (1.1.6): виправлення 1.1.0 чекало свіжої відповіді лише на першому проході — далі
// t = time(nullptr) у кінці кожного проходу, і через 100 мс бралось системне, не свіже; а системний час ставив клієнт
// SNTP ядра (configTime), що приймає будь-яку відповідь без перевірок. 14.11.2026 так годинник став на 12 год 24 хв
// назад. Тепер — власний клієнт NTP з перевірками відповіді й другим сервером для стрибків (ntp.cpp); SNTP ядра не
// запускається.
static String s_ntp_src;                           // звідки прийнято час — для рядка журналу подій (sys_sync_ntp)
void ntp_request() {
  const char *srv = "pool.ntp.org";
  if (g_c3434.length() != 0) srv = g_c3434.c_str();
  g_c22b5 = 0;
  g_c3464 = 0;
  uint32_t utc;
  int8_t r = (g_c2d29 == 0 || g_c3319 != 0) ? -1 : fx_ntp_sync(srv, utc, s_ntp_src);
  if (r != 1) {
    dbg_ws_print(String(r < 0 ? "NTP server error" : "NTP        : reply rejected, time kept"));
    return;
  }
  int32_t off = fx_tz_offset() + fx_dst_offset((time_t)utc);   // sumT 2 — авто (timefix.cpp)
  g_c3464 = utc + off;
  g_fx_off_applied = off;
  g_fx_off_known = true;
  dbg_ws_print(String("NTP server - data received, unix=") + String((long)g_c3464, 10));
  g_c22b5 = 1;
  g_c2eb4 = 1;
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
  {
    tmElements_t e;                                  // 1.1.6: без спільного кешу TimeLib (кличеться й з веб-сервера)
    e.Second = 0; e.Minute = g_c22bf; e.Hour = g_c22c0; e.Day = g_c22be; e.Month = g_c22bd;
    e.Year = g_c22b8 > 99 ? g_c22b8 - 1970 : g_c22b8 + 30;   // як setTime(год, хв, с, д, м, р) TimeLib
    fx_time_set_local(makeTime(e), String("manual (t_m/d_m)"));   // стрибок понад хвилину — у журнал подій
  }
  fx_time_offset_set();                              // нове: подальші переходи літнього часу — від цього часу
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
          fx_time_set_local(rt.unixtime(), String("RTC (offline)"));   // 1.1.6: стрибок понад хвилину — у журнал подій
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
  // Виправлено (1.1.4): автор перед кожною синхронізацією часу відписувався від усіх 12 топіків і рвав з'єднання MQTT.
  // Відписку від порожнього топіка брокер вважає зіпсованим пакетом і сам рве з'єднання (Mosquitto, 07.11: «Empty
  // unsubscription string … disconnected: malformed packet»), а розрив публікує заповіт «offline»: у Home Assistant усі
  // сутності годинника кожні t_syn хв (у власника — 10) на 2–7 с ставали «недоступні». Клієнт MQTT працює в задачі
  // AsyncTCP і запиту NTP не заважає — з'єднання не чіпаємо.
  ntp_request();
  if (g_c22b5 == 0) return;
  // 1.1.6: поля — через власну структуру, час — setTime(time_t). У автора year(t)…second(t) і setTime(год, хв, …)
  // ішли через спільний кеш TimeLib (tm), куди паралельно пише веб-сервер (потік AsyncTCP: /content?sys_time= тощо).
  tmElements_t e;
  breakTime((time_t)g_c3464, e);
  g_c3458 = e.Year + 1970;
  g_c345c = e.Month;
  g_c345d = e.Day;
  g_c345e = e.Hour;
  g_c345f = e.Minute;
  g_c3460 = e.Second;
  dbg_ws_print(String("=== SYS synced from NTP ==="));
  fx_time_set_local((time_t)g_c3464, s_ntp_src);  // стрибок понад хвилину — у журнал подій
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
        fx_time_set_local(rt.unixtime(), String("RTC"));   // 1.1.6: стрибок понад хвилину — у журнал подій
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
