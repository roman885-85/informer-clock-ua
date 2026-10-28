# Informer Clock UA documentation

[Українською](../uk/README.md) · [Project home](../../README.en.md)

Informer Clock UA is independent firmware for the ESP32 + MAX7219 LED-matrix information clock, based on the
Clock Informer 2.7.1 firmware by [Semen_p](https://www.thingiverse.com/thing:3924947).

| Document | Contents |
|---|---|
| [User manual](https://roman885-85.github.io/informer-clock-ua/manual/?lang=en) | first start, web interface pages, brightness, night, time, updates, the button |
| [Flashing](flashing.md) | every way to flash the clock: browser, scripts, Flash Download Tool, esptool, PlatformIO, over the air |
| [Wiring](wiring.md) | diagram, pin table, power, two floors of matrices, sensors, sound, case |
| [Building from source](building.md) | PlatformIO, the web interface, making a release, simulators and checks |
| [How the firmware works](architecture.md) | where the code comes from, module map, all fixes and new features, independence from the author's server |
| [Wi-Fi debugging](wifi-debug.md) | network log, firmware upload over Wi-Fi, event log and crashes |
| [Troubleshooting](troubleshooting.md) | common problems and what to do, how to report a bug |

The 1.0.0 pre-release has not been tested on a clock yet — please report problems in
[Issues](https://github.com/roman885-85/informer-clock-ua/issues).
