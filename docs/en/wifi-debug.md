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
On macOS a Python that is not the system one (including PlatformIO's) gets “No response from the ESP” without the
“Local Network” permission — then run `espota.py` with the system `/usr/bin/python3` and give the computer's address:
`-I <Mac address>`.

While writing, the clock stops its services (MQTT, LED strip, sound, smooth brightness); the web interface keeps working.

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
  -e informer-clock-ua-esp32-1.1.5.elf 0x400d1234 0x400d5678
```

If `loop()` hangs for more than 3 min, the watchdog restarts the clock and writes `[BOOT] loop guard: loop() stuck > 180 s`.

Before 1.1.4 the guard sometimes fired falsely: about once every few hours it restarted the clock although the loop was
running (see [architecture](architecture.md)). Since 1.1.4 it rejects such an alarm and writes
`[GUARD] loop() running, false alarm skipped (N since start)` to the event log — without a restart. Test (only with
Wi-Fi debugging on; not saved to the settings): `http://<address>/post?wdbg_guard_race=N` — N guard checks (up to 60,
one a second) with a deliberately long window between reading the time and the loop's stamp; the log gets N such
lines, and the clock keeps running.

Since 1.1.3, before such a restart (and before a restart by the Wi-Fi event watchdog, see below) the clock records
the **task state**, and after the start the event log shows:

```
[BOOT] stuck tasks (loop guard): up 246 s, stage 6, Wi-Fi status 3, heap 158060, events 2, last 7 243 s ago
[BOOT] stuck loopTask R (ipc) free 2328: 4000d033 400828a3 400efc00 400e4fe3 401442a9 4008a1dd
[BOOT] stuck tiT B free 1684: 400837a6 40089ff1 401c3b8d 401b431e 401accc4 4008a1dd
…
```

The first line says who restarted the clock (`loop guard` or `Wi-Fi watchdog` — the Wi-Fi event watchdog), how long it
had been running, the loop stage, `WiFi.status()`, free memory and the Wi-Fi event handling state (as in the section
below). Then a line per task: name, state (`R` — running, `r` — ready, `B` — blocked, `S` — suspended; `(ipc)` — the
frame was taken from the other core), free stack in bytes and up to 8 call-chain addresses — decode them with the same
`addr2line` and the `.elf` of this version. Please send such a log to Issues: it shows where exactly the loop stopped.
Test (only with Wi-Fi debugging on; not saved to the settings): `http://<address>/post?wdbg_loop_spin=N` — `loop()`
spins for N s (up to 600); after 3 min — `Software reset` and a snapshot with `loopTask` in `fx_loop_spin_tick` (or in
the `millis()` it calls: then the next address is the call of `fx_loop_spin_tick` in `loop()`).

## Sockets and signal strength (since 1.1.0)

- `http://<address>/content?wifi_rssi=` — router signal strength, dBm (−40…−70 is fine; closer than −30 — the clock
  stands too close to the router). The event log has it in every “Connected” line.
- `http://<address>/content?sock_diag=` — lwIP socket census (once a minute): `used` — open, `free` — free slots
  (counted only with Wi-Fi debugging on), `lost` — neither open nor free; then for each open socket — type (`T` TCP,
  `U` UDP), ports, address, seconds open, task/loop stage and the call chain `@…` (addresses — with the same
  `addr2line`). With Wi-Fi debugging on, the same `[SOCK] …` line goes to the log — when something changes or every
  10 min.
- If sockets ever run out (no socket can be created three checks in a row), the clock writes
  `[NET ] Restart: no sockets…` and the `[SOCK] …` census to the event log and restarts.

## Simulating a router outage (since 1.1.1)

- `http://<address>/post?wdbg_wifi_off=N` — for N seconds (up to 3600) the clock “does not see” the router: it
  disconnects and keeps reconnecting to a non-existent network. Works only with Wi-Fi debugging on; it is not saved to
  the settings, and a restart ends it. The event log shows `[WIFI] Test: router outage simulated for N s` and
  `[WIFI] Test: simulated outage over`.
- A short simulation (2 min): the clock must come back by itself within seconds after it ends, without a restart. A
  long one (30 min): after ~10 min — `[WIFI] Restart: router unreachable (cycle 3, …)`, a restart and `Connected`
  right away.

## Wi-Fi event handling state (since 1.1.2)

- The `[NET ] Internet offline — ping failed (…)` and `[NET ] Forced re-association (…)` records end with the Wi-Fi
  state: `events N` — how many Wi-Fi events the core's event task (`network_event`) has handled; `last E X s ago` —
  the last event (a `system_event_id_t` number: 1 — scan done, 2/3 — station start/stop, 5 — disconnected, 7 — got an
  address) and when; `handler busy: event E step S for X s` — the handler is stuck on event E, step S (1–2 —
  disconnect, 3–5 — got address, 6 — station start, 99 — simulation) for X s; `task …` — the event task state and its
  stack headroom; `driver: associated / not associated` — whether the Wi-Fi driver is associated with the router;
  `status N` — `WiFi.status()` (3 — “connected”, 6 — “disconnected”, 255 — Wi-Fi off); `heap` — free memory.
- The clock restarts by itself when it sees that event handling has stopped: `[WIFI] Restart: Wi-Fi events not
  delivered after re-association (…)` (no events for 8 s after a forced re-association), `[WIFI] Restart: status says
  connected, driver is not (…)` (for a minute in a row), `[NET ] Restart: router silent, no reconnect since last
  re-association (…)` (the fallback rule). Please send such a log to Issues — it shows where the handling got stuck.
- Simulation (only with Wi-Fi debugging on; not saved to the settings): `http://<address>/post?wdbg_evt_reassoc=N` —
  event handling stops for N s (up to 3600) and the clock re-associates; within ~8 s you should see `Restart: Wi-Fi
  events not delivered…` and `Connected` right away. `http://<address>/post?wdbg_evt_block=N` — the same with a link
  drop; within ~1 min — `Restart: status says connected, driver is not…`.

## Security

While the switch is on, port 23 is open to the whole local network (the log contains no Wi-Fi passwords — they are
masked), and firmware upload over Wi-Fi is protected only by the web interface password (none if it is not set). Turn
debugging off when you do not need it.
