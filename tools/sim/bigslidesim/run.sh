#!/bin/sh
# Збирає симуляцію перелистування цифр великого циферблата: справжні bigslide.cpp, шрифти автора (fonts.cpp) і
# бібліотеки табло з firmware/lib (MD_Parola, MD_MAX72XX) із заглушкою Arduino. run.sh pgm — ще й кадри у файл.
set -e
D=$(cd "$(dirname "$0")" && pwd); S="$D/../../../firmware/src"; L="$D/../../../firmware/lib"
B="${TMPDIR:-/tmp}/bigslidesim.$$"; mkdir -p "$B"
cp "$S/bigslide.cpp" "$S/fonts.cpp" "$S/fonts.h" "$S/fonts_2_7_1.h" "$S/sprites_2_7_1.h" "$D/mock/"* "$D/sim.cpp" "$B/"
cp "$L"/MD_MAX72XX/src/* "$L"/MD_Parola/src/* "$B/"
cd "$B"
c++ -std=c++17 -O1 -w -I. -DARDUINO=10800 -o sim sim.cpp bigslide.cpp fonts.cpp MD_MAX72xx*.cpp MD_P*.cpp
./sim "$@"; rc=$?
[ "$1" = pgm ] && cp slide-*.pgm "$D/" 2>/dev/null || true
cd /; rm -rf "$B"; exit $rc
