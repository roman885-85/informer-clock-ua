#!/bin/sh
# Збирає симуляцію плавної яскравості зі справжніх bright.cpp, photo.cpp і dim.cpp (копії поруч із заглушкою fw.h)
set -e
D=$(cd "$(dirname "$0")" && pwd)
SRC="$D/../../../firmware/src"
B="${TMPDIR:-/tmp}/brightsim.$$"
mkdir -p "$B"
cp "$D/mock/fw.h" "$SRC/fix.h" "$SRC/version.h" "$SRC/bright.cpp" "$SRC/photo.cpp" "$SRC/dim.cpp" "$D/sim.cpp" "$B/"
sed -i '' 's#"mock/fw.h"#"fw.h"#' "$B/sim.cpp"
c++ -std=c++17 -O1 -w -I "$B" "$B/sim.cpp" "$B/bright.cpp" "$B/photo.cpp" "$B/dim.cpp" -o "$B/sim"
"$B/sim"
rm -rf "$B"
