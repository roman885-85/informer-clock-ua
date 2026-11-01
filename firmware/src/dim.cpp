// Плавна яскравість табло (нове, не з оригіналу): проміжні значення між 16 рівнями MAX7219.
// EN: Smooth display dimming (new): in-between values for the MAX7219's 16 intensity levels, 1 kHz sigma-delta.
// Задача (власник, 28.10.2026, на 1.0.0): «зміна яскравості відбувається ступінчасто, а не плавно». У 1.0.0 табло йшло до
// цілі по одному рівню MAX7219 кожні 200 мс (bright.cpp), але рівнів лише 16 (шпаруватість (2L+1)/32), і внизу сусідні
// рівні відрізняються втричі (1/32 → 3/32): кожен крок видно сходинкою. Тепер окремий потік щомілісекунди веде
// яскравість до цілі рівномірно для ока — у логарифмі шпаруватості, весь діапазон за FX_DIM_FULL_MS — і показує
// проміжні значення, чергуючи два сусідні рівні (сигма-дельта, 1 кГц; частка — шістнадцятими, тож найповільніше
// чергування — раз на 16 мс, 62 Гц: оком не видно). У спокої — рівно ціль, без чергування й без записів у табло. Ціль, як і раніше, рахує bright.cpp (таблиця автора, гістерезис); інші режими
// яскравості (вручну, день/ніч, MQTT, будильник) потік не чіпає. Команду інтенсивності пише сам, під одним замком шини
// SPI з бібліотекою табло (lib/MD_MAX72XX, FORK.md): інакше його запис міг би вклинитись посеред кадру.
#include "fw.h"

static SemaphoreHandle_t s_spi;               // замок шини SPI табло (CS 5)
void md_spi_lock() { if (s_spi) xSemaphoreTake(s_spi, portMAX_DELAY); }
void md_spi_unlock() { if (s_spi) xSemaphoreGive(s_spi); }

static volatile bool s_on;                    // яскравість веде фоторезистор (bright.cpp)
static volatile uint8_t s_tgt;                // ціль, 0..15
static volatile int8_t s_jump = -1;           // поставити рівень одразу (старт, пробудження), -1 — ні
static volatile uint8_t s_lvl;                // найближчий цілий рівень зараз — для решти коду
static float s_q = -3.4657359f;               // яскравість зараз: ln шпаруватості, ln(1/32) .. ln(31/32)
static uint8_t s_acc;                         // накопичувач сигма-дельта (1/16)
static uint8_t s_out = 0xff;                  // що записано в MAX7219; 0xff — невідомо

static inline float q_of(uint8_t l) { return logf((2 * l + 1) / 32.0f); }

static void write_intensity(uint8_t l) {      // усім модулям ланцюжка, як MD_MAX72XX::control(INTENSITY)
  uint8_t n = g_c34bc->getDeviceCount();
  SPI.beginTransaction(SPISettings(8000000, MSBFIRST, SPI_MODE0));
  digitalWrite(5, LOW);
  for (uint8_t i = 0; i < n; i++) { SPI.transfer(0x0a); SPI.transfer(l); }
  digitalWrite(5, HIGH);
  SPI.endTransaction();
}

// один крок потоку (1 мс): повертає рівень на табло (0..15) або 0xff, коли потік не керує
uint8_t fx_dim_step_ms() {
  if (!s_on) { s_out = 0xff; return 0xff; }
  int8_t j = s_jump;
  if (j >= 0) { s_jump = -1; s_q = q_of(j); s_acc = 0; s_out = 0xff; }
  uint8_t tgt = s_tgt, l;
  float qt = q_of(tgt);
  if (s_q < qt) s_q = s_q + FX_DIM_DQ < qt ? s_q + FX_DIM_DQ : qt;
  else if (s_q > qt) s_q = s_q - FX_DIM_DQ > qt ? s_q - FX_DIM_DQ : qt;
  if (s_q == qt) { l = tgt; s_acc = 0; s_lvl = tgt; }
  else {
    float p = (expf(s_q) * 32.0f - 1.0f) * 0.5f;      // рівень з дробом
    if (p < 0) p = 0;
    if (p > 15) p = 15;
    uint8_t lo = (uint8_t)p;
    uint8_t f = (uint8_t)((p - lo) * 16.0f + 0.5f);  // частка сусіднього рівня, 1/16 (дрібніша — мерехтіла б)
    l = lo;
    if (lo < 15 && f >= 16) l = lo + 1;
    else if (lo < 15 && f > 0) {
      s_acc += f;
      if (s_acc >= 16) { s_acc -= 16; l = lo + 1; }
    }
    s_lvl = (uint8_t)(p + 0.5f);
  }
  if (l != s_out) {
    md_spi_lock();
    if (s_on) { write_intensity(l); s_out = l; }     // перевірка під замком: керування могли щойно забрати
    md_spi_unlock();
  }
  return l;
}

static void dim_task(void *) {
  TickType_t t = xTaskGetTickCount();
  for (;;) {
    vTaskDelayUntil(&t, 1);                   // 1 тік = 1 мс; пропущені кроки надолужуються
    fx_dim_step_ms();
  }
}

void fx_dim_begin() {
  if (s_spi) return;
  s_spi = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(dim_task, "fx_dim", 4096, nullptr, 2, nullptr, 1);   // стек із запасом: переповнення = перезапуск по колу
}
void fx_dim_follow(bool on, uint8_t tgt) { s_tgt = tgt > 15 ? 15 : tgt; s_on = on; }
void fx_dim_jump(uint8_t l) { s_jump = l > 15 ? 15 : l; }
uint8_t fx_dim_level() { return s_lvl; }
