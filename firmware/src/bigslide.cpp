// Великий циферблат «зі зсувом» (нове, не з оригіналу): цифри, що змінились, перелистуються на всю висоту двох поверхів.
// EN: Big two-floor clock face with sliding digits (new): when An=1 ("sliding digits", the small clock's setting), digits
// that change roll over the full 16-row height of both floors — the old digit moves down, the new one comes in from the top.
// Задача (власник, 28.10.2026): «зробити зміну цифр у великому циферблаті з ефектом перелистування, як у малому».
// У автора великий циферблат новий час просто друкує (PA_PRINT) у двох зонах: верхні половини цифр — зона 1 (верхній
// поверх), нижні — зона 0, кожна своїм шрифтом. Тут під час зміни цифр стовпці обох зон складаються в 16-рядкові
// (біт 0 — верхній рядок верхнього поверху, біт 15 — нижній рядок нижнього) і зсуваються по рядку за крок, як у малому
// циферблаті (clock_slide.cpp: старе — вниз, нове — згори). Двокрапка не рухається: у кадрі-цілі лишається та, що
// на табло, а нову ставить звичайний друк наступного оновлення. Кадри рахуються тими самими шрифтами й так само, як
// друкує MD_Parola (PA_CENTER, проміжок між символами), і перед початком звіряються з табло: не збіглося хоч у
// стовпці (інша розкладка, інвертовані зони) — без анімації, як у автора.
#include "fw.h"

#define BS_COLS 32                 // зона великого циферблата — 4 модулі
#define BS_STEP_MS 28              // рядок за крок: 16 кроків ≈ 0,45 с (малий циферблат: 8 кроків по 45–60 мс)

static bool s_run;
static uint8_t s_k;                // зроблено кроків, 0..16
static uint32_t s_t, s_seen;       // час кроку; останній виклик з clock_tick_2zones
static uint16_t s_old[BS_COLS], s_new[BS_COLS];
static uint32_t s_mov;             // стовпці, що рухаються (біт на стовпець)
static uint16_t s_c0, s_c1;        // перший стовпець зони 0 (нижній поверх) і зони 1 (верхній) у MD_MAX72XX
static bool s_warned;

// стовпці тексту зони так, як їх друкує MD_Parola (calcTextLimits PA_CENTER + commonPrint): out[i] — стовпець
// «початок зони + i»; false — зона не з 4 модулів або текст не вміщається
static bool render(uint8_t z, const char *txt, uint8_t *out) {
  uint8_t ms, me;
  P.getZone(z, ms, me);
  if (me < ms || me - ms + 1 != BS_COLS / 8) return false;
  MD_MAX72XX *mx = g_c34bc;
  mx->setFont(P.getFont(z));
  uint8_t sp = P.getCharSpacing(z), cb[12];
  uint16_t w = 0;
  for (const char *p = txt; *p; p++) {
    uint8_t cw = mx->getChar((uint8_t)*p, sizeof cb, cb);
    w += cw;
    if (cw != 0 && p[1]) w += sp;     // проміжок лише після символу з шириною (getTextWidth)
  }
  if (w > BS_COLS) return false;
  memset(out, 0, BS_COLS);
  int16_t pos = (BS_COLS - w) / 2 + w - 1;    // _limitLeft відносно початку зони; друк — до _limitRight
  for (const char *p = txt; *p; p++) {
    uint8_t cw = mx->getChar((uint8_t)*p, sizeof cb, cb);
    for (uint8_t i = 0; i < cw && pos >= 0; i++) out[pos--] = cb[i];
    if (cw != 0 && p[1]) pos -= sp;
  }
  return true;
}

