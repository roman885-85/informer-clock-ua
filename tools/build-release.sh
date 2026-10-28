#!/bin/sh
# Informer Clock UA — збірка випуску. EN: build a release.
#   1) веб-інтерфейс: web/webui_patch.py → firmware/src/web_assets.h
#   2) прошивка: PlatformIO (pio run -e clock), таблиця розділів
#   3) docs/firmware/: bootloader.bin, partitions.bin, boot_app0.bin, informer-clock-ua-esp32.bin (+ -full.bin одним файлом)
#   4) docs/flasher/manifest.json (прошивальник у браузері) і update/esp32.json (опис версії для оновлення годинника)
#   5) release/informer-clock-ua-<версія>-flash.zip — скрипти й файли прошивки одним архівом (для випуску на GitHub)
# Номер версії — FX_VERSION у firmware/src/version.h. Потрібні: python3, PlatformIO (pio), esptool (pip install esptool).
set -eu
ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"
PIO=${PIO:-pio}
command -v "$PIO" >/dev/null 2>&1 || PIO="$HOME/.platformio/penv/bin/pio"
CORE=${PLATFORMIO_CORE_DIR:-$HOME/.platformio}
FWK="$CORE/packages/framework-arduinoespressif32"
VER=$(sed -n 's/^#define FX_VERSION "\(.*\)"$/\1/p' firmware/src/version.h)
[ -n "$VER" ] || { echo "немає FX_VERSION у firmware/src/version.h"; exit 1; }
echo "▶ версія / version $VER"

python3 web/webui_patch.py
(cd firmware && "$PIO" run -e clock)
# partitions.bin з'являється під час upload-підготовки; згенеруємо самі тим самим скриптом ядра
B=firmware/.pio/build/clock
python3 "$FWK/tools/gen_esp32part.py" -q firmware/partitions_clock.csv "$B/partitions.bin"

OUT=docs/firmware
mkdir -p "$OUT"
cp "$FWK/tools/sdk/bin/bootloader_dio_40m.bin" "$OUT/bootloader.bin"
cp "$B/partitions.bin" "$OUT/partitions.bin"
cp "$FWK/tools/partitions/boot_app0.bin" "$OUT/boot_app0.bin"
cp "$B/firmware.bin" "$OUT/informer-clock-ua-esp32.bin"
cp "$B/firmware.elf" "firmware/.pio/informer-clock-ua-esp32-$VER.elf"   # для адрес аварій (не публікується в docs/)
ESPTOOL=${ESPTOOL:-}
[ -n "$ESPTOOL" ] || { python3 -c "import esptool" 2>/dev/null && ESPTOOL="python3 -m esptool"; } || true
[ -n "$ESPTOOL" ] || ESPTOOL=esptool
$ESPTOOL --chip esp32 merge_bin -o "$OUT/informer-clock-ua-esp32-full.bin" --flash_mode dio --flash_freq 40m --flash_size 4MB \
  0x1000 "$OUT/bootloader.bin" 0x8000 "$OUT/partitions.bin" 0xe000 "$OUT/boot_app0.bin" 0x10000 "$OUT/informer-clock-ua-esp32.bin"

python3 - "$VER" <<'PY'
import hashlib, json, os, sys, datetime
ver = sys.argv[1]
app = 'docs/firmware/informer-clock-ua-esp32.bin'
b = open(app, 'rb').read()
# адреса файла — за тегом версії (v<версія>): незмінна, кеш GitHub не віддасть іншу збірку під тим самим ім'ям
raw = f'https://raw.githubusercontent.com/roman885-85/informer-clock-ua/v{ver}'
json.dump({'ver': ver, 'date': datetime.date.today().isoformat(), 'bin': f'{raw}/{app}', 'size': len(b),
           'md5': hashlib.md5(b).hexdigest(), 'sha256': hashlib.sha256(b).hexdigest(),
           'notes': f'https://github.com/roman885-85/informer-clock-ua/releases/tag/v{ver}'},
          open('update/esp32.json', 'w'), indent=2)
json.dump({'name': 'Informer Clock UA', 'version': ver, 'new_install_prompt_erase': True,
           'builds': [{'chipFamily': 'ESP32', 'parts': [
               {'path': '../firmware/bootloader.bin', 'offset': 4096},
               {'path': '../firmware/partitions.bin', 'offset': 32768},
               {'path': '../firmware/boot_app0.bin', 'offset': 57344},
               {'path': '../firmware/informer-clock-ua-esp32.bin', 'offset': 65536}]}]},
          open('docs/flasher/manifest.json', 'w'), indent=2)
sums = []
for f in sorted(os.listdir('docs/firmware')):
    if f.endswith('.bin'):
        sums.append(f'{hashlib.sha256(open("docs/firmware/" + f, "rb").read()).hexdigest()}  {f}')
open('docs/firmware/SHA256SUMS.txt', 'w').write('\n'.join(sums) + '\n')
print('update/esp32.json, docs/flasher/manifest.json, docs/firmware/SHA256SUMS.txt — готово')
PY

# архів для випуску на GitHub: скрипти прошивки + файли прошивки поруч (працює без інтернету й без клонування)
PKG="informer-clock-ua-$VER-flash"
rm -rf "release/$PKG" "release/$PKG.zip"
mkdir -p "release/$PKG/firmware"
cp tools/flash/flash.sh tools/flash/flash.ps1 tools/flash/flash.bat tools/flash/README.txt "release/$PKG/"
cp docs/firmware/*.bin docs/firmware/SHA256SUMS.txt "release/$PKG/firmware/"
chmod +x "release/$PKG/flash.sh"
(cd release && zip -qr "$PKG.zip" "$PKG")
ls -la docs/firmware release/"$PKG.zip"
