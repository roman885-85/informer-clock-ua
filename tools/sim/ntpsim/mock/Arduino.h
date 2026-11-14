// Заглушка Arduino.h для імітатора часу (tools/sim/ntpsim): String ядра Arduino-ESP32 1.0.4 (справжній WString.cpp),
// модельний millis()/delay(), IPAddress, WiFi.hostByName і критичні секції FreeRTOS (на комп'ютері — порожні).
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <sys/time.h>
#include "WString.h"

uint32_t millis();
void delay(uint32_t ms);
void yield();
uint32_t esp_random();

// як в ядрі: адреса в порядку мережі, перший октет — молодший байт
class IPAddress {
  uint32_t a;
public:
  IPAddress() : a(0) {}
  IPAddress(uint32_t v) : a(v) {}
  IPAddress(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3) : a(b0 | (b1 << 8) | (b2 << 16) | ((uint32_t)b3 << 24)) {}
  operator uint32_t() const { return a; }
  String toString() const {
    char b[16];
    snprintf(b, sizeof b, "%u.%u.%u.%u", a & 255, (a >> 8) & 255, (a >> 16) & 255, a >> 24);
    return String(b);
  }
};
struct SimWiFi { int hostByName(const char *host, IPAddress &out); };
extern SimWiFi WiFi;

// ядро 1.0.4 (esp32-hal-time.c): запускає клієнт SNTP lwIP — тут модель за машинним кодом 1.1.5 (world.cpp)
void configTime(long gmtOffset_sec, int daylightOffset_sec, const char *server1, const char *server2 = nullptr,
                const char *server3 = nullptr);

typedef int portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED 0
#define portENTER_CRITICAL(m) ((void)(m))
#define portEXIT_CRITICAL(m) ((void)(m))
