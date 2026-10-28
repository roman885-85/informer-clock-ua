// Симуляція перелистування цифр великого циферблата: справжні bigslide.cpp, MD_Parola, MD_MAX72XX і шрифти автора.
// Друкує кадри (два поверхи, 32×16), перевіряє: (1) перед початком розрахунок збігається з тим, що надрукувала
// MD_Parola; (2) останній кадр перелистування — рівно те, що MD_Parola надрукує для нового часу з тією ж двокрапкою;
// (3) нерухомі символи (цифри без змін і двокрапка) не мінялись жодного кроку; (4) кожен крок — зсув на рядок.
// Використання: sim [pgm] — pgm: ще й кадри у файли PGM (для картинки в звіті).
#include "fw.h"
#include "fonts.h"
#include "SPI.h"
uint32_t g_ms = 1000; SerialMock Serial; SPIClass SPI;
MD_Parola P(MD_MAX72XX::FC16_HW, 5, 16);
MD_MAX72XX *g_c34bc = P.getGraphicObject();
uint8_t g_c34ad = 1, g_c34ba = 8;
void md_spi_lock() {}
void md_spi_unlock() {}
static const uint8_t *F0[3] = { font_font_3f43daa6, font_font_3f43d964, font_font_3f43d822 };   // нижні половини
static const uint8_t *F1[3] = { font_font_3f43db47, font_font_3f43da05, font_font_3f43d8c3 };   // верхні половини
static uint16_t frame(int i) { return g_c34bc->getColumn(32 + i) | ((uint16_t)g_c34bc->getColumn(i) << 8); }
static void print_parola(const char *s) {         // як clock_tick_2zones: PA_PRINT, швидкість 0, обидві зони
  static char buf[2][16];
  strcpy(buf[0], s); strcpy(buf[1], s);
  P.displayZoneText(0, buf[0], PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(1, buf[1], PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  for (int k = 0; k < 10 && !(P.getZoneStatus(0) && P.getZoneStatus(1)); k++) { g_ms += 1; P.displayAnimate(); }
}
static void show(const char *t, FILE *pgm) {
  printf("%s\n", t);
  for (int y = 0; y < 16; y++) {
    printf("   ");
    for (int i = 31; i >= 0; i--) putchar((frame(i) >> y) & 1 ? '#' : '.');
    printf("%s\n", y == 7 ? "   ← межа поверхів" : "");
  }
  if (pgm) for (int y = 0; y < 16; y++) for (int i = 31; i >= 0; i--) fputc((frame(i) >> y) & 1 ? 255 : 0, pgm);
}
static int run(int font, const char *prev, const char *next, bool pics) {
  P.begin(2); P.setZone(0, 0, 3); P.setZone(1, 4, 7);           // розкладка власника: 2×4, зона 0 — модулі 0–3 (низ)
  P.setFont(0, (MD_MAX72XX::fontType_t *)F0[font]); P.setFont(1, (MD_MAX72XX::fontType_t *)F1[font]);
  P.setCharSpacing(1);
  print_parola(prev);
  uint16_t before[32]; for (int i = 0; i < 32; i++) before[i] = frame(i);
  // що надрукує MD_Parola для нового часу з двокрапкою prev — еталон кінця
  char tgt[16]; { const char *s = prev; size_t o = 0;
    for (const char *p = next; *p; p++) { if (*p >= '0' && *p <= '9') tgt[o++] = *p; else { while (*s >= '0' && *s <= '9') s++; tgt[o++] = *s ? *s++ : *p; } }
    tgt[o] = 0; }
  print_parola(tgt);
  uint16_t want[32]; for (int i = 0; i < 32; i++) want[i] = frame(i);
  print_parola(prev);
  printf("== шрифт %d: «%s» → «%s»\n", font, prev, next);
  if (!fx_bigslide_begin(prev, next)) { printf("   перелистування не почалось\n"); return 1; }
  FILE *pgm = nullptr; char fn[64];
  if (pics) { snprintf(fn, sizeof fn, "slide-f%d-%c%c%c%c.pgm", font, next[0], next[1], next[3], next[4]); pgm = fopen(fn, "wb"); fprintf(pgm, "P5 32 %d 255\n", 16 * 17); show("крок 0", pgm); }
  // стовпці символів, що не змінились (за шириною символів шрифту, як друкує Parola: зліва направо від старшого стовпця)
  bool fixed[32] = {false};
  { uint8_t cb[12]; g_c34bc->setFont((MD_MAX72XX::fontType_t *)F1[font]);
    int w = 0, n = strlen(prev); for (int k = 0; k < n; k++) { int cw = g_c34bc->getChar((uint8_t)prev[k], 12, cb); w += cw + (cw && k + 1 < n ? 1 : 0); }
    int pos = (32 - w) / 2 + w - 1;
    for (int k = 0; k < n; k++) { int cw = g_c34bc->getChar((uint8_t)prev[k], 12, cb);
      for (int c = 0; c < cw; c++, pos--) if (pos >= 0 && prev[k] == tgt[k]) fixed[pos] = true;
      if (cw && k + 1 < n) pos--; } }
  int steps = 0, bad_static = 0, bad_shift = 0;
  uint16_t prevf[32]; for (int i = 0; i < 32; i++) prevf[i] = frame(i);
  while (true) {
    g_ms += 7;
    bool on = fx_bigslide_tick();
    bool changed = false;
    for (int i = 0; i < 32; i++) if (frame(i) != prevf[i]) changed = true;
    if (changed) {
      steps++;
      for (int i = 0; i < 32; i++) {
        uint16_t f = frame(i);
        if (fixed[i] && f != before[i]) bad_static++;                       // символ без змін зрушив
        if (f != prevf[i] && (uint16_t)(prevf[i] << 1) != (uint16_t)(f & 0xfffe)) bad_shift++;   // не зсув на рядок
        prevf[i] = f;
      }
      if (pics) { char t[16]; snprintf(t, sizeof t, "крок %d", steps); show(t, pgm); }
    }
    if (!on && !changed) break;
  }
  if (pgm) fclose(pgm);
  int bad_end = 0; for (int i = 0; i < 32; i++) bad_end += frame(i) != want[i];
  printf("   кроків %d, кінець = друк Parola: %s, символи без змін стояли: %s, кожен крок — зсув на рядок: %s\n", steps,
         bad_end ? "НІ" : "так", bad_static ? "НІ" : "так", bad_shift ? "НІ" : "так");
  return (steps == 16 && !bad_end && !bad_static && !bad_shift) ? 0 : 1;
}
int main(int argc, char **argv) {
  bool pics = argc > 1 && !strcmp(argv[1], "pgm");
  int bad = 0;
  const char *cases[][2] = { {"12:59", "13:00"}, {"09:59", "10:00"}, {"23:59", "00:00"}, {"12:34", "12:35"},
                             {"12\x0a" "34", "12:35"}, {"07:19", "07\x0b" "20"} };
  for (int f = 0; f < 3; f++) for (auto &c : cases) bad += run(f, c[0], c[1], pics && f == 0 && c == cases[0]);
  // 1) та сама хвилина — лише двокрапка: без перелистування
  P.setFont(0, (MD_MAX72XX::fontType_t *)F0[0]); P.setFont(1, (MD_MAX72XX::fontType_t *)F1[0]); print_parola("12:34");
  bool b1 = fx_bigslide_begin("12:34", "12 34");
  printf("== лише двокрапка: перелистування %s\n", b1 ? "ПОЧАЛОСЬ (помилка)" : "немає — так і має бути");
  // 2) на табло не те, що думаємо (інша розкладка) — без перелистування
  g_c34bc->setColumn(5, 0xff);
  bool b2 = fx_bigslide_begin("12:34", "12:35");
  printf("== на табло не той кадр: перелистування %s\n", b2 ? "ПОЧАЛОСЬ (помилка)" : "немає — так і має бути");
  // 3) An=0 — як у автора
  print_parola("12:34"); g_c34ad = 0;
  bool b3 = fx_bigslide_begin("12:34", "12:35");
  printf("== An=0: перелистування %s\n", b3 ? "ПОЧАЛОСЬ (помилка)" : "немає — так і має бути");
  bad += b1 + b2 + b3;
  printf(bad ? "РЕЗУЛЬТАТ: Є ПОМИЛКИ (%d)\n" : "РЕЗУЛЬТАТ: усе гаразд\n", bad);
  return bad ? 1 : 0;
}
