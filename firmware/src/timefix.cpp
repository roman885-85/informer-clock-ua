// Час після перезапуску (нове, не з оригіналу).
// EN: Time after restart, time zone and summer time incl. automatic EU rules (new).
#include "fw.h"

bool fx_time_known() { return year() >= 2021; }

// «ГГ:ХХ:СС ДД.ММ.РРРР». breakTime — у власну структуру: hour(t)/day(t)… TimeLib пишуть спільний кеш tm, і з двох
// задач (цикл і AsyncTCP) поля могли б змішатись.
String fx_time_text(time_t t) {
  tmElements_t e;
  breakTime(t, e);
  char b[24];
  snprintf(b, sizeof b, "%02u:%02u:%02u %02u.%02u.%04u", e.Hour, e.Minute, e.Second, e.Day, e.Month, (unsigned)e.Year + 1970);
  return String(b);
}

// Нове (1.1.6): усі встановлення показаного часу (NTP, вручну, RTC) — через цю функцію. Зміна відомого часу більш ніж
// на хвилину — рядок у журнал подій: звідки, було → стало. 14.11.2026 годинник показував час на 12 год 24 хв назад, а в
// журналі подій не лишилось жодного сліду, коли й звідки він узявся.
void fx_time_set_local(time_t t, const String &src) {
  bool known = fx_time_known();
  time_t was = now();
  setTime(t);
  if (!known) return;                                // до першого встановлення (1970 р.) — не стрибок
  int32_t d = (int32_t)(t - was);
  if (d > 60 || d < -60)
    evlog_write(String("[TIME] ") + src + ": " + fx_time_text(was) + " -> " + fx_time_text(t) + " (" + (d > 0 ? "+" : "") +
                String((long)d) + " s)");
}

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
  String why = String("offset ") + String((long)g_fx_off_applied / 60) + String(" -> ") + String((long)off / 60) +
               String(" min (time zone / summer time)");
  dbg_ws_print(String("Time        : ") + why);
  g_fx_off_applied = off;
  fx_time_set_local(utc + off, why);                 // 1.1.6: у журналі подій — і було -> стало (раніше лише новий зсув)
}
