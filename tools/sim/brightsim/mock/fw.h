// Заглушка fw.h для перевірки bright.cpp + photo.cpp + dim.cpp на комп'ютері (tools/sim/brightsim)
#pragma once
#include <cstdint>
#include <cstdio>
#include <string>
#include <cmath>
typedef unsigned int uint;
struct String {
  std::string s;
  String() {}
  String(const char *c) : s(c) {}
  String(const std::string &x) : s(x) {}
  String(int v) : s(std::to_string(v)) {}
  String(unsigned v, int base = 10) : s(std::to_string(v)) { (void)base; }
  String(uint8_t v, int base) : s(std::to_string((unsigned)v)) { (void)base; }
  String(uint16_t v) : s(std::to_string((unsigned)v)) {}
  String &operator+=(const String &o) { s += o.s; return *this; }
  String operator+(const String &o) const { return String(s + o.s); }
};
extern uint32_t g_now;
inline uint32_t millis() { return g_now; }
extern int g_adc;
inline int analogRead(int) { return g_adc; }
struct MockP { int intensity = -1; int sets = 0; void setIntensity(uint8_t v) { intensity = v; sets++; } };
extern MockP P;
// dim.cpp: FreeRTOS, SPI (ловимо команди інтенсивності 0x0A між CS LOW/HIGH) і табло — лише потрібне
#include <cstring>
#include <ctime>
typedef void *SemaphoreHandle_t;
typedef uint32_t TickType_t;
#define portMAX_DELAY 0xffffffffu
inline SemaphoreHandle_t xSemaphoreCreateMutex() { static int m; return &m; }
inline int xSemaphoreTake(SemaphoreHandle_t, uint32_t) { return 1; }
inline int xSemaphoreGive(SemaphoreHandle_t) { return 1; }
inline TickType_t xTaskGetTickCount() { return 0; }
inline void vTaskDelayUntil(TickType_t *, TickType_t) {}
inline int xTaskCreatePinnedToCore(void (*)(void *), const char *, int, void *, int, void *, int) { return 1; }
#define LOW 0
#define HIGH 1
#define MSBFIRST 1
#define SPI_MODE0 0
struct SPISettings { SPISettings(uint32_t, int, int) {} };
struct MockSPI { bool cs = false; int n = 0; uint8_t last[64]; int writes = 0; int level = -1;
  void beginTransaction(SPISettings) {} void endTransaction() {} uint8_t transfer(uint8_t b) { if (n < 64) last[n++] = b; return 0; } };
extern MockSPI SPI;
inline void digitalWrite(int pin, int v) {
  if (pin != 5) return;
  if (v == LOW) { SPI.cs = true; SPI.n = 0; }
  else if (SPI.cs) { SPI.cs = false; if (SPI.n >= 2 && SPI.last[0] == 0x0a) { SPI.level = SPI.last[1]; SPI.writes++; } }
}
struct MockMX { uint8_t getDeviceCount() { return 16; } };
extern MockMX *g_c34bc;
inline void dbg_ws_print(const String &) {}
extern uint8_t g_c2ee9, g_c2f25, g_bf041, g_c34b2, g_c2ee3, g_c34b3, g_c3080;
extern uint16_t g_c3284;
struct IPAddress { IPAddress() {} };
#include "fix.h"
