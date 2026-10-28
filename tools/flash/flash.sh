#!/bin/sh
# Informer Clock UA — прошивка годинника через USB (macOS, Linux). EN: flash the clock over USB (macOS, Linux).
#
#   ./flash.sh update [порт]   оновлення: лише програма, налаштування й Wi-Fi зберігаються (і при переході з прошивки
#                              автора Clock Informer) / update: firmware only, settings and Wi-Fi are kept
#   ./flash.sh full   [порт]   чиста установка: стерти все й записати заново (налаштування скинуться)
#                              / clean install: erase everything and write all parts (settings are reset)
#
# Порт / port: /dev/cu.usbserial-* (macOS), /dev/ttyUSB0 (Linux); без порту — шукається сам / auto-detected.
# Файли прошивки — з теки firmware/ поруч (архів випуску), з docs/firmware/ репозиторію або завантажуються з GitHub. esptool — з PATH або ставиться
# у ~/.cache/informer-clock-ua/venv (потрібен python3). Швидкість: BAUD=460800 ./flash.sh update (типово 230400).
set -eu
MODE=${1:-}
PORT=${2:-}
BAUD=${BAUD:-230400}
RAW=https://raw.githubusercontent.com/roman885-85/informer-clock-ua/main/docs/firmware
HERE=$(cd "$(dirname "$0")" && pwd)
FW="$HERE/../../docs/firmware"                       # у репозиторії
[ -s "$HERE/firmware/informer-clock-ua-esp32.bin" ] && FW="$HERE/firmware"   # архів випуску: файли поруч
CACHE="$HOME/.cache/informer-clock-ua"

say() { printf '\n▶ %s\n' "$1"; }
die() { printf '\n✗ %s\n' "$1" >&2; exit 1; }

if [ -z "$MODE" ] && [ -t 0 ]; then
  echo "Informer Clock UA — прошивка через USB / USB flashing"
  echo "  1 — оновлення, налаштування зберігаються / update, settings kept"
  echo "  2 — чиста установка, стерти все / clean install, erase all"
  printf "Ваш вибір / your choice [1/2]: "; read -r a
  case "$a" in 1) MODE=update ;; 2) MODE=full ;; esac
fi
case "$MODE" in update|full) ;; *)
  echo "Використання / usage: $0 update|full [порт]"; exit 1 ;; esac

# --- esptool
ESPTOOL=""
if command -v esptool.py >/dev/null 2>&1; then ESPTOOL="esptool.py"
elif command -v esptool >/dev/null 2>&1; then ESPTOOL="esptool"
elif python3 -c "import esptool" >/dev/null 2>&1; then ESPTOOL="python3 -m esptool"
else
  command -v python3 >/dev/null 2>&1 || die "потрібен python3 / python3 is required (https://www.python.org/)"
  if [ ! -x "$CACHE/venv/bin/esptool.py" ]; then
    say "Ставлю esptool у $CACHE/venv / installing esptool"
    mkdir -p "$CACHE"
    rm -rf "$CACHE/venv"
    python3 -m venv "$CACHE/venv" || die "python3 не створив venv / python3 venv failed"
    # старий pip (напр. 19.x з Python 3.8) не ставить залежності esptool — спершу оновити pip
    "$CACHE/venv/bin/python" -m pip -q install --upgrade pip >/dev/null 2>&1 || true
    "$CACHE/venv/bin/python" -m pip -q install "esptool==4.8.1" || {
      rm -rf "$CACHE/venv"
      die "не вдалося встановити esptool / esptool install failed: python3 -m pip install esptool"; }
  fi
  ESPTOOL="$CACHE/venv/bin/esptool.py"
fi

# --- файли прошивки
FILES="bootloader.bin partitions.bin boot_app0.bin informer-clock-ua-esp32.bin"
if [ ! -s "$FW/informer-clock-ua-esp32.bin" ]; then
  FW="$CACHE/firmware"; mkdir -p "$FW"
  say "Завантажую прошивку з GitHub / downloading firmware"
  for f in $FILES; do curl -fsSL -o "$FW/$f" "$RAW/$f" || die "не вдалося завантажити $f / download failed"; done
fi
for f in $FILES; do [ -s "$FW/$f" ] || die "немає файла $FW/$f / missing file"; done

# --- порт
if [ -z "$PORT" ]; then
  FOUND=""; N=0
  for p in /dev/cu.usbserial-* /dev/cu.SLAB_USBtoUART /dev/cu.wchusbserial* /dev/cu.usbmodem* /dev/ttyUSB* /dev/ttyACM*; do
    [ -e "$p" ] && { FOUND="$FOUND $p"; N=$((N + 1)); }
  done
  [ "$N" -gt 0 ] || die "годинник не знайдено: під'єднайте USB-кабель (з передачею даних) / clock not found on USB"
  if [ "$N" -eq 1 ]; then PORT=${FOUND# }
  else
    # кілька пристроїв: не вгадуємо, щоб не прошити чужу плату / several devices: never guess
    echo "Знайдено порти / ports found:$FOUND"
    [ -t 0 ] || die "вкажіть порт годинника / pass the clock port: $0 $MODE <порт>"
    printf "Порт годинника / clock port: "; read -r PORT
    [ -e "$PORT" ] || die "немає порту $PORT / no such port"
  fi
fi
say "Порт / port: $PORT, швидкість / baud: $BAUD"

HINT="запис не вдався: інший кабель чи порт; тримайте кнопку BOOT на платі, поки не почнеться запис; BAUD=115200 sh flash.sh …
   / write failed: try another cable or port; hold BOOT until writing starts; or BAUD=115200"
if [ "$MODE" = full ]; then
  say "Стираю флеш-пам'ять / erasing flash"
  $ESPTOOL --chip esp32 --port "$PORT" --baud "$BAUD" erase_flash || die "$HINT"
  say "Записую все / writing all parts"
  $ESPTOOL --chip esp32 --port "$PORT" --baud "$BAUD" write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect \
    0x1000 "$FW/bootloader.bin" 0x8000 "$FW/partitions.bin" 0xe000 "$FW/boot_app0.bin" 0x10000 "$FW/informer-clock-ua-esp32.bin" \
    || die "$HINT"
else
  say "Записую програму (налаштування зберігаються) / writing firmware, settings kept"
  $ESPTOOL --chip esp32 --port "$PORT" --baud "$BAUD" write_flash -z \
    0xe000 "$FW/boot_app0.bin" 0x10000 "$FW/informer-clock-ua-esp32.bin" || die "$HINT"
fi
say "Готово. Годинник перезапуститься сам. / Done, the clock restarts by itself."
[ "$MODE" = full ] && echo "Далі: під'єднайтеся до Wi-Fi «Clock ESP» і відкрийте http://192.168.4.1 / then join Wi-Fi \"Clock ESP\" and open http://192.168.4.1"
exit 0
