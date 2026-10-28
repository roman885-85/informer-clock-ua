// Час після перезапуску (нове, не з оригіналу).
// EN: Time after restart, time zone and summer time incl. automatic EU rules (new).
#include "fw.h"

bool fx_time_known() { return year() >= 2021; }

// Системний час ESP32 переживає програмний перезапуск (лічильник RTC), а TimeLib — ні: у автора до першої вдалої
// синхронізації годинник жив у 00:00 1 січня 1970 р. Якщо перезапуск стався вночі без Wi-Fi (роутер перезавантажується
// чи вимикає Wi-Fi за розкладом), табло за нічним розкладом (напр. 00:00–05:00) гасло ще на 5 год, а годинник
// стояв точкою доступу — «вранці не вмикається й не видно в мережі». Тепер — одразу системний час (неточний: лічильник
// RTC іде від RC-генератора, похибка — до хвилин за добу роботи), без позначки «синхронізовано»; справжня синхронізація
// NTP — як і раніше, щойно з'явиться інтернет. Після ввімкнення живлення системного часу немає — нічого не робимо.
void fx_time_boot() {
  time_t t = time(nullptr);
  if (t <= 0x5fee65ff) return;
  int32_t off = fx_tz_offset() + fx_dst_offset(t);
  setTime(t + off);
  g_fx_off_applied = off;
  g_fx_off_known = true;
  dbg_ws_print(String("Time        : system clock kept after restart (not synced yet)"));
}

// ---------- пояс і літній час ----------
// Виправлено: у автора пояс (tz) і літній час (sumT, +1 год) враховувались лише в момент синхронізації NTP, тож
// перемикач «Літній час» у веб-інтерфейсі «нічого не робив» (скарга користувача): час мінявся лише за t_syn хв, а без
// інтернету — ніколи; переходити на літній/зимовий час треба було вручну двічі на рік. Тепер: зміна tz/sumT — одразу,
// а sumT = 2 («Авто») — перехід за правилами ЄС (Україна, Молдова, Румунія, Польща, Болгарія…): з 01:00 UTC останньої
// неділі березня до 01:00 UTC останньої неділі жовтня.
bool g_fx_voice_init, g_fx_voice_test;
int32_t g_fx_off_applied;
bool g_fx_off_known;
uint32_t g_fx_dst_t;

int32_t fx_tz_offset() { return (int32_t)(g_c3430 * 3600.0); }

// 00:00 UTC останньої неділі місяця m (1..12) року y
static time_t last_sunday(int y, int m) {
  tmElements_t e = {};
  e.Day = 1;
  if (m == 12) { e.Month = 1; e.Year = y + 1 - 1970; }
  else { e.Month = m + 1; e.Year = y - 1970; }
  time_t last = makeTime(e) - SECS_PER_DAY;          // останній день місяця
  return last - (time_t)(weekday(last) - 1) * SECS_PER_DAY;   // TimeLib: 1 — неділя
}

int32_t fx_dst_offset(time_t utc) {
  if (g_c32e4 == 1) return 0xe10;
  if (g_c32e4 != 2) return 0;
  int y = year(utc);
  time_t a = last_sunday(y, 3) + 3600, b = last_sunday(y, 10) + 3600;
  return (utc >= a && utc < b) ? 0xe10 : 0;
}

void fx_time_offset_set() {
  int32_t tz = fx_tz_offset();
  g_fx_off_applied = tz + fx_dst_offset(now() - tz);   // місцевий → UTC з точністю до години літнього часу — досить
  g_fx_off_known = true;
}

void fx_time_offset_apply() {
  if (!g_fx_off_known || !fx_time_known()) return;
  time_t utc = now() - g_fx_off_applied;
  int32_t off = fx_tz_offset() + fx_dst_offset(utc);
  if (off == g_fx_off_applied) return;
  setTime(utc + off);
  dbg_ws_print(String("Time        : offset ") + String((long)g_fx_off_applied / 60) + String(" -> ") + String((long)off / 60) +
               String(" min (time zone / summer time)"));
  evlog_write(String("[TIME] offset ") + String((long)off / 60) + String(" min (time zone / summer time)"));
  g_fx_off_applied = off;
}
