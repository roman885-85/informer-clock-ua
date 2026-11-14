#!/bin/sh
# Імітатор часу годинника: справжні time.cpp, timefix.cpp (з 1.1.6 — і ntp.cpp) прошивки зі справжньою TimeLib 1.6.1 і
# справжнім String ядра Arduino-ESP32 1.0.4 на моделі мережі (world.cpp). Двічі: код робочої копії й, для порівняння,
# випуску 1.1.5 (git v1.1.5) з моделлю клієнта SNTP ядра за машинним кодом 1.1.5. Сценарії — 14.11.2026 (годинник
# 12 год 23 хв 42 с позаду) і перевірки нового клієнта NTP. Див. sim.cpp.
#   ./run.sh       — підсумки сценаріїв
#   ./run.sh -v    — з рядками налагодження й журналу подій
# Потрібно: ядро Arduino-ESP32 у $PLATFORMIO_CORE_DIR (типово ~/.platformio). TimeLib: для 1.1.6 — firmware/lib/Time
# (копія з замком; у одній задачі поводиться як оригінал), для 1.1.5 — оригінал 1.6.1 з git (коміт, що додав lib/Time).
set -e
D=$(cd "$(dirname "$0")" && pwd); R=$(cd "$D/../../.." && pwd); S="$R/firmware/src"
C="${PLATFORMIO_CORE_DIR:-$HOME/.platformio}/packages/framework-arduinoespressif32/cores/esp32"
[ -f "$C/WString.cpp" ] || { echo "немає ядра Arduino-ESP32: $C"; exit 1; }
B="${TMPDIR:-/tmp}/ntpsim.$$"; mkdir -p "$B/c" "$B/old/tl" "$B/new"
trap 'rm -rf "$B"' EXIT
TC=$(git -C "$R" log --diff-filter=A --format=%H -1 -- firmware/lib/Time/Time.cpp)
[ -n "$TC" ] || { echo "у git немає коміту з оригінальною firmware/lib/Time"; exit 1; }
for f in Time.cpp TimeLib.h; do git -C "$R" show "$TC:firmware/lib/Time/$f" > "$B/old/tl/$f"; done
T="$R/firmware/lib/Time"
# __time_t_defined: macOS його не визначає, а TimeLib інакше оголосить свій time_t
F="-w -DARDUINO=10804 -D__time_t_defined -I$D/mock -I$C"
cc -w -I"$D/mock" -I"$C" -c "$C/stdlib_noniso.c" -o "$B/c/noniso.o"
cc -w -c "$D/mock/libc_extra.c" -o "$B/c/libc_extra.o"
c++ -std=c++17 $F -c "$C/WString.cpp" -o "$B/c/WString.o"
c++ -std=c++17 $F -I"$B/old/tl" -c "$B/old/tl/Time.cpp" -o "$B/old/Time.o"
c++ -std=c++17 $F -I"$T" -c "$T/Time.cpp" -o "$B/new/Time.o"
for f in time.cpp timefix.cpp fix.h version.h; do git -C "$R" show "v1.1.5:firmware/src/$f" > "$B/old/$f"; done
for f in time.cpp timefix.cpp ntp.cpp fix.h version.h; do cp "$S/$f" "$B/new/$f"; done
cp "$D/mock/fw.h" "$B/old/"; cp "$D/mock/fw.h" "$B/new/"
c++ -std=c++17 $F -DOLD -I"$B/old" -I"$B/old/tl" -o "$B/old/sim" "$D/sim.cpp" "$D/world.cpp" "$B/old/time.cpp" \
  "$B/old/timefix.cpp" "$B/old/Time.o" "$B"/c/*.o
c++ -std=c++17 $F -I"$B/new" -I"$T" -o "$B/new/sim" "$D/sim.cpp" "$D/world.cpp" "$B/new/time.cpp" "$B/new/timefix.cpp" \
  "$B/new/ntp.cpp" "$B/new/Time.o" "$B"/c/*.o
r=0
"$B/old/sim" "$@" || r=1
echo
"$B/new/sim" "$@" || r=1
exit $r
