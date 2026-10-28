# Прошивка годинника

[English](../en/flashing.md) · [Зміст документації](README.md)

## Що підтримується

- Плата: **ESP32** (класичний ESP32-WROOM-32, напр. ESP32 DevKit V1 на 30 або 38 виводів), флеш-пам'ять **4 МБ**.
- ESP8266, ESP32-C3, ESP32-C6, ESP32-S3 цей проєкт **не підтримує** (оригінальний Clock Informer їх підтримує — див.
  сторінку автора).
- Годинник зібраний за схемою автора Clock Informer: [підключення](wiring.md).

## Оновлення чи чиста установка

| | Оновлення | Чиста установка |
|---|---|---|
| Що записується | лише програма (і позначка розділу завантаження) | усе: завантажувач, таблиця розділів, програма |
| Налаштування, Wi-Fi, ключі, мовні пакети | **зберігаються** | стираються |
| Коли | перехід із прошивки автора Clock Informer, нова версія Informer Clock UA | новий годинник, «щось зовсім не так» |

Таблиця розділів Informer Clock UA така сама, як у прошивці автора (nvs, otadata, app0, app1, spiffs), тому перехід
із Clock Informer — це звичайне оновлення: налаштування й Wi-Fi лишаються.

**Перший раз** Informer Clock UA записується через USB (будь-яким способом нижче). Далі оновлення приходять по мережі —
«Оновлення» → «Оновити» у веб-інтерфейсі.

## Спосіб 1. З браузера (найпростіше)

Потрібні Chrome, Edge або Opera на комп'ютері (Windows, macOS, Linux). Safari й Firefox не вміють працювати з USB-портом.

1. Під'єднайте годинник USB-кабелем **з передачею даних** (не «лише для заряджання»).
2. Відкрийте <https://roman885-85.github.io/informer-clock-ua/flasher/> і натисніть **«Під'єднати й прошити»**.
3. Виберіть порт: `CP2102`/`CP210x`, `CH340`, `USB Serial`.
4. На запитання про стирання (**Erase device**):
   - позначку **не ставити** — оновлення, налаштування зберігаються;
   - **поставити** — чиста установка.
5. **Install** → зачекайте «Installation complete» (1–2 хв). Годинник перезапуститься сам.

