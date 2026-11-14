# Time (TimeLib) 1.6.1 — форк / fork

Оригінал / original: [Time](https://github.com/PaulStoffregen/Time) 1.6.1 (Michael Margolis, Paul Stoffregen),
ліцензія / license **LGPL-2.1** (заголовок / header of `Time.cpp`). До 1.1.5 бралась з реєстру PlatformIO
(`paulstoffregen/Time @ 1.6.1`), з 1.1.6 — з цієї теки / used from the PlatformIO registry up to 1.1.5, from this folder
since 1.1.6.

Змінено / modified (2026-11-14, Informer Clock UA), лише `Time.cpp`:

| Місце / where | Зміна / change |
|---|---|
| `now()` | під замком (`portENTER_CRITICAL`), різниця `millis() - prevMillis` — зі знаком. У годиннику `now()` кличуть дві задачі: головний цикл і потік AsyncTCP (веб: `/content?sys_time=`, ручне встановлення часу; журнал подій з MQTT). Коли одна прочитала `millis()`, а друга встигла перейти межу секунди й посунути `prevMillis`, беззнакова різниця ставала ≈4,29 млрд, і цикл додавав ≈4 294 967 с — час стрибав на 49,7 доби вперед до наступної синхронізації (відтворено: `tools/sim/timelibrace`) / under a lock, signed difference: with two tasks calling `now()`, an unsigned negative difference made it add ≈4,294,967 s (49.7 days) until the next sync |
| `setTime(time_t)`, `adjustTime()` | під замком / under the lock |
| `hour(t)`, `minute(t)`, `second(t)`, `day(t)`, `weekday(t)`, `month(t)`, `year(t)`, `hourFormat12(t)` | оновлення спільного кешу `tm` і читання поля — під замком: інакше задача могла отримати поле іншого часу / the shared `tm` cache update and the field read are under the lock: otherwise a task could read a field of another time |
| `setTime(год, хв, с, д, м, р)` | власна структура замість спільного кешу `tm` / own struct instead of the shared `tm` cache |

Замок — спін-замок FreeRTOS (`portMUX`), лише на ESP32; поза ESP32 (або з `-DTL_LOCK()=…`) — як в оригіналі. Решта
файлів — без змін / the lock is a FreeRTOS spinlock (ESP32 only); all other files are unchanged.
