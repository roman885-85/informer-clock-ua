// Заглушка fw.h для імітатора часу (tools/sim/ntpsim): справжні time.cpp і timefix.cpp (і ntp.cpp з 1.1.6) прошивки
// збираються на комп'ютері зі справжньою TimeLib 1.6.1 і справжнім String ядра; системний час ESP32, мережа й сервери
// NTP — модель (world.cpp). fix.h — справжній, тієї версії, що перевіряється (кладеться поруч).
#pragma once
#include <Arduino.h>
#include "TimeLib.h"

// системний час ESP32 (gettimeofday/settimeofday) — модельний; у прошивці settimeofday обгорнуто (netfix.cpp: лічильник)
time_t sim_time(time_t *p);
int sim_settimeofday(const struct timeval *tv, const void *tz);
#define time(p) sim_time(p)
#define settimeofday(tv, tz) sim_settimeofday(tv, tz)

// RTClib: годинника DS3231 у власника немає (/content?rtc_detect = 0) — лише щоб зібрався код
class DateTime {
  uint32_t t;
public:
  DateTime(uint32_t u = 0) : t(u) {}
  DateTime(uint16_t y, uint8_t m, uint8_t d, uint8_t hh = 0, uint8_t mm = 0, uint8_t ss = 0) {
    tmElements_t e; e.Year = y - 1970; e.Month = m; e.Day = d; e.Hour = hh; e.Minute = mm; e.Second = ss;
    t = (uint32_t)makeTime(e);
  }
  uint16_t year() const { tmElements_t e; breakTime(t, e); return e.Year + 1970; }
  uint8_t month() const { tmElements_t e; breakTime(t, e); return e.Month; }
  uint8_t day() const { tmElements_t e; breakTime(t, e); return e.Day; }
  uint8_t hour() const { tmElements_t e; breakTime(t, e); return e.Hour; }
  uint8_t minute() const { tmElements_t e; breakTime(t, e); return e.Minute; }
  uint8_t second() const { tmElements_t e; breakTime(t, e); return e.Second; }
  uint32_t unixtime() const { return t; }
};
class RTC_DS3231 {
public:
  DateTime now() { return DateTime((uint32_t)0); }
  void adjust(const DateTime &) {}
  bool lostPower() { return false; }
};

// глобальні змінні прошивки (g.h), потрібні time.cpp / timefix.cpp — ті самі типи
extern uint32_t g_c18a0, g_c18a4, g_c18a8, g_c22b8, g_c2ed4, g_c2edc, g_c3458, g_c3464;
extern uint8_t g_c18ac, g_c18ad, g_c22b4, g_c22b5, g_c22bc, g_c22bd, g_c22be, g_c22bf, g_c22c0, g_c2d29, g_c2eb4, g_c2ed8,
    g_c2ed9, g_c2ee0, g_c2ee1, g_c32e4, g_c3319, g_c342c, g_c345c, g_c345d, g_c345e, g_c345f, g_c3460, g_c3461, g_c3468;
extern String g_c22c4, g_c22d0, g_c2ebc, g_c2ec8, g_c33fc, g_c3408, g_c3414, g_c3434, g_c3440, g_c344c;
extern float g_c3430;
extern RTC_DS3231 g_c346c;

// функції інших модулів прошивки
void dbg_ws_print(const String &s);
void evlog_write(const String &msg);
// прототипи time.cpp (protos.h прошивки)
void ntp_request();
void day2_text(String &out);
void month2_text(String &out);
void year2_text(String &out);
void manual_time_print();
void rem1_date_parse();
void rem2_date_parse();
void manual_time_set();
void time_hm_text(String &out, int8_t mode);
bool time_in_range(int32_t from, int32_t to);
void rtc_sync_from_ntp();
void rtc_lost_power_sync(int32_t interval);
void sys_sync_ntp();
void ntp_periodic(int32_t minutes);
void rtc_detect_sync();

#include "fix.h"
