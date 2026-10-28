# NOTICE — авторство й ліцензії / attribution and licenses

## Українською

**Informer Clock UA** — адаптація проєкту **«Smart Internet Clock Informer»** автора **Semen_p**
(<https://www.thingiverse.com/thing:3924947>, <https://www.goldart.ee/>), що поширюється за ліцензією
[Creative Commons Attribution-NonCommercial 4.0 International](https://creativecommons.org/licenses/by-nc/4.0/)
(CC BY-NC 4.0). Informer Clock UA поширюється за тією самою ліцензією (повний текст — [LICENSE](LICENSE)): можна
копіювати, поширювати й змінювати із зазначенням авторства, **не з комерційною метою**.

Informer Clock UA — незалежний проєкт: автор оригіналу його не створював, не перевіряв і не підтримує. Що змінено
відносно оригіналу — [CHANGELOG.md](CHANGELOG.md), [docs/uk/architecture.md](docs/uk/architecture.md) і історія git від
тегу `reconstructed-2.7.1`.

| Частина | Походження | Ліцензія |
|---|---|---|
| `firmware/src` | відтворено з прошивки Clock Informer 2.7.1 (Semen_p) + зміни цього проєкту | CC BY-NC 4.0 |
| шрифти й спрайти табло (`firmware/src/fonts_2_7_1.h`, `sprites_2_7_1.h`) | Semen_p, з прошивки 2.7.1 без змін | CC BY-NC 4.0 |
| `web/original/index.html.gz` | веб-інтерфейс Clock Informer 2.7.1 (Semen_p) без змін | CC BY-NC 4.0 |
| `firmware/src/web_assets.h`, `web/webui_patch.py` | змінений інтерфейс автора; правки цього проєкту | CC BY-NC 4.0 |
| `lang/` | мовні пакети Clock Informer (Semen_p) без змін | CC BY-NC 4.0 |
| `docs/images/original-esp32-schematic.png`, `sounds_sd.jpg`, `3d_printing.jpg` | Semen_p | CC BY-NC 4.0 |
| випуск `files`: `sounds_sd_card.zip`, `clock_cases.zip` | Semen_p, без змін | CC BY-NC 4.0 |
| `firmware/lib/MD_Parola` | MD_Parola 3.5.2, majicDesigns; змінено — [FORK.md](firmware/lib/MD_Parola/FORK.md) (ефекти автора Clock Informer) | LGPL-2.1 |
| `firmware/lib/MD_MAX72XX` | MD_MAX72XX 3.2.3, majicDesigns, без змін | LGPL-2.1 |
| `firmware/lib/MHZ19_uart` | за [nara256/mhz19_uart](https://github.com/nara256/mhz19_uart), відтворено з машинного коду | MIT |
| `firmware/lib/Base64` | за [adamvr/arduino-base64](https://github.com/adamvr/arduino-base64) (Adam Rudd), відтворено з машинного коду | MIT |
| `firmware/lib/MD5` | API як у tzikis/ArduinoMD5, реалізація власна (MD5 з ПЗУ ESP32) | CC BY-NC 4.0 |
| нове в цьому проєкті: модулі `firmware/src` з позначкою «нове», `tools/`, `docs/` | Informer Clock UA | CC BY-NC 4.0 |

Під час збирання PlatformIO завантажує (не входять у репозиторій): ядро Arduino-ESP32 1.0.4 (LGPL-2.1) з ESP-IDF 3.3
(Apache-2.0) і бібліотеки з `lib_deps` у `firmware/platformio.ini` — кожна за своєю ліцензією (ESPAsyncWebServer —
LGPL-3.0, AsyncTCP — LGPL-3.0, AsyncMqttClient — MIT, ArduinoJson — MIT, FastLED — MIT, DFRobotDFPlayerMini —
GNU LGPL, OneWire, DallasTemperature, DHTesp, бібліотеки Adafruit і SparkFun, RTClib, Time, YoutubeApi та інші).
Файли прошивки містять ці компоненти; повний вихідний код прошивки — у цьому репозиторії, тож її можна перезібрати
з іншими версіями бібліотек. Прошивальник у браузері використовує [ESP Web Tools](https://github.com/esphome/esp-web-tools)
(Apache-2.0), скрипти прошивки — [esptool](https://github.com/espressif/esptool) (GPL-2.0); обидва завантажуються з їхніх
джерел і не входять у репозиторій.

## English

**Informer Clock UA** is an adaptation of **“Smart Internet Clock Informer”** by **Semen_p**
(<https://www.thingiverse.com/thing:3924947>, <https://www.goldart.ee/>), licensed under
[Creative Commons Attribution-NonCommercial 4.0 International](https://creativecommons.org/licenses/by-nc/4.0/)
(CC BY-NC 4.0). Informer Clock UA is distributed under the same license (full text — [LICENSE](LICENSE)): you may copy,
share and adapt it with attribution, **not for commercial purposes**.

Informer Clock UA is an independent project: the original author did not create, review or endorse it. What was
changed compared to the original — [CHANGELOG.md](CHANGELOG.md), [docs/en/architecture.md](docs/en/architecture.md) and
the git history from the `reconstructed-2.7.1` tag.

The table above lists the origin and license of every part (CC BY-NC 4.0 for the author's material and this project's
work; LGPL-2.1 for MD_Parola — modified, see [FORK.md](firmware/lib/MD_Parola/FORK.md) — and MD_MAX72XX; MIT for the
reconstructed MHZ19_uart and Base64). Libraries downloaded by PlatformIO at build time (Arduino-ESP32 core, ESP-IDF and
the `lib_deps` in `firmware/platformio.ini`) keep their own licenses; the firmware binaries contain them, and the full
firmware source is in this repository so it can be rebuilt with other library versions. The browser flasher uses
ESP Web Tools (Apache-2.0) and the flashing scripts use esptool (GPL-2.0); both are fetched from their sources and are
not part of this repository.
