// Заглушка fw.h для перевірки wdbg.cpp на комп'ютері (tools/sim/wdbgsim)
#pragma once
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <string>
#include <mutex>
#include <chrono>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
struct String { std::string s; String() {} String(const char *c) : s(c) {} String(const std::string &x) : s(x) {}
  String(int v) : s(std::to_string(v)) {} String(unsigned v) : s(std::to_string(v)) {} String(unsigned long v) : s(std::to_string(v)) {} String(long v) : s(std::to_string(v)) {}
  String &operator+=(const String &o) { s += o.s; return *this; }
  friend String operator+(const String &a, const String &b) { return String(a.s + b.s); }
  friend String operator+(const String &a, const char *b) { return String(a.s + b); }
  friend String operator+(const char *a, const String &b) { return String(a + b.s); }
  bool operator!=(const char *o) const { return s != o; } unsigned length() const { return s.size(); } const char *c_str() const { return s.c_str(); } };
inline uint32_t millis() { using namespace std::chrono; static auto t0 = steady_clock::now(); return (uint32_t)duration_cast<milliseconds>(steady_clock::now() - t0).count(); }
inline bool fx_time_known() { return false; }
inline int hour() { return 0; } inline int minute() { return 0; } inline int second() { return 0; }
struct { uint32_t getFreeHeap() { return 180000; } uint32_t getMaxAllocHeap() { return 110000; } } ESP;
inline int esp_reset_reason() { return 1; }
#define WL_CONNECTED 3
struct { int status() { return WL_CONNECTED; } } WiFi;
extern String g_c33a4, g_c3398, g_fx_crash_last;
inline void fx_crash_text(String &o) { o = ""; }
inline void dbg_ws_print(const String &s) { printf("[ws] %s\n", s.c_str()); }
inline void loop_guard_feed() {}
inline void fx_status_begin(uint8_t, bool = false) {} inline void fx_status_end() {}
#define FXS_FLASH 9
inline int ets_printf(const char *f, ...) { va_list a; va_start(a, f); int n = vprintf(f, a); va_end(a); return n; }
typedef int (*vprintf_like_t)(const char *, va_list);
inline vprintf_like_t esp_log_set_vprintf(vprintf_like_t f) { (void)f; return nullptr; }
typedef std::mutex portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED {}
#define portENTER_CRITICAL(m) (m)->lock()
#define portEXIT_CRITICAL(m) (m)->unlock()
#include "version.h"   // FX_NAME, FX_VERSION — назва й версія в шапці журналу
