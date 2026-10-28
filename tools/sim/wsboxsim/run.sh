#!/bin/sh
# Перевірка черги повідомлень веб-інтерфейсу: справжній код з firmware/src/ws.cpp (від «#define WS_BOX» до кінця
# ws_async_hook) зі заглушкою AsyncWebSocket.
set -e
D=$(cd "$(dirname "$0")" && pwd); S="$D/../../../firmware/src"
B="${TMPDIR:-/tmp}/wsboxsim.$$"; mkdir -p "$B"
awk '/^#define WS_BOX/{f=1} f{print} f&&/^void ws_async_hook/{h=1} h&&/^}/{exit}' "$S/ws.cpp" > "$B/box.inc"
cp "$D/sim.cpp" "$B/"
(cd "$B" && c++ -std=c++17 -O1 -w -I. -o sim sim.cpp)
"$B/sim"; rc=$?
rm -rf "$B"; exit $rc
