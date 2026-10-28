#!/bin/sh
# Перевірка fx_dst_offset (firmware/src/timefix.cpp) на комп'ютері: дати переходів на літній час ЄС 2024–2031.
# Функції беруться зі справжнього timefix.cpp; TimeLib — з firmware/.pio/libdeps (спершу: cd firmware && pio run -e clock).
# EN: checks the EU summer-time switch dates using the real timefix.cpp code and TimeLib from PlatformIO libdeps.
set -e   # -D__time_t_defined: macOS не визначає цей макрос, а TimeLib інакше оголошує свій time_t
D=$(cd "$(dirname "$0")" && pwd); S="$D/../../../firmware/src"; T="$D/../../../firmware/.pio/libdeps/clock/Time"
[ -f "$T/TimeLib.h" ] || { echo "немає TimeLib: спершу cd firmware && pio run -e clock / build the firmware once first"; exit 1; }
B="${TMPDIR:-/tmp}/dsttest.$$"; mkdir -p "$B"
printf '#pragma once\n#include <cstdint>\nunsigned long millis();\n' > "$B/WProgram.h"
{ echo '#include "TimeLib.h"'; echo 'extern uint8_t g_c32e4;'
  awk '/^static time_t last_sunday\(/,/^}/' "$S/timefix.cpp"; awk '/^int32_t fx_dst_offset\(/,/^}/' "$S/timefix.cpp"; } > "$B/dst.cpp"
printf '#include <ctime>\nunsigned long millis() { return 0; }\n' > "$B/stub.cpp"
c++ -std=c++17 -w -D__time_t_defined -I "$B" -I "$T" -o "$B/t" "$D/test.cpp" "$B/dst.cpp" "$B/stub.cpp" "$T/Time.cpp"
"$B/t"
rm -rf "$B"
