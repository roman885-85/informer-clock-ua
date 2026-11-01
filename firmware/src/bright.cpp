// Плавна яскравість за фоторезистором (нове, не з оригіналу).
// EN: Smooth photoresistor brightness (new): filtered sensor, author's level table with hysteresis; the fade itself — dim.cpp.
// Задача: яскравість за датчиком світла має змінюватись плавно, а не стрибками, і не чекати кінця прокрутки новин.
// У автора: один замір АЦП раз на 7 с і лише після кінця прокрутки (dbg_ram_print під P.getZoneStatus(0)), рівень
// табло за таблицею — одразу (стрибок на кілька рівнів), на межі рівнів — перемикання туди-сюди; стрічка — так само
// стрибком. Тепер: замір щоразу через FX_ADC_MS зі згладжуванням, рівень — за тією ж таблицею автора (поріг prg), нова
// ціль — з гістерезисом (різниця ≥ 2 рівні — одразу, в 1 рівень — якщо тримається FX_HOLD_MS), стрічка — дрібними
// кроками (fx_led_ramp). Не залежить від анімації. Табло до цілі веде потік dim.cpp — з проміжними значеннями між
// рівнями MAX7219 (у 1.0.0 тут був крок у рівень кожні 200 мс — власник: «ступінчасто, а не плавно»).
#include "fw.h"

void photo_brightness_read();
static float s_adc = -1;
static uint32_t s_t_adc, s_t_map, s_t_diff;
static uint8_t s_lvl = 0xff;    // рівень, що зараз на табло (0..15); 0xff — невідомо
static uint8_t s_tgt = 0xff;    // прийнята ціль
static int s_off;               // зсув значення для перевірки запасу біля межі рівнів

uint16_t fx_photo_adc() {
  float v = (s_adc < 0 ? analogRead(0x22) : s_adc) + s_off;
  return v < 0 ? 0 : v > 4095 ? 4095 : (uint16_t)(v + 0.5f);
}
// рівень за таблицею автора для значення АЦП + off (у «щілинах» таблиці автор лишає попередній — тут це ціль)
static uint8_t level_at(int off) {
  s_off = off;
  if (s_tgt <= 15) g_bf041 = s_tgt;
  photo_brightness_read();
  s_off = 0;
  return g_bf041;
}

void fx_bright_tick() {
  bool photo = g_c2ee9 == 1, led = g_c2f25 == 1;
  if (!photo && !led) { s_adc = -1; s_lvl = 0xff; s_tgt = 0xff; return; }
  uint32_t now = millis();
  if (now - s_t_adc >= FX_ADC_MS) {
    s_t_adc = now;
    float v = analogRead(0x22);
    s_adc = s_adc < 0 ? v : s_adc + (v - s_adc) * FX_ADC_K;
  }
  if (now - s_t_map >= FX_MAP_MS) {
    s_t_map = now;
    uint8_t lo = level_at(-FX_ADC_MARGIN), hi = level_at(FX_ADC_MARGIN);
    uint8_t nb = level_at(0);       // останнім — щоб g_c2ee3 (стрічка) й g_c3284 були за справжнім значенням
    if (s_tgt <= 15 && s_tgt >= (lo < hi ? lo : hi) && s_tgt <= (lo < hi ? hi : lo)) nb = s_tgt;   // біля межі — лишаємо
    if (s_tgt > 15) s_tgt = nb;
    else if (nb != s_tgt) {
      if (s_t_diff == 0) s_t_diff = now;
      if ((nb > s_tgt ? nb - s_tgt : s_tgt - nb) >= 2 || now - s_t_diff >= FX_HOLD_MS) { s_tgt = nb; s_t_diff = 0; }
    } else s_t_diff = 0;
    g_bf041 = s_tgt;
  }
  if (!photo || g_c3080 != 0 || s_tgt > 15) { fx_dim_follow(false, 0); return; }   // яскравістю керує інше
  if (g_c34b3 > 15 || s_lvl > 15) {  // невідомо, що на табло (старт, будильник, повідомлення, примусове оновлення автора)
    if (s_lvl > 15) s_lvl = s_tgt;
    fx_dim_jump(s_lvl);              // записати одразу, далі — плавно
  }
  fx_dim_follow(true, s_tgt);
  s_lvl = fx_dim_level();
  g_c34b3 = s_lvl;
}

void fx_bright_actual(uint8_t lvl) {
  if (s_lvl > 15) return;
  s_lvl = lvl;
  g_c34b3 = lvl;
  fx_dim_jump(lvl);                  // автор щойно записав lvl (пробудження) — звідси плавно до цілі
}

// рядок налагодження в тому ж вигляді, що в автора (раз на 7 с з dbg_ram_print)
void fx_photo_print() {
  if (g_c2ee9 != 1 && g_c2f25 != 1) return;
  dbg_ws_print(String("Display INFO: Photoresistor data: ") + String(g_c3284) + String(" , Treshold UI selected:  ") +
               String(g_c34b2, 10) + String(" , Treshold:  ") + String((int)g_c34b2 * 0x44) + String(" , brightness=") +
               String(g_bf041, 10));
}

// стрічка: до цілі дрібними кроками (≈1–1,5 с на весь діапазон)
uint8_t fx_led_ramp(uint8_t cur, uint8_t tgt) {
  static uint32_t t;
  if (millis() - t < 15) return cur;
  t = millis();
  int d = (int)tgt - (int)cur;
  int st = d / 10;
  if (st == 0) st = d > 0 ? 1 : -1;
  if (st > 8) st = 8;
  if (st < -8) st = -8;
  return (uint8_t)(cur + st);
}
