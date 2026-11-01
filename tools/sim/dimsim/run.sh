#!/bin/sh
# Збирає симуляцію плавної яскравості зі справжнього dim.cpp (крок 1 мс) і друкує таблицю переходів.
#   run.sh            — таблиця й підсумок;   run.sh csv 0 12 — сліди для графіка (мс;шпаруватість;середня за 20 мс)
set -e
D=$(cd "$(dirname "$0")" && pwd); S="$D/../../../firmware/src"
B="${TMPDIR:-/tmp}/dimsim.$$"; mkdir -p "$B"
cp "$D/mock/fw.h" "$S/fix.h" "$S/version.h" "$S/dim.cpp" "$D/sim.cpp" "$B/"
(cd "$B" && c++ -std=c++17 -O1 -w -I. -o sim sim.cpp dim.cpp)
"$B/sim" "$@"; rc=$?
rm -rf "$B"; exit $rc
