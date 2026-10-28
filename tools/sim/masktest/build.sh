#!/bin/sh
# Перевірка mask_passwords (firmware/src/ws.cpp): паролі Wi-Fi приховано в рядках консолі. Функція — зі справжнього ws.cpp.
# EN: checks Wi-Fi password masking using the real mask_passwords() from ws.cpp.
set -e
D=$(cd "$(dirname "$0")" && pwd); S="$D/../../../firmware/src"
B="${TMPDIR:-/tmp}/masktest.$$"; mkdir -p "$B"
awk '/^static String mask_passwords\(/,/^}/' "$S/ws.cpp" > "$B/fn.inc"
cp "$D/t.cpp" "$B/"
c++ -std=c++17 -w -o "$B/t" "$B/t.cpp"
"$B/t"
rm -rf "$B"
