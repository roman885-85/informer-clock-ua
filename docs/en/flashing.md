# Flashing the clock

[Українською](../uk/flashing.md) · [Documentation index](README.md)

## What is supported

- Board: **ESP32** (classic ESP32-WROOM-32, e.g. ESP32 DevKit V1 with 30 or 38 pins), **4 MB** flash.
- ESP8266, ESP32-C3, ESP32-C6 and ESP32-S3 are **not supported** by this project (the original Clock Informer supports
  them — see the author's page).
- The clock is built after the original Clock Informer schematic: [wiring](wiring.md).

## Update or clean install

| | Update | Clean install |
|---|---|---|
| What is written | the program only (and the boot partition selector) | everything: bootloader, partition table, program |
| Settings, Wi-Fi, keys, language packs | **kept** | erased |
| When | switching from the original Clock Informer firmware, a new Informer Clock UA version | a new clock, “something is badly wrong” |

The Informer Clock UA partition table is the same as in the author's firmware (nvs, otadata, app0, app1, spiffs), so
switching from Clock Informer is an ordinary update: settings and Wi-Fi stay.

The **first time**, Informer Clock UA is written over USB (any method below). After that, updates come over the
network — “Update” → “Update” in the web interface.

## Method 1. From the browser (easiest)

You need Chrome, Edge or Opera on a computer (Windows, macOS, Linux). Safari and Firefox cannot use USB serial ports.

1. Connect the clock with a USB **data** cable (not a charge-only one).
2. Open <https://roman885-85.github.io/informer-clock-ua/flasher/> and press **“Connect and flash”**.
3. Choose the port: `CP2102`/`CP210x`, `CH340`, `USB Serial`.
4. When asked about erasing (**Erase device**):
   - leave it **unticked** — update, settings are kept;
   - **tick** it — clean install.
5. **Install** → wait for “Installation complete” (1–2 min). The clock restarts by itself.

No port visible — install the driver:
[CP210x](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) or
[CH340](https://www.wch-ic.com/downloads/CH341SER_EXE.html). Cannot connect — hold the **BOOT** button on the board for
2–3 s while “Connecting…” is shown.

## Method 2. Script (Windows, macOS, Linux)

On the [releases](https://github.com/roman885-85/informer-clock-ua/releases) page download
`informer-clock-ua-<version>-flash.zip` and unpack it. It contains the scripts and the firmware files (no internet
needed afterwards, except for downloading esptool the first time).

**Windows:** double-click `flash.bat` and choose `1` (update) or `2` (clean install). The script downloads the official
Espressif esptool into `%LOCALAPPDATA%\informer-clock-ua` and finds the port (asks if there are several).
From a command prompt:

```bat
flash.bat update COM3
flash.bat full COM3 -Baud 115200
```

**macOS, Linux:** `python3` is required. In a Terminal in the unpacked folder:

```sh
sh flash.sh                                  # asks: 1 — update, 2 — clean install
sh flash.sh update /dev/cu.usbserial-0001    # macOS
sh flash.sh full /dev/ttyUSB0                # Linux
BAUD=115200 sh flash.sh update               # slower, if writing fails
```

esptool installs itself into `~/.cache/informer-clock-ua/venv`. On Linux the user must be in the `dialout` group
(`sudo usermod -aG dialout $USER`, then log in again).

The same scripts are in the repository: `tools/flash/` (they take the files from `docs/firmware/`).

## Method 3. Flash Download Tool (Windows, Espressif)

1. Download the [Flash Download Tool](https://www.espressif.com/en/support/download/other-tools) and the firmware files:
   from the `…-flash.zip` archive (`firmware` folder) or from [docs/firmware](../firmware/).
2. Start it, choose **ChipType: ESP32**, **WorkMode: Develop**, **LoadMode: UART**.
3. Fill in the rows (tick the boxes on the left):

   | File | Address | For an update | For a clean install |
   |---|---|---|---|
   | `bootloader.bin` | `0x1000` | — | yes |
   | `partitions.bin` | `0x8000` | — | yes |
   | `boot_app0.bin` | `0xe000` | yes | yes |
   | `informer-clock-ua-esp32.bin` | `0x10000` | yes | yes |

   Or a single file: `informer-clock-ua-esp32-full.bin` at `0x0` (clean install only).
4. **SPI SPEED: 40MHz**, **SPI MODE: DIO**; if there is a FLASH SIZE field — **32Mbit** (4 MB). Tick **DoNotChgBin**.
5. Choose the COM port and speed (**460800** or **230400**). For a clean install press **ERASE** first, then **START**.
6. After “FINISH” press **EN**/**RST** on the board or reconnect USB.

## Method 4. esptool (any system)

```sh
python3 -m pip install esptool
# update (settings kept)
esptool.py --chip esp32 --port PORT --baud 230400 write_flash -z \
  0xe000 boot_app0.bin 0x10000 informer-clock-ua-esp32.bin
# clean install
esptool.py --chip esp32 --port PORT erase_flash
esptool.py --chip esp32 --port PORT --baud 230400 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect \
  0x1000 bootloader.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 informer-clock-ua-esp32.bin
```

esptool 5 spells the commands with a hyphen (`write-flash`, `erase-flash`); the old names still work with a warning.

## Method 5. From source (PlatformIO)

```sh
cd firmware
pio run -e clock -t upload          # USB; the flash is not erased — settings are kept
pio run -e clock -t upload --upload-port <clock-address>   # over Wi-Fi, see wifi-debug.md
```

Details — [building from source](building.md).

## Method 6. Over the network (once Informer Clock UA is installed)

Web interface → **“Update” → “Update”**. The clock compares its version with
[update/esp32.json](../../update/esp32.json) in this repository and offers an update only when the version there is
**newer**. The firmware file is downloaded from GitHub (HTTPS) at an address that includes the version tag, so a given
version number always means the same build. A clock running the original author's firmware does not see Informer
Clock UA updates and vice versa — switching is done over USB only.

## Checking the files

Checksums are in [docs/firmware/SHA256SUMS.txt](../firmware/SHA256SUMS.txt) (and in the archive):

```sh
shasum -a 256 -c SHA256SUMS.txt                                   # macOS, Linux (in the folder with the files)
certutil -hashfile informer-clock-ua-esp32.bin SHA256             # Windows
```

## After flashing

- Clean install: the clock creates the Wi-Fi access point **“Clock ESP”** (no password) → `http://192.168.4.1` → wizard.
- Update: the clock joins your network as before; the “Update” page shows the new version.
- You can go back to the author's firmware the same way over USB, using the files from the author's archive on
  [Thingiverse](https://www.thingiverse.com/thing:3924947) (the author's instructions are there too).
