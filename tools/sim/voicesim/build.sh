#!/bin/sh
# Збирає симуляцію голосу з функцій voice_hourly і f_400e3500 файлу firmware/src/sound.cpp
set -e
D=$(cd "$(dirname "$0")" && pwd)
B="${TMPDIR:-/tmp}/voicesim.$$"
mkdir -p "$B"
awk '/^void f_400e3500\(\)/,/^}/' "$D/../../../firmware/src/sound.cpp" > "$B/voice_fns.inc"
awk '/^void voice_hourly\(\)/,/^}/' "$D/../../../firmware/src/sound.cpp" >> "$B/voice_fns.inc"
cp "$D/sim.cpp" "$B/"
c++ -std=c++17 -w -I "$B" "$B/sim.cpp" -o "$B/sim"
"$B/sim"
rm -rf "$B"
# --orig: те саме на коді автора (тег reconstructed-2.7.1) — для порівняння
if [ "$1" = "--orig" ]; then
  B="${TMPDIR:-/tmp}/voicesim-o.$$"; mkdir -p "$B"
  git -C "$D/../../.." show reconstructed-2.7.1:firmware/src/sound.cpp > "$B/sound.cpp"
  awk '/^void f_400e3500\(\)/,/^}/' "$B/sound.cpp" > "$B/voice_fns.inc"
  awk '/^void voice_hourly\(\)/,/^}/' "$B/sound.cpp" >> "$B/voice_fns.inc"
  sed 's/^  printf("== кнопка.*$/  return 0;/' "$D/sim.cpp" > "$B/sim.cpp"
  c++ -std=c++17 -w -I "$B" "$B/sim.cpp" -o "$B/sim" && echo "== КОД АВТОРА:" && "$B/sim"
  rm -rf "$B"
fi
