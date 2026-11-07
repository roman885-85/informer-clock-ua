# Troubleshooting

[Українською](../uk/troubleshooting.md) · [Documentation index](README.md)

## Flashing

| What you see | What to do |
|---|---|
| The browser flasher shows no port | Chrome/Edge/Opera on a computer (not Safari, not Firefox, not a phone); a data cable; the [CP210x](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) or [CH340](https://www.wch-ic.com/downloads/CH341SER_EXE.html) driver |
| “Failed to connect”, “Timed out waiting for packet header” | hold **BOOT** for 2–3 s while connecting; another USB port (no hub); a lower speed (`BAUD=115200`) |
| Linux: “Permission denied” on `/dev/ttyUSB0` | `sudo usermod -aG dialout $USER` and log in again |
| After flashing the clock restarts in a loop | clean install (with erase); check the 5 V 2 A power supply |
| The clock restarts by itself every few hours; the event log shows “loop guard: loop() stuck > 180 s”, each time with a different stage | a false alarm of the loop guard before 1.1.4 (the loop was running) — please update; since 1.1.4 the event log shows “[GUARD] loop() running, false alarm skipped” instead of a restart |
| I want to go back to the author's firmware | over USB, with the author's files from [Thingiverse](https://www.thingiverse.com/thing:3924947) |

## Network

| What you see | What to do |
|---|---|
| The clock is not on the network, the “Clock ESP” access point is visible | it could not join your network: join “Clock ESP”, open `http://192.168.4.1`, choose the network again (2.4 GHz — the ESP32 does not do 5 GHz) |
| Neither the network nor “Clock ESP” | hold the button ≈5 s — the clock switches between access point mode and joining the network |
| The address is on the display but the page does not open | the phone/computer must be on the same network (not a guest one); type `http://` and the address by hand |
| “No internet” | the clock checks 8.8.8.8 and 1.1.1.1 (TCP 53), then `http://www.msftconnecttest.com`; if the router blocks external DNS — allow it for the clock |
| No update is offered | an update appears only for a newer version of this project; internet is required; GitHub cache — up to 5 min after a release |
| After ~1.5 h of work “no internet”, although the web interface opens | a 1.0.0 bug: the internet check did not free its sockets, and after 8–10 checks no outbound connection could be opened — fixed in 1.1.0; until you update, a restart helps (update right after it) |
| After a router or power outage the clock stays off the network for hours (the event log shows “AP restored” every ~3.5 min) | before 1.1.1: power the clock off and on; since 1.1.1 it restarts by itself — after ~10 min, and less often (up to once an hour) while the router is gone |
| The clock is off the network, and every 6 min the event log shows “Forced re-association (… router silent …)” | before 1.1.2: power the clock off and on; since 1.1.2 it restarts by itself (the log shows “Restart: Wi-Fi events not delivered…” or “Restart: status says connected, driver is not…”); if it keeps happening, please send the event log to Issues: the records show where event handling got stuck |
| Updates break off (“Stream Read Timeout”, “Verify Bin Header Failed”), the web interface is slow, the clock drops off the network | the clock stands right next to the router: its receiver is overloaded (up to 67 % lost packets). Move it 1–2 m away — at 1.5–2 m there are no losses; the signal strength is in the event log (“RSSI=… dBm”, −40…−70 is fine) |
| In Home Assistant the clock entities become “unavailable” for a few seconds every few minutes | before 1.1.4 the clock dropped the MQTT connection at every time sync — please update |

## Display

| What you see | What to do |
|---|---|
| Garbage, mirrored or mixed-up characters | module type — FC16; DIN/CLK/CS on GPIO23/18/5; shorter wires, common GND; number of floors and matrices — “Display” |
| The display flickers, the clock restarts at high brightness | weak power: a 5 V 2 A supply, a 1000 µF capacitor, the display powered from the supply, not through the board |
| The display does not turn off at night / on in the morning | “Display” → shut down the display at night, check the hours; time and zone — “Time” page |
| Brightness “jumps” | photoresistor and 10 kΩ on GPIO34; “Brightness type”, “Brightness threshold” on the “Display” page; since 1.1.0 changes are smooth — the whole range in 2 s |
| Before or after the scrolling line the display flashes at full brightness for a few seconds (the halves of the big clock face differ in brightness) | fixed in 1.1.3 — please update; on earlier versions pick another effect: “Clock face” → “Effect” (not “Fade” and not random) |
| A news item ends with “... [2072 chars]” | that is how GNews gives the article text on the free plan — only the beginning; since 1.1.3 the clock removes the tail and ends the news at the last complete sentence |
| The display went dark and does not come back, although the clock is online | it was switched off by an MQTT command — for example the “Display” light or a voice command in Home Assistant: switch it on there; since 1.1.4 the event log shows when — “[MQTT] Display off by command” |

## Time

| What you see | What to do |
|---|---|
| The time is off by an hour | “Time” → “Summer time”: “Auto” for Europe; check the time zone |
| The time is inaccurate after a restart without internet | expected: until the first sync the system time is shown (drifts from minutes to tens of minutes per day of operation); with a DS3231 — accurate |

## Sound

| What you see | What to do |
|---|---|
| The voice is silent | microSD card FAT32 with the folders `01`…`16` from the “Sounds for SD card” archive in the root; DFPlayer: TX→GPIO16, RX←GPIO17 (through 1 kΩ), 5 V supply; DFPlayer selected on the “Sound” page |
| The voice does not work together with an MH-Z19B sensor | both use the same UART — choose one |
| The button does not stop the alarm | a short press only snoozes it (≈5 min); to stop, hold the button ≥2 s |

## Reporting a bug

[Issues](https://github.com/roman885-85/informer-clock-ua/issues) → New issue. Please include:

1. the firmware version (“Update” page) and how the clock is built (floors, matrices, sensors, DFPlayer);
2. what you did, what happened and what you expected;
3. the **event log** (clock menu → System → Event Log) as text;
4. if possible — the [network log](wifi-debug.md) around the problem.

Remove network names, addresses and keys from the logs before posting if you do not want to share them.
