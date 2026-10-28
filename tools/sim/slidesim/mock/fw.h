// Заглушка fw.h: табло MD_MAX72XX у пам'яті (16 модулів × 8 стовпців), MD_Parola — лише потрібне, керований годинник
#pragma once
#include "Arduino.h"
#include "fonts.h"
extern uint32_t g_ms;                       // мс симуляції
extern long g_t;                            // секунди доби
inline uint32_t millis() { return g_ms; }
inline int hour() { return (int)((g_t / 3600) % 24); }
inline int minute() { return (int)((g_t / 60) % 60); }
inline int second() { return (int)(g_t % 60); }
struct MD_MAX72XX {
  typedef uint8_t fontType_t;
  enum controlRequest_t { SHUTDOWN, SCANLIMIT, INTENSITY, TEST, DECODE, UPDATE, WRAPAROUND };
  enum { OFF = 0, ON = 1 };
  uint8_t fb[16 * 8] = {0};
  const uint8_t *font = nullptr;
  void setColumn(int16_t c, uint8_t v) { if (c >= 0 && c < 128) fb[c] = v; }
  void clear(uint8_t a, uint8_t b) { for (int d = a; d <= b && d < 16; d++) memset(fb + d * 8, 0, 8); }
  void clear() { memset(fb, 0, sizeof fb); }
  void control(controlRequest_t, int) {}
  void setFont(fontType_t *f) { font = f; }
  uint8_t getChar(uint16_t c, uint8_t size, uint8_t *buf) {        // формат MD_MAX72XX: 'F' 1 перший останній висота | 256 записів
    const uint8_t *p = font; int first = 0, last = 255;
    if (p[0] == 'F') { first = p[2]; last = p[3]; p += 5; }
    if (c < first || c > last) return 0;
    for (int k = first; k < (int)c; k++) p += 1 + p[0];
    uint8_t w = p[0];
    for (int i = 0; i < w && i < size; i++) buf[i] = p[1 + i];
    return w;
  }
};
extern MD_MAX72XX MX;
enum textEffect_t { PA_NO_EFFECT = 0, PA_PRINT = 1 };
extern int g_zs[4], g_ze[4];   // зони за розкладкою (sim.cpp)
struct MockP { bool getZoneStatus(int) { return true; } int getZoneCount() { return 4; } void setTextEffect(int, textEffect_t, textEffect_t) {}
  bool displayAnimate() { return true; } void getZone(uint8_t z, uint8_t &s, uint8_t &e) { s = g_zs[z]; e = g_ze[z]; } };
extern MockP P;
inline void config_set(const char *, const char *) {}
inline void config_save_atomic() {}
void colon_timing(int8_t ph);
#include "globals.inc"
