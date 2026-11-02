#!/bin/sh
# Перевіряє news_cut_tail (firmware/src/feeds.cpp, 1.1.3): GNews на безкоштовному тарифі обрізає текст статті й дописує
# «... [2072 chars]» — функція прибирає хвіст і закінчує текст на повному реченні або цілому слові з «...».
# Збирає справжню функцію зі справжнім String ядра Arduino-ESP32 (те саме ядро, що й прошивка):
# $PLATFORMIO_CORE_DIR (типово ~/.platformio)/packages/framework-arduinoespressif32/cores/esp32.
# Свій рядок: ./run.sh "текст... [123 chars]"
set -e
D=$(cd "$(dirname "$0")" && pwd); S="$D/../../../firmware/src"
C="${PLATFORMIO_CORE_DIR:-$HOME/.platformio}/packages/framework-arduinoespressif32/cores/esp32"
[ -f "$C/WString.cpp" ] || { echo "немає ядра Arduino-ESP32: $C"; exit 1; }
B="${TMPDIR:-/tmp}/newstail.$$"; mkdir -p "$B"
cp "$D/sim.cpp" "$D/mock/Arduino.h" "$D/mock/libc_extra.c" "$B/"
awk '/^static void news_cut_tail\(String &c\) \{/,/^}/' "$S/feeds.cpp" > "$B/cut.inc"
[ -s "$B/cut.inc" ] || { echo "news_cut_tail у feeds.cpp не знайдено"; exit 1; }
(cd "$B" && cc -w -I. -I"$C" -c "$C/stdlib_noniso.c" -o noniso.o && cc -w -c libc_extra.c -o libc_extra.o \
  && c++ -std=c++17 -w -I. -I"$C" -o sim sim.cpp "$C/WString.cpp" noniso.o libc_extra.o)
"$B/sim" "$@"
