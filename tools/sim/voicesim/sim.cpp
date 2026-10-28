// Симуляція голосу щогодини: справжні voice_hourly() і f_400e3500() з firmware/src/sound.cpp (вирізаються build.sh)
#include <cstdio>
#include <cstdint>
#include <string>
typedef unsigned int uint;
struct String { std::string s; String() {} String(const char *c) : s(c) {} String(uint32_t v) : s(std::to_string(v)) {}
  String &operator+=(const String &o) { s += o.s; return *this; } };
static uint32_t T;            // мс від старту
static long T0 = 14 * 3600 + 23 * 60;   // старт о 14:23:00
uint32_t millis() { return T; }
int hour() { return (int)(((T0 + T / 1000) / 3600) % 24); }
int minute() { return (int)(((T0 + T / 1000) / 60) % 60); }
void delay(uint32_t ms) { T += ms; }
long random(long a, long b) { return a + 5 % (b - a); }
bool fx_time_known() { return true; }
bool time_in_range(int32_t, int32_t) { return true; }   // день — звук дозволено
void dbg_ws_print(const String &s) { (void)s; }
struct DF { void playFolder(int f, int n) { printf("  %02d:%02d:%02d.%03u  грає папка %d, трек %d%s\n", hour(), minute(), (int)((T0 + T / 1000) % 60), T % 1000, f, n,
  f == 15 ? " (мелодія)" : n == 30 ? " («Зараз»)" : n == 50 ? " (тест гучності)" : " (година)"); }
  void volume(int) {} } myDFPlayer;
uint8_t g_c2f1a, g_c2f1c = 13, g_c19d8, g_c2f1d = 1, g_c2f1b = 4, g_c2f21, g_c2e70, g_c19c5, g_c19c6, g_c19c7, g_c19c8, g_c19d9, g_bec04;
uint32_t g_c19cc, g_c19d0, g_c19d4; int32_t g_c2e5c, g_c2e58;
bool g_fx_voice_init, g_fx_voice_test;
#include "voice_fns.inc"
int main() {
  printf("== старт о 14:23, далі до 16:00:30\n");
  for (; T < (uint32_t)(5850 * 1000); T += 5) voice_hourly();
  printf("== кнопка «Тест голоса» о %02d:%02d\n", hour(), minute());
  g_fx_voice_test = true; g_c2f21 = 99;
  for (uint32_t e = T + 20000; T < e; T += 5) voice_hourly();
}
