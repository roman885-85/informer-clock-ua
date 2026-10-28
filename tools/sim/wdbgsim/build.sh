#!/bin/sh
# Збирає перевірку сетевого журналу зі справжнього wdbg.cpp (порт 2323 замість 23)
set -e
D=$(cd "$(dirname "$0")" && pwd)
B="${TMPDIR:-/tmp}/wdbgsim.$$"
mkdir -p "$B/lwip"
cp "$D/mock/fw.h" "$D/mock/ArduinoOTA.h" "$D/../../../firmware/src/version.h" "$B/"; cp "$D/mock/lwip/sockets.h" "$B/lwip/"; cp "$D/sim.cpp" "$B/"
sed 's/htons(23)/htons(2323)/' "$D/../../../firmware/src/wdbg.cpp" > "$B/wdbg.cpp"
c++ -std=c++17 -w -I "$B" "$B/sim.cpp" "$B/wdbg.cpp" -o "$B/sim"
echo "$B/sim"
