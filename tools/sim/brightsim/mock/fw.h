// Заглушка fw.h для перевірки bright.cpp + photo.cpp на комп'ютері (tools/sim/brightsim)
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
inline void dbg_ws_print(const String &) {}
extern uint8_t g_c2ee9, g_c2f25, g_bf041, g_c34b2, g_c2ee3, g_c34b3, g_c3080;
extern uint16_t g_c3284;
struct IPAddress { IPAddress() {} };
#include "fix.h"
