// Заглушка Arduino.h для перевірки гонки TimeLib (tools/sim/timelibrace): лише millis() з модельного лічильника.
#pragma once
#include <stdint.h>
uint32_t millis();
// замок копії TimeLib на комп'ютері (run.sh: -DTL_LOCK()=tl_lock()), у race.cpp — std::mutex
void tl_lock();
void tl_unlock();