// ціль: цифри — з next, роздільники (двокрапка й фази її анімації) — з prev; false — цифри ті самі
static bool make_target(const char *prev, const char *next, char *tgt, size_t cap) {
  const char *a = prev, *b = next;
  bool same = true;
  while (true) {
    while (*a && (*a < '0' || *a > '9')) a++;
    while (*b && (*b < '0' || *b > '9')) b++;
    if (*a != *b) { same = false; break; }
    if (!*a) break;
    a++; b++;
  }
  if (same) return false;
  const char *s = prev;
  size_t o = 0;
  for (const char *p = next; *p && o + 1 < cap; p++) {
    if (*p >= '0' && *p <= '9') { tgt[o++] = *p; continue; }
    while (*s >= '0' && *s <= '9') s++;
    tgt[o++] = *s ? *s++ : *p;
  }
  tgt[o] = 0;
  return true;
}

static void draw(uint8_t k) {
  MD_MAX72XX *mx = g_c34bc;
  mx->control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
  for (uint8_t i = 0; i < BS_COLS; i++) {
    if (!((s_mov >> i) & 1)) continue;
    uint16_t v = k >= 16 ? s_new[i] : (uint16_t)((s_old[i] << k) | (s_new[i] >> (16 - k)));
    mx->setColumn(s_c1 + i, v & 0xff);       // верхній поверх
    mx->setColumn(s_c0 + i, v >> 8);         // нижній поверх
  }
  mx->control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void fx_bigslide_reset() { s_run = false; }

// з clock_tick_2zones при оновленні часу: prev — що на табло, next — новий рядок; true — почалось перелистування
// (друк Parola не потрібен), false — друкувати як завжди
bool fx_bigslide_begin(const char *prev, const char *next) {
  s_run = false;
  if (g_c34ad != 1 || g_c34ba != 8) return false;           // An=1 («зі зсувом») і два поверхи
  char tgt[16];
  if (!make_target(prev, next, tgt, sizeof tgt)) return false;
  uint8_t ob[BS_COLS], ot[BS_COLS], nb[BS_COLS], nt[BS_COLS];
  if (!render(0, prev, ob) || !render(1, prev, ot) || !render(0, tgt, nb) || !render(1, tgt, nt)) return false;
  uint8_t ms, me;
  P.getZone(0, ms, me); s_c0 = ms * 8;
  P.getZone(1, ms, me); s_c1 = ms * 8;
  MD_MAX72XX *mx = g_c34bc;
  for (uint8_t i = 0; i < BS_COLS; i++) {
    if (mx->getColumn(s_c0 + i) != ob[i] || mx->getColumn(s_c1 + i) != ot[i]) {
      if (!s_warned) { s_warned = true; dbg_ws_print(String("Big clock slide: display differs from the computed frame — plain print")); }
      return false;
    }
    s_old[i] = ot[i] | ((uint16_t)ob[i] << 8);
    s_new[i] = nt[i] | ((uint16_t)nb[i] << 8);
  }
  // рухаються цілі символи: стовпці між порожніми в обох кадрах, де хоч щось змінилось
  s_mov = 0;
  for (uint8_t i = 0; i < BS_COLS;) {
    if ((s_old[i] | s_new[i]) == 0) { i++; continue; }
    uint8_t j = i;
    bool ch = false;
    while (j < BS_COLS && (s_old[j] | s_new[j]) != 0) { if (s_old[j] != s_new[j]) ch = true; j++; }
    if (ch) for (uint8_t k = i; k < j; k++) s_mov |= 1UL << k;
    i = j;
  }
  if (s_mov == 0) return false;
  s_k = 0;
  s_t = s_seen = millis();
  s_run = true;
  return true;
}

// на початку clock_tick_2zones: true — триває перелистування (решту кроку годинника пропустити)
bool fx_bigslide_tick() {
  if (!s_run) return false;
  uint32_t now = millis();
  // інший режим табло чи хтось перемалював зони (вихід циферблата, повідомлення) — кинути
  if (now - s_seen > 300 || !P.getZoneStatus(0) || !P.getZoneStatus(1)) { s_run = false; return false; }
  s_seen = now;
  if (now - s_t < BS_STEP_MS) return true;
  s_t = now;
  draw(++s_k);
  if (s_k >= 16) s_run = false;
  return true;
}