Порту не видно — встановіть драйвер:
[CP210x](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) або
[CH340](https://www.wch-ic.com/downloads/CH341SER_EXE.html). Не під'єднується — під час «Connecting…» тримайте кнопку
**BOOT** на платі 2–3 с.

## Спосіб 2. Скриптом (Windows, macOS, Linux)

На сторінці [випусків](https://github.com/roman885-85/informer-clock-ua/releases) завантажте архів
`informer-clock-ua-<версія>-flash.zip` і розпакуйте. У ньому — скрипти й файли прошивки (інтернет далі не потрібен,
крім першого завантаження esptool).

**Windows:** двічі клацніть `flash.bat` і виберіть `1` (оновлення) або `2` (чиста установка). Скрипт сам завантажить
офіційний esptool від Espressif у `%LOCALAPPDATA%\informer-clock-ua` і знайде порт (якщо портів кілька — запитає).
З командного рядка:

```bat
flash.bat update COM3
flash.bat full COM3 -Baud 115200
```

**macOS, Linux:** потрібен `python3`. У Терміналі в розпакованій теці:

```sh
sh flash.sh                                  # запитає: 1 — оновлення, 2 — чиста установка
sh flash.sh update /dev/cu.usbserial-0001    # macOS
sh flash.sh full /dev/ttyUSB0                # Linux
BAUD=115200 sh flash.sh update               # повільніше, якщо запис зривається
```

esptool ставиться сам у `~/.cache/informer-clock-ua/venv`. У Linux користувач має бути в групі `dialout`
(`sudo usermod -aG dialout $USER`, потім перелогінитись).

Ті самі скрипти лежать у репозиторії: `tools/flash/` (беруть файли з `docs/firmware/`).

## Спосіб 3. Flash Download Tool (Windows, Espressif)

1. Завантажте [Flash Download Tool](https://www.espressif.com/en/support/download/other-tools) і файли прошивки:
   з архіву `…-flash.zip` (тека `firmware`) або з [docs/firmware](../firmware/).
2. Запустіть, виберіть **ChipType: ESP32**, **WorkMode: Develop**, **LoadMode: UART**.
3. Заповніть рядки (позначте галочки ліворуч):

   | Файл | Адреса | Для оновлення | Для чистої установки |
   |---|---|---|---|
   | `bootloader.bin` | `0x1000` | — | так |
   | `partitions.bin` | `0x8000` | — | так |
   | `boot_app0.bin` | `0xe000` | так | так |
   | `informer-clock-ua-esp32.bin` | `0x10000` | так | так |

   Або одним файлом: `informer-clock-ua-esp32-full.bin` за адресою `0x0` (лише чиста установка).
4. **SPI SPEED: 40MHz**, **SPI MODE: DIO**; якщо є поле FLASH SIZE — **32Mbit** (4 МБ). Галочку **DoNotChgBin** — поставити.
5. Виберіть COM-порт і швидкість (**460800** або **230400**). Для чистої установки спершу **ERASE**, потім **START**.
6. Після «FINISH» натисніть **EN**/**RST** на платі або перепідключіть USB.

## Спосіб 4. esptool (будь-яка система)

```sh
python3 -m pip install esptool
# оновлення (налаштування зберігаються)
esptool.py --chip esp32 --port PORT --baud 230400 write_flash -z \
  0xe000 boot_app0.bin 0x10000 informer-clock-ua-esp32.bin
# чиста установка
esptool.py --chip esp32 --port PORT erase_flash
esptool.py --chip esp32 --port PORT --baud 230400 write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect \
  0x1000 bootloader.bin 0x8000 partitions.bin 0xe000 boot_app0.bin 0x10000 informer-clock-ua-esp32.bin
```

У esptool 5 команди пишуться через дефіс (`write-flash`, `erase-flash`), старі назви працюють із попередженням.

## Спосіб 5. З вихідного коду (PlatformIO)

```sh
cd firmware
pio run -e clock -t upload          # USB; флеш не стирається — налаштування зберігаються
pio run -e clock -t upload --upload-port <адреса-годинника>   # по Wi-Fi, див. wifi-debug.md
```

Докладно — [збирання з вихідного коду](building.md).

## Спосіб 6. По мережі (коли на годиннику вже Informer Clock UA)

Веб-інтерфейс → **«Оновлення» → «Оновити»**. Годинник порівнює свою версію з
[update/esp32.json](../../update/esp32.json) у цьому репозиторії й пропонує оновлення, лише коли там **новіша** версія.
Файл прошивки завантажується з GitHub (HTTPS) за адресою з тегом версії, тож під одним номером завжди та сама збірка.
Годинник із прошивкою автора оригіналу оновлень Informer Clock UA не бачить і навпаки — перехід лише через USB.

## Перевірка файлів

Контрольні суми — у [docs/firmware/SHA256SUMS.txt](../firmware/SHA256SUMS.txt) (і в архіві):

```sh
shasum -a 256 -c SHA256SUMS.txt                                   # macOS, Linux (у теці з файлами)
certutil -hashfile informer-clock-ua-esp32.bin SHA256             # Windows
```

## Після прошивки

- Чиста установка: годинник створить точку Wi-Fi **«Clock ESP»** (без пароля) → `http://192.168.4.1` → майстер.
- Оновлення: годинник під'єднається до вашої мережі, як і раніше; сторінка «Оновлення» покаже нову версію.
- Повернутися на прошивку автора можна так само через USB, файлами з архіву автора на
  [Thingiverse](https://www.thingiverse.com/thing:3924947) (його інструкція — там само).
