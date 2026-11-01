// Заглушка Arduino для справжніх MD_MAX72XX і MD_Parola на комп'ютері (tools/sim/bigslidesim)
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <algorithm>
typedef bool boolean;
typedef uint8_t byte;
#define PROGMEM
#define pgm_read_byte(p) (*(const uint8_t *)(p))
#define LOW 0
#define HIGH 1
#define OUTPUT 1
#define INPUT 0
#define MSBFIRST 1
#define LSBFIRST 0
#define SPI_MODE0 0
#define bitRead(v, b) (((v) >> (b)) & 1)
#define bitSet(v, b) ((v) |= (1UL << (b)))
#define bitClear(v, b) ((v) &= ~(1UL << (b)))
using std::min;
using std::max;
extern uint32_t g_ms;
inline uint32_t millis() { return g_ms; }
inline void delay(uint32_t ms) { g_ms += ms; }
inline long random(long a, long b) { return a + rand() % (b - a); }
inline long random(long b) { return rand() % b; }
inline long map(long x, long a, long b, long c, long d) { return (x - a) * (d - c) / (b - a) + c; }
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
inline void shiftOut(int, int, int, uint8_t) {}
struct Print {                                    // базовий клас Arduino (MD_Parola — нащадок)
  virtual size_t write(uint8_t) = 0;
  virtual size_t write(const uint8_t *b, size_t n) { size_t r = 0; while (n--) r += write(*b++); return r; }
  size_t write(const char *s) { return s ? write((const uint8_t *)s, strlen(s)) : 0; }
  virtual ~Print() {}
};
struct SerialMock { template <class T> void print(T) {} template <class T> void println(T) {} template <class T, class U> void print(T, U) {} };
extern SerialMock Serial;
