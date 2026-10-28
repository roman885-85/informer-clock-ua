#!/bin/sh
# Збирає симуляцію (справжні clock_slide.cpp, fonts.cpp, colon_timing) і порівнює перехід через межу години з «чистим»
# стартом для nz 0/1, обох переходів і шрифтів font_l 0–12. Вихід: таблиця «лишніх точок».
set -e
D=$(cd "$(dirname "$0")" && pwd); S="$D/../../../firmware/src"
B="${TMPDIR:-/tmp}/slidesim.$$"; mkdir -p "$B"
cp "$S/clock_slide.cpp" "$S/fonts.cpp" "$S/fonts.h" "$S/fonts_2_7_1.h" "$S/sprites_2_7_1.h" "$D/mock/fw.h" "$D/mock/Arduino.h" "$D/mock/globals.inc" "$D/defs.inc" "$D/sim.cpp" "$B/"
awk '/^void colon_timing\(int8_t ph\) \{/,/^}/' "$S/display.cpp" > "$B/colon.inc"
(cd "$B" && c++ -std=c++17 -w -I. -o sim sim.cpp clock_slide.cpp fonts.cpp)
LAYOUTS=${LAYOUTS:-"0 1 2 3 4 5 6 7 8"}
for L in $LAYOUTS; do for nz in 0 1; do for sc in 0 1; do line=""; for f in 0 1 2 3 4 5 6 7 8 9 10 11 12; do
  a=$("$B/sim" $nz $sc $f t q $L ${PAN:-2} | grep -E "^[0-9a-f]+$" | tail -1); c=$("$B/sim" $nz $sc $f c q $L ${PAN:-2} | grep -E "^[0-9a-f]+$" | tail -1)
  n=$(python3 -c "import sys;a=bytes.fromhex('$a');c=bytes.fromhex('$c');print(sum(bin(x^y).count('1') for x,y in zip(a,c)))")
  line="$line $n"; done
  echo "pAn=${PAN:-2} розкладка $L nz=$nz $( [ $sc = 0 ] && echo '9:59→10:00 ' || echo '23:59→0:00 ') лишні точки за font_l 0..12:$line"; done; done; done
echo "$B/sim"
# --orig: ті самі «чисті» кадри на коді автора (тег reconstructed-2.7.1) — відмінність = обрізане обмеженням зони
if [ "$1" = "--orig" ]; then
  O="${TMPDIR:-/tmp}/slidesim-o.$$"; mkdir -p "$O"; cp "$B"/* "$O/" 2>/dev/null || true
  git -C "$D/../../.." show reconstructed-2.7.1:firmware/src/clock_slide.cpp > "$O/clock_slide.cpp"
  (cd "$O" && c++ -std=c++17 -w -I. -o sim sim.cpp clock_slide.cpp fonts.cpp)
  bad=0
  for L in $LAYOUTS; do for nz in 0 1; do for sc in 0 1; do for f in 0 1 2 3 4 5 6 7 8 9 10 11 12; do
    x=$("$B/sim" $nz $sc $f c q $L | grep -E "^[0-9a-f]+$" | tail -1); y=$("$O/sim" $nz $sc $f c q $L | grep -E "^[0-9a-f]+$" | tail -1)
    [ "$x" = "$y" ] || { bad=$((bad+1)); echo "відмінність: розкладка $L nz=$nz сценарій $sc font_l $f"; }
  done; done; done; done
  echo "«чисті» кадри, відмінностей від коду автора: $bad"
fi
