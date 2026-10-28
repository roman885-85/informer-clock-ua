# Wi-Fi debugging

[Українською](../uk/wifi-debug.md) · [Documentation index](README.md)

To find out what is going on with the clock you do not need to connect it to a computer with a cable: the running log
and firmware uploads work over Wi-Fi. The feature is switched on and off in the web interface.

## Switching it on

Clock menu (the clock-icon button at the bottom left; on a phone — **⋮** at the bottom right) → **System info & logs**
→ the **“Wi-Fi debug”** switch. A hint with the clock's address appears under it. The setting is saved (key `wdbg`) and
stays after a restart. When it is off, the ports are closed and no log lines are collected.

![The switch on the System page](../images/webui-system-wdbg.png)

## Running log (TCP port 23)

```sh
nc <clock-address> 23                    # macOS, Linux
nc <clock-address> 23 >> clock.log       # write to a file (e.g. overnight)
```

Windows: [PuTTY](https://www.putty.org/) → Connection type **Raw**, Host — the clock address, Port **23**; or
`telnet <address> 23` (the Telnet Client feature).

What is in the log:

- on connect — a header: firmware name and version, uptime, free memory, reset reason, the last crash; then the last
  ~8 KB of the log;
- debug lines (the same as in the web interface live console): Wi-Fi, NTP, weather, news, MQTT, display;
- Arduino core (`log_e`) and ESP-IDF errors.

Up to 2 connections at a time. The log does not take the web interface's WebSocket connection (the clock has only one).

## Firmware upload over Wi-Fi (ArduinoOTA, port 3232)

For developers — write your own build without a cable:

```sh
cd firmware
pio run -e clock -t upload --upload-port <clock-address>
# if a web interface user name and password are set in “Settings”:
PLATFORMIO_UPLOAD_FLAGS="--auth=<password>" pio run -e clock -t upload --upload-port <clock-address>
```

Without PlatformIO — `espota.py` from the Arduino-ESP32 core:
`python3 espota.py -i <address> -p 3232 -a <password> -f firmware.bin`.

While writing, the display shows “Updating firmware”. With the espota protocol the clock connects back to the computer,
so the computer's firewall must allow incoming connections for Python/PlatformIO.

## Event log and crashes

Regardless of the switch, the clock keeps an **event log** (clock menu → System → Event Log): starts, restart reasons,
Wi-Fi, updates. If the clock crashed and restarted, the log gets a line like

```
[BOOT] crash: exception LoadProhibited (28) PC=0x400d1234 addr=0x00000000 bt: 0x400d1234 0x400d5678 …
```

The addresses are turned into source file and line with the `.elf` file of the same version (it is among the GitHub
release files):

```sh
~/.platformio/packages/toolchain-xtensa32/bin/xtensa-esp32-elf-addr2line -pfiaC \
  -e informer-clock-ua-esp32-1.0.0.elf 0x400d1234 0x400d5678
```

If `loop()` hangs for more than 3 min, the watchdog restarts the clock and writes `[BOOT] loop guard: loop() stuck > 180 s`.

## Security

While the switch is on, port 23 is open to the whole local network (the log contains no Wi-Fi passwords — they are
masked), and firmware upload over Wi-Fi is protected only by the web interface password (none if it is not set). Turn
debugging off when you do not need it.
