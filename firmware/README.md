# firmware — прошивка / firmware source

**UA.** Вихідний код прошивки Informer Clock UA для ESP32 (PlatformIO, Arduino-ESP32 1.0.4).

```sh
pio run -e clock                 # зібрати
pio run -e clock -t upload       # записати через USB (налаштування зберігаються)
```

- `src/` — код; кожен файл починається з опису (українською й `// EN:`). Нові модулі позначено «нове, не з оригіналу»,
  виправлення в перенесеному коді — «Виправлено:». Карта модулів — [docs/uk/architecture.md](../docs/uk/architecture.md).
- `lib/` — бібліотеки, яких немає в реєстрі PlatformIO або змінені: MD_Parola 3.5.2 з ефектами автора
  ([FORK.md](lib/MD_Parola/FORK.md)), MD_MAX72XX 3.2.3, MHZ19_uart, Base64, MD5.
- `partitions_clock.csv` — таблиця розділів, як у прошивці автора.
- `platformio.ini` — закріплені версії платформи, ядра, компілятора й бібліотек; прапорці `--wrap`.

Збирання, випуск, перевірки — [docs/uk/building.md](../docs/uk/building.md).

**EN.** Informer Clock UA firmware source for ESP32 (PlatformIO, Arduino-ESP32 1.0.4). `pio run -e clock` builds,
`pio run -e clock -t upload` writes over USB (settings are kept). `src/` — code (each file starts with a Ukrainian
description and an `// EN:` line; new modules are marked “нове, не з оригіналу”, fixes in ported code “Виправлено:”);
`lib/` — modified or unregistered libraries; `partitions_clock.csv` — the author's partition table; `platformio.ini` —
pinned versions and `--wrap` flags. See [docs/en/architecture.md](../docs/en/architecture.md) and
[docs/en/building.md](../docs/en/building.md).
