// Перевірка fx_dst_offset (timefix.cpp) на комп'ютері з TimeLib: дати переходів ЄС 2024–2031
#include <cstdio>
#include <cstdint>
#include "TimeLib.h"
uint8_t g_c32e4; float g_c3430;
int32_t fx_dst_offset(time_t utc);
int main() {
  g_c32e4 = 2;
  const char *names[] = {"березень", "жовтень"};
  for (int y = 2024; y <= 2031; y++) {
    tmElements_t e = {}; e.Year = y - 1970; e.Month = 1; e.Day = 1;
    time_t t = makeTime(e), prev = fx_dst_offset(t);
    int k = 0;
    for (; year(t) == y; t += 600) {
      int32_t d = fx_dst_offset(t);
      if (d != prev) { printf("%d %-8s: %02d.%02d %02d:%02d UTC (%s), +%d хв\n", y, names[k++ & 1], day(t), month(t), hour(t), minute(t), weekday(t) == 1 ? "неділя" : "НЕ неділя!", (int)d / 60); prev = d; }
    }
  }
  g_c32e4 = 1; printf("sumT=1: %d; ", (int)fx_dst_offset(0)); g_c32e4 = 0; printf("sumT=0: %d\n", (int)fx_dst_offset(1800000000));
}
