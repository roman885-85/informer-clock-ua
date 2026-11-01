// Заглушка fw.h для перевірки dim.cpp на комп'ютері (tools/sim/dimsim): FreeRTOS, SPI і табло — лише те, що потрібно.
#pragma once
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include <string>
struct String { std::string s; String() {} String(const char *c) : s(c) {} };
struct IPAddress { IPAddress() {} };
typedef void *SemaphoreHandle_t;
typedef uint32_t TickType_t;
#define portMAX_DELAY 0xffffffffu
inline SemaphoreHandle_t xSemaphoreCreateMutex() { static int m; return &m; }
extern int g_locks;                                    // скільки разів брали замок (для перевірки)
inline int xSemaphoreTake(SemaphoreHandle_t, uint32_t) { g_locks++; return 1; }
inline int xSemaphoreGive(SemaphoreHandle_t) { return 1; }
inline TickType_t xTaskGetTickCount() { return 0; }
inline void vTaskDelayUntil(TickType_t *, TickType_t) {}
inline int xTaskCreatePinnedToCore(void (*)(void *), const char *, int, void *, int, void *, int) { return 1; }
#define LOW 0
#define HIGH 1
#define MSBFIRST 1
#define SPI_MODE0 0
struct SPISettings { SPISettings(uint32_t, int, int) {} };
// MAX7219 у пам'яті: ловимо команди інтенсивності (0x0A) між CS LOW і CS HIGH
struct MockSPI {
  bool cs = false; int n = 0; uint8_t last[64]; int writes = 0; int level = -1;
  void beginTransaction(SPISettings) {}
  void endTransaction() {}
  uint8_t transfer(uint8_t b) { if (n < 64) last[n++] = b; return 0; }
};
extern MockSPI SPI;
inline void digitalWrite(int pin, int v) {
  if (pin != 5) return;
  if (v == LOW) { SPI.cs = true; SPI.n = 0; }
  else if (SPI.cs) {
    SPI.cs = false;
    bool all = SPI.n >= 2;                              // усі пари — (0x0A, рівень) і рівень однаковий
    for (int i = 0; i + 1 < SPI.n; i += 2) all = all && SPI.last[i] == 0x0a && SPI.last[i + 1] == SPI.last[1];
    if (all) { SPI.level = SPI.last[1]; SPI.writes++; }
  }
}
struct MockMX { uint8_t getDeviceCount() { return 16; } };
extern MockMX *g_c34bc;
#include "fix.h"
