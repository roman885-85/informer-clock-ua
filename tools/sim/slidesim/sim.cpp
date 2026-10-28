// Симуляція годинника зі зсувом цифр (справжній clock_slide.cpp): перехід через межу години при nz (без нуля попереду).
// Використання: sim <nz 0|1> <сценарій 0 — 9:59→10:00, 1 — 23:59→0:00> [font_l] ; порівняння з «чистим» стартом.
#include "fw.h"
#include <cstdlib>
#include "defs.inc"
uint32_t g_ms; long g_t; MD_MAX72XX MX; MockP P; MD_MAX72XX *g_c34bc = &MX;
int g_zs[4], g_ze[4];
void clock_slide();
#include "colon.inc"
static int g_pan = 2;   // pAn — анімація двокрапки
static int L;   // розкладка: 0 — 2×4, 1 — 2×8 (c_position 0), 2 — 2×8 (c_position 1), 3..8 — 1 ряд з 4,5,6,8,10,12 модулів
static void settings(int nz, int font) {
  static const int one[6] = { 4, 5, 6, 8, 10, 12 };
  g_c349c = font; g_c2eed = g_pan; g_c34ab = nz; g_c2c00 = 3;
  if (L <= 2) { g_c34ba = 8; g_c34b8 = L == 0 ? 4 : 8; g_c34b7 = L == 2; g_c34b9 = 4; }
  else { g_c34ba = 4; g_c34b9 = one[L - 3]; g_c34b8 = 4; g_c34b7 = 0; }
  // зони — як у display_zones_init / content_cycle_8m автора
  if (L == 0) { g_zs[0] = 0; g_ze[0] = 3; g_zs[1] = 4; g_ze[1] = 7; }
  else if (L == 1) { g_zs[0] = 0; g_ze[0] = 7; g_zs[1] = 12; g_ze[1] = 15; }
  else if (L == 2) { g_zs[0] = 0; g_ze[0] = 7; g_zs[1] = 8; g_ze[1] = 11; }
  else { g_zs[0] = 0; g_ze[0] = g_c34b9 - 1; }
}
static void run(long from, long to) {            // g_t і g_ms крокують разом, крок 2 мс; clock_slide щокроку
  long ms0 = g_ms;
  for (long ms = 0; ; ms += 2) {
    g_ms = ms0 + ms; g_t = from + ms / 1000;
    if (g_t >= to) break;
    clock_slide();
  }
}
static void show(const char *title, const uint8_t *fb) {
  printf("%s\n", title);
  for (int y = 0; y < 8; y++) {
    printf("  ");
    for (int c = 63; c >= 32; c--) putchar((fb[c] >> y) & 1 ? '#' : '.');   // верхній ряд: модулі 4–7 (зона 1)
    printf("\n");
  }
}
int main(int argc, char **argv) {
  int nz = argc > 1 ? atoi(argv[1]) : 1, sc = argc > 2 ? atoi(argv[2]) : 0, font = argc > 3 ? atoi(argv[3]) : 1;
  bool clean = argc > 4 && argv[4][0] == 'c';     // «чистий» старт: з :00:00 тієї ж години — еталон
  bool quiet = argc > 5;
  L = argc > 6 ? atoi(argv[6]) : 0;
  g_pan = argc > 7 ? atoi(argv[7]) : 2;
  long h0 = sc == 0 ? 9 : 23;
  long T0 = h0 * 3600 + 59 * 60 + 50, T1 = (h0 + 1) * 3600 + 7;      // з :59:50 до :00:07 наступної години
  if (sc == 1) T1 = 24 * 3600 + 7;
  // 1) перехід
  settings(nz, font); g_ms = 1000;
  if (clean) {
    run(T1 - 7, T1);
    for (int c = 0; c < 128; c++) printf("%02x", MX.fb[c]);
    printf("\n");
    return 0;
  }
  run(T0, T0 + 9);
  uint8_t before[128]; memcpy(before, MX.fb, 128);
  run(T0 + 9, T1);
  uint8_t after[128]; memcpy(after, MX.fb, 128);
  // 2) «чистий» старт у той самий момент: нова пам'ять табло й нові змінні
  { uint8_t keep[128]; memcpy(keep, MX.fb, 128); }
  printf("nz=%d font_l=%d: %02ld:59:59 → %02ld:00:07\n", nz, font, h0, (h0 + 1) % 24);
  if (!quiet) { show("до (59:59):", before); show("після переходу (00:07):", after); }
  for (int c = 0; c < 128; c++) printf("%02x", after[c]);
  printf("\n");
  int diff = 0; for (int c = 0; c < 128; c++) diff += __builtin_popcount(after[c] ^ 0);
  printf("засвічено точок: %d\n", diff);
  return 0;
}
