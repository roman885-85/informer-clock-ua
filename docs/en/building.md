# Building from source

[Українською](../uk/building.md) · [Documentation index](README.md)

## Requirements

- `git`, `python3` (3.8 or newer);
- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/) (`python3 -m pip install platformio`)
  or VS Code with the PlatformIO IDE extension;
- for releases — `esptool` (`python3 -m pip install esptool`) and `zip`.

## Firmware

```sh
git clone https://github.com/roman885-85/informer-clock-ua.git
cd informer-clock-ua/firmware
pio run -e clock                 # build: firmware/.pio/build/clock/firmware.bin
pio run -e clock -t upload       # build and write over USB (settings are kept)
pio device monitor -b 115200     # log on the USB port
```

The first build downloads exactly pinned versions (`firmware/platformio.ini`): platform `espressif32@3.0.0`, Arduino-ESP32
core **1.0.4** (`framework-arduinoespressif32 @ 3.10004.210126`, ESP-IDF 3.3), compiler `toolchain-xtensa32 @ 2.50200.97`
and the `lib_deps` libraries — the same as in the author's 2.7.1 firmware. Newer cores are not suitable: the code
reproduces the behaviour of exactly this core.

Environments:

| Environment | What it is |
|---|---|
| `clock` | normal firmware |
| `diag` | the same + web console debug lines are also printed to the USB port (`-DCOPY_DIAG`) |

The `-Wl,--wrap=…` flags in `platformio.ini` replace a few functions of core 1.0.4 (DNS, connect without timeout, setting
the time, crash handlers, core log) — explained in [architecture.md](architecture.md).

## Web interface

The web interface is the author's original React interface (`web/original/index.html.gz`, built and compressed), to
which `web/webui_patch.py` applies **exact text patches**: new fields on existing pages, this project's addresses
instead of the author's server, the project name, the change history from `update/changelog.json`.

```sh
python3 web/webui_patch.py
```

- every patch must match exactly as many times as stated, otherwise the script stops naming the patch;
- the script stops if any address of the author's server is left in the interface;
- output: `firmware/src/web_assets.h` (the `INDEX_HTML_GZ` array and a content-based `INDEX_HTML_ETAG` — browsers do
  not show a stale cached interface) and `web/build/index.html(.gz)` for previewing.

Run it after changing the patches or `update/changelog.json`, then rebuild the firmware.

### Previewing the interface without a clock

```sh
python3 web/webui_patch.py
python3 tools/sim/mockclock.py --port 8791      # http://localhost:8791
```

`mockclock.py` serves the built interface and answers requests like a clock with factory settings
(`tools/sim/mock-settings.json`); saves are only printed in the terminal.

## Making a release

1. `firmware/src/version.h` — new `FX_VERSION` (format `X.Y.Z`; clocks offer an update only when the number is higher).
2. `update/changelog.json` — a new entry at the top (`v`, `date`, `items` with `t`: `added` / `improved` / `fixed` /
   `removed`; text in Ukrainian and English separated by “ / ”). Same in `CHANGELOG.md`.
3. Build:
   ```sh
   sh tools/build-release.sh
   ```
   The script: web interface patches → PlatformIO build → `docs/firmware/` (bootloader, partitions, boot_app0, program,
   full image, `SHA256SUMS.txt`) → `update/esp32.json` (version, size, MD5, SHA-256, file address with the `vX.Y.Z` tag)
   → `docs/flasher/manifest.json` → the `release/informer-clock-ua-X.Y.Z-flash.zip` archive.
   Variables: `PIO=…` (path to pio), `ESPTOOL=…`, `PLATFORMIO_CORE_DIR=…`.
4. Test on a clock.
5. Commit, tag and push **together** (the file address in `esp32.json` points to the tag):
   ```sh
   git commit -am "Release X.Y.Z"
   git tag vX.Y.Z
   git push origin main vX.Y.Z
   ```
6. Create the GitHub release (`gh release create vX.Y.Z release/informer-clock-ua-X.Y.Z-flash.zip docs/firmware/*.bin
   docs/firmware/SHA256SUMS.txt`). Clocks see the update as soon as `update/esp32.json` is on `main` (GitHub cache — up
   to 5 min).

## Checks and simulators (`tools/sim`)

Small programs that build the **real** firmware code on a computer with core stubs and check its behaviour:

| What | Run | Checks |
|---|---|---|
| `brightsim` | `sh tools/sim/brightsim/build.sh` | smooth brightness (`bright.cpp`, `photo.cpp`) |
| `voicesim` | `sh tools/sim/voicesim/build.sh` | the hourly voice state machine (`sound.cpp`) |
| `slidesim` | `sh tools/sim/slidesim/run.sh` | “stray dots” at the hour change on the sliding-digits clock face |
| `wdbgsim` | `sh tools/sim/wdbgsim/build.sh`, then run the printed file and `nc localhost 2323` | network log (`wdbg.cpp`, port 2323 instead of 23) |
| `dsttest` | `sh tools/sim/dsttest/build.sh` (after `pio run`: needs TimeLib) | EU summer-time switch dates 2024–2031 |
| `masktest` | `sh tools/sim/masktest/build.sh` | Wi-Fi password masking in the console (`ws.cpp`) |
| `statusprev` | `python3 tools/sim/statusprev/preview.py 1 out.gif --rows 2` | how the start-up status looks on the display (GIF) |
| `status_icons.py` | `python3 tools/sim/status_icons.py --png icons.png` | frames of the start-up status icons |
| `mockclock.py` | see above | the web interface without a clock |

Documentation tools: `tools/docs/wiring_svg.py` (wiring diagram), `tools/docs/build-pdf.mjs` (documentation as PDF:
`npm install --prefix tools/docs markdown-it@14`, then `node tools/docs/build-pdf.mjs` — a book per language and the
separate parts in `release/docs-pdf/`, needs Chrome), `tools/build-release.sh` (release), `tools/flash/` (flashing scripts).

## Repository layout

```
firmware/            firmware (PlatformIO): src/ — code, lib/ — libraries, partitions_clock.csv
web/original/        the author's original web interface (built React, gzip)
web/webui_patch.py   interface patches → firmware/src/web_assets.h
update/              esp32.json (latest version for clocks), changelog.json (change history for the interface)
lang/                interface (*.json) and display (*.led) language packs — from the original project
docs/                project site (GitHub Pages): flasher, manual, firmware files, documentation
tools/               flashing and release scripts, simulators
```
