// Сторож головного циклу (нове, не з оригіналу).
// EN: Main-loop watchdog (new): restarts the clock if loop() is stuck for more than 3 minutes.
// Ядро 1.0.4 стежить лише за задачею простою ядра 0, тож loop(), що завис у мережевому очікуванні чи в
// нескінченному циклі, сторож не помічає: годинник «живий», але табло й мережа стоять до ручного перезапуску
// (так оригінал і помирав уночі). Таймер щосекунди дивиться, коли loop() востаннє дійшов до кінця; якщо понад
// LOOP_GUARD_MS — позначка в RTC-пам'яті (її й етап циклу setup() запише в журнал подій) і перезапуск плати.
#include "fw.h"
#include "esp_timer.h"

RTC_NOINIT_ATTR uint32_t rtc_guard;
uint32_t g_fx_gate;
volatile uint32_t g_fx_alive_ms;        // коли loop() востаннє дійшов до кінця (і для запису аварії)
#define s_alive g_fx_alive_ms
static esp_timer_handle_t s_tmr;

void loop_guard_feed() { s_alive = millis(); }

static void guard_cb(void *) {
  if (millis() - s_alive <= LOOP_GUARD_MS) return;
  rtc_guard = LOOP_GUARD_MAGIC;
  esp_restart();
}

void loop_guard_start() {
  s_alive = millis();
  if (s_tmr != nullptr) return;
  esp_timer_create_args_t a = {};
  a.callback = guard_cb;
  a.name = "loopguard";
  if (esp_timer_create(&a, &s_tmr) == ESP_OK) esp_timer_start_periodic(s_tmr, 1000000);
}
