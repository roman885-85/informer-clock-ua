// Перевірка сетевого журналу: справжній wdbg.cpp, порт 2323 замість 23 (без root), 40 с роботи
#include "fw.h"
String g_c33a4, g_c3398, g_fx_crash_last;
void fx_wdbg_set(bool on); void fx_wdbg_tick(); void fx_wdbg_line(const char *s, size_t n);
extern "C" int __wrap_log_printf(const char *fmt, ...);
int main() {
  fx_wdbg_line("до ввімкнення — не збирається", 30);
  fx_wdbg_set(true);
  for (int i = 0; i < 300; i++) { char b[64]; fx_wdbg_line(b, snprintf(b, sizeof b, "рядок історії %03d", i)); }
  __wrap_log_printf("[E][WiFiClient.cpp:%d] connect(): помилка ядра %d\n", 258, 104);
  uint32_t t0 = millis(), k = 0;
  while (millis() - t0 < 40000) {
    fx_wdbg_tick();
    if (millis() - t0 > k * 1000) { char b[64]; fx_wdbg_line(b, snprintf(b, sizeof b, "живий рядок %u", k)); k++; }
    usleep(2000);
  }
  fx_wdbg_set(false);
  printf("кінець\n");
}
