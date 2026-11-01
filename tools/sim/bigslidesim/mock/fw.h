// Заглушка fw.h для перевірки bigslide.cpp зі справжніми MD_Parola і MD_MAX72XX (tools/sim/bigslidesim)
#pragma once
#include "Arduino.h"
#include "MD_Parola.h"
#include <string>
struct String { std::string s; String() {} String(const char *c) : s(c) {} };
inline void dbg_ws_print(const String &s) { printf("  [dbg] %s\n", s.s.c_str()); }
extern MD_Parola P;
extern MD_MAX72XX *g_c34bc;
extern uint8_t g_c34ad, g_c34ba;                   // An («зі зсувом»), модулів у рядку
bool fx_bigslide_begin(const char *prev, const char *next);
bool fx_bigslide_tick();
void fx_bigslide_reset();
