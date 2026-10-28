# Збирання з вихідного коду

[English](../en/building.md) · [Зміст документації](README.md)

## Що потрібно

- `git`, `python3` (3.8 або новіший);
- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/) (`python3 -m pip install platformio`)
  або VS Code з розширенням PlatformIO IDE;
- для випуску — `esptool` (`python3 -m pip install esptool`) і `zip`.

## Прошивка

```sh
git clone https://github.com/roman885-85/informer-clock-ua.git
cd informer-clock-ua/firmware
pio run -e clock                 # зібрати: firmware/.pio/build/clock/firmware.bin
pio run -e clock -t upload       # зібрати й записати через USB (налаштування зберігаються)
pio device monitor -b 115200     # журнал у USB-порт
```

Перше збирання завантажить точно закріплені версії (`firmware/platformio.ini`): платформа `espressif32@3.0.0`,
ядро Arduino-ESP32 **1.0.4** (`framework-arduinoespressif32 @ 3.10004.210126`, ESP-IDF 3.3), компілятор
`toolchain-xtensa32 @ 2.50200.97` і бібліотеки з `lib_deps` — ті самі, що в прошивці автора 2.7.1. Новіші ядра
не підходять: код відтворено під поведінку саме цього ядра.

Середовища:

| Середовище | Що це |
|---|---|
| `clock` | звичайна прошивка |
| `diag` | те саме + налагоджувальні рядки веб-консолі дублюються в USB-порт (`-DCOPY_DIAG`) |

Прапорці `-Wl,--wrap=…` у `platformio.ini` підміняють кілька функцій ядра 1.0.4 (DNS, з'єднання без тайм-ауту,
встановлення часу, обробники аварій, журнал ядра) — пояснення в [architecture.md](architecture.md).

## Веб-інтерфейс

Веб-інтерфейс — це оригінальний React-інтерфейс автора (`web/original/index.html.gz`, зібраний і стиснутий), до якого
`web/webui_patch.py` застосовує **точні текстові правки**: нові поля на існуючих сторінках, адреси цього проєкту
замість сервера автора, назву проєкту, історію змін з `update/changelog.json`.

```sh
python3 web/webui_patch.py
```

- кожна правка має збігтися рівно стільки разів, скільки вказано, інакше скрипт зупиняється з назвою правки;
- скрипт зупиняється, якщо в інтерфейсі лишилась хоч одна адреса сервера автора;
- результат: `firmware/src/web_assets.h` (масив `INDEX_HTML_GZ` і мітка `INDEX_HTML_ETAG` за вмістом — браузер не
  покаже старий інтерфейс із кешу) і `web/build/index.html(.gz)` для перегляду.

Запускати після зміни правок або `update/changelog.json`, потім перезібрати прошивку.

### Перегляд інтерфейсу без годинника

```sh
python3 web/webui_patch.py
python3 tools/sim/mockclock.py --port 8791      # http://localhost:8791
```

`mockclock.py` віддає зібраний інтерфейс і відповідає на запити як годинник із заводськими налаштуваннями
(`tools/sim/mock-settings.json`); збереження лише друкуються в терміналі.

## Випуск нової версії

1. `firmware/src/version.h` — нове `FX_VERSION` (формат `X.Y.Z`; годинник пропонує оновлення, лише коли номер більший).
2. `update/changelog.json` — новий запис нагорі (`v`, `date`, `items` з `t`: `added` / `improved` / `fixed` /
   `removed`; текст українською й англійською через « / »). Те саме — у `CHANGELOG.md`.
3. Зібрати:
   ```sh
   sh tools/build-release.sh
   ```
   Скрипт: правки веб-інтерфейсу → збирання PlatformIO → `docs/firmware/` (bootloader, partitions, boot_app0,
   програма, повний образ, `SHA256SUMS.txt`) → `update/esp32.json` (версія, розмір, MD5, SHA-256, адреса файла за тегом
   `vX.Y.Z`) → `docs/flasher/manifest.json` → архів `release/informer-clock-ua-X.Y.Z-flash.zip`.
   Змінні: `PIO=…` (шлях до pio), `ESPTOOL=…`, `PLATFORMIO_CORE_DIR=…`.
4. Перевірити на годиннику.
5. Закомітити, поставити тег і відправити **разом** (адреса файла в `esp32.json` веде на тег):
   ```sh
   git commit -am "Випуск X.Y.Z"
   git tag vX.Y.Z
   git push origin main vX.Y.Z
   ```
6. Створити випуск на GitHub (`gh release create vX.Y.Z release/informer-clock-ua-X.Y.Z-flash.zip docs/firmware/*.bin
   docs/firmware/SHA256SUMS.txt`). Годинники побачать оновлення, щойно `update/esp32.json` з'явиться на `main`
   (кеш GitHub — до 5 хв).

## Перевірки й імітатори (`tools/sim`)

Невеликі програми, що збирають **справжній** код прошивки на комп'ютері з заглушками ядра й перевіряють поведінку:

| Що | Запуск | Що перевіряє |
|---|---|---|
| `brightsim` | `sh tools/sim/brightsim/build.sh` | плавну яскравість (`bright.cpp`, `photo.cpp`) |
| `voicesim` | `sh tools/sim/voicesim/build.sh` | автомат голосу щогодини (`sound.cpp`) |
| `slidesim` | `sh tools/sim/slidesim/run.sh` | «лишні точки» при зміні години на циферблаті зі зсувом цифр |
| `wdbgsim` | `sh tools/sim/wdbgsim/build.sh`, потім запустити надрукований файл і `nc localhost 2323` | сетевий журнал (`wdbg.cpp`, порт 2323 замість 23) |
| `dsttest` | `sh tools/sim/dsttest/build.sh` (після `pio run`: потрібна TimeLib) | дати переходу на літній час ЄС 2024–2031 |
| `masktest` | `sh tools/sim/masktest/build.sh` | приховування паролів Wi-Fi у консолі (`ws.cpp`) |
| `statusprev` | `python3 tools/sim/statusprev/preview.py 1 out.gif --rows 2` | як виглядає стан запуску на табло (GIF) |
| `status_icons.py` | `python3 tools/sim/status_icons.py --png icons.png` | кадри значків стану запуску |
| `mockclock.py` | див. вище | веб-інтерфейс без годинника |

Інструменти документації: `tools/docs/wiring_svg.py` (схема підключення), `tools/build-release.sh` (випуск),
`tools/flash/` (скрипти прошивки).

## Структура репозиторію

```
firmware/            прошивка (PlatformIO): src/ — код, lib/ — бібліотеки, partitions_clock.csv
web/original/        оригінальний веб-інтерфейс автора (зібраний React, gzip)
web/webui_patch.py   правки інтерфейсу → firmware/src/web_assets.h
update/              esp32.json (опис останньої версії для годинників), changelog.json (історія змін для інтерфейсу)
lang/                мовні пакети інтерфейсу (*.json) і табло (*.led) — з оригінального проєкту
docs/                сайт проєкту (GitHub Pages): прошивальник, інструкція, файли прошивки, документація
tools/               скрипти прошивки, випуску, імітатори
```
