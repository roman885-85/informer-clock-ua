# Informer Clock UA

[Українською](README.md) · [Project site](https://roman885-85.github.io/informer-clock-ua/?lang=en) ·
[Flash from the browser](https://roman885-85.github.io/informer-clock-ua/flasher/?lang=en) ·
[Manual](https://roman885-85.github.io/informer-clock-ua/manual/?lang=en) ·
[Documentation](docs/en/README.md) · [Releases](https://github.com/roman885-85/informer-clock-ua/releases)

**Independent firmware for the ESP32 + MAX7219 LED-matrix information clock**, based on the **Clock Informer 2.7.1**
firmware by **Semen_p**.

> **Pre-release 1.0.0.** Built and tested in simulators and in the browser, but not yet on a clock. Please report
> problems in [Issues](https://github.com/roman885-85/informer-clock-ua/issues).

## What it is and what it is for

A DIY desk clock: time (NTP, RTC), weather, news, currency and Bitcoin rates, sensor readings (temperature, humidity,
pressure, CO₂), WS2812B backlight, an hourly voice time announcement (DFPlayer), alarms, MQTT and Home Assistant.
The display is made of MAX7219 matrices in one or two floors; settings are made from a browser on your network.

The clock was created by Semen_p (Clock Informer): cases, schematic and firmware. Informer Clock UA is for those who
built a Clock Informer on ESP32 and want it to run reliably: the bugs of the 2.7.1 firmware are fixed here, new features
are added, and updates come from this repository. Documentation is in Ukrainian and English.

## How it differs from the original

- **A separate project with its own firmware.** Its own name and version numbers (1.x). Update checks and files,
  language packs, change history, SD-card sounds, cases, images and the manual come from this project, not from the
  author's server. This project's firmware never shows up in the original project and the author's firmware never shows
  up here; the clock offers updates only to a newer Informer Clock UA version.
- **Fixed** (details — [architecture.md](docs/en/architecture.md)): hangs without internet, “the clock is not visible
  on the network”, needless Wi-Fi drops, crashes caused by the core's DNS, minute-long HTTPS stalls without timeouts,
  service tasks waiting for news to finish scrolling, time jumps after restarts, summer time not switching, the hourly
  voice without the hour, alarms with the display off, the button not stopping the melody, the display staying on at
  night after a message, a stray dot at 9:59→10:00, a stale cached interface, Wi-Fi passwords in the console.
- **New:** start-up status on the display (icon and text, also on two floors), smooth light-sensor brightness, night
  quiet, “Auto” summer time by EU rules, time after a restart without internet, Wi-Fi debugging (log and firmware
  upload), crash cause in the event log, a main-loop watchdog.
- **Same hardware and settings:** the wiring and the partition table are as in the original — switching from the author's
  firmware keeps Wi-Fi and all settings. The web interface is the author's interface with additions on existing pages.
- Only **ESP32** (classic, 4 MB) is supported; ESP8266 and ESP32-C3/C6/S3 are not.

## Original author

**Clock Informer** is a project by **Semen_p**: [Thingiverse page](https://www.thingiverse.com/thing:3924947) (cases,
schematic, description), [goldart.ee](https://www.goldart.ee/), support the author at
[ko-fi.com/clockinformer](https://ko-fi.com/clockinformer). Informer Clock UA is not affiliated with or supported by the
author: please ask about this firmware here.

## Quick start

1. Build the clock after the [wiring guide](docs/en/wiring.md) (or take an already built Clock Informer on ESP32).
2. Flash it from the browser: <https://roman885-85.github.io/informer-clock-ua/flasher/> (Chrome, Edge, Opera; USB cable).
   Other ways — [flashing](docs/en/flashing.md).
3. New clock: Wi-Fi **“Clock ESP”** → `http://192.168.4.1` → setup wizard. Switching from the author's firmware without
   erasing — the clock is on your network right away with all its settings.
4. After that, updates are in the web interface: “Update” → “Update”.

## Documentation

| | |
|---|---|
| [User manual](https://roman885-85.github.io/informer-clock-ua/manual/?lang=en) | using the clock |
| [Flashing](docs/en/flashing.md) | browser, scripts, Flash Download Tool, esptool, PlatformIO, over the air |
| [Wiring](docs/en/wiring.md) | diagram, pins, power, sensors, sound |
| [Building](docs/en/building.md) | PlatformIO, web interface, making a release, simulators |
| [How the firmware works](docs/en/architecture.md) | code origin, modules, fixes, new features |
| [Wi-Fi debugging](docs/en/wifi-debug.md) | network log, OTA, crashes |
| [Troubleshooting](docs/en/troubleshooting.md) | common problems |

## Repository contents

```
firmware/     firmware source (PlatformIO, Arduino-ESP32 1.0.4) with explanatory comments
web/          the author's original web interface and the patch script (webui_patch.py)
update/       latest version description for clocks and the change history
lang/         interface and display language packs
docs/         project site: flasher, manual, firmware files, uk/en documentation
tools/        flashing scripts (Windows, macOS, Linux), release script, simulators and checks
```

## License

[CC BY-NC 4.0](LICENSE), same as the original: you may copy and modify it with attribution, not for commercial purposes.
Attribution and licenses of the components (MD_Parola and MD_MAX72XX — LGPL-2.1, etc.) — [NOTICE.md](NOTICE.md).
