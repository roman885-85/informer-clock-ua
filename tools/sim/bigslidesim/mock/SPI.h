#pragma once
#include "Arduino.h"
struct SPISettings { SPISettings(uint32_t, int, int) {} };
struct SPIClass { void begin() {} void end() {} void beginTransaction(SPISettings) {} void endTransaction() {} uint8_t transfer(uint8_t) { return 0; } };
extern SPIClass SPI;
