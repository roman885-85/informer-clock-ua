#!/bin/sh
# Гонка TimeLib між двома задачами (race.cpp): спершу оригінальна TimeLib 1.6.1 (такою вона була в прошивці до 1.1.5
# включно — з git, коміт, що додав firmware/lib/Time без змін), потім копія з замком (firmware/lib/Time).
#   ./run.sh [секунд=20]
# Очікується: оригінал — стрибок now() на ≈49,7 доби за секунди; копія — жодного відхилення понад 1 с.
set -e
D=$(cd "$(dirname "$0")" && pwd); R=$(cd "$D/../../.." && pwd); L="$R/firmware/lib/Time"
S=${1:-20}
B="${TMPDIR:-/tmp}/tlrace.$$"; mkdir -p "$B/orig"
trap 'rm -rf "$B"' EXIT
C=$(git -C "$R" log --diff-filter=A --format=%H -1 -- firmware/lib/Time/Time.cpp)
[ -n "$C" ] || { echo "у git немає коміту з оригінальною firmware/lib/Time"; exit 1; }
git -C "$R" show "$C:firmware/lib/Time/Time.cpp" > "$B/orig/Time.cpp"
git -C "$R" show "$C:firmware/lib/Time/TimeLib.h" > "$B/orig/TimeLib.h"
F="-std=c++17 -O1 -w -DARDUINO=10804 -D__time_t_defined -I$D"
c++ $F -I"$B/orig" -o "$B/orig/race" "$D/race.cpp" "$B/orig/Time.cpp" -lpthread
c++ $F -I"$L" "-DTL_LOCK()=tl_lock()" "-DTL_UNLOCK()=tl_unlock()" -o "$B/fork" "$D/race.cpp" "$L/Time.cpp" -lpthread
echo "Оригінальна TimeLib 1.6.1 (до 1.1.5 включно):"
if "$B/orig/race" "$S"; then echo "  стрибка немає (очікувався)"; o=1; else echo "  стрибок відтворено"; o=0; fi
echo "Копія з замком (firmware/lib/Time, 1.1.6):"
if "$B/fork" "$S"; then echo "  стрибка немає"; f=0; else echo "  СТРИБОК — замок не допоміг"; f=1; fi
echo "Ручне встановлення з іншої задачі (setTime), копія з замком:"
if "$B/fork" "$S" s; then echo "  стрибка немає"; else echo "  СТРИБОК"; f=1; fi
[ $o -eq 0 ] && [ $f -eq 0 ]
