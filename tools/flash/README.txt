Informer Clock UA — прошивка годинника через USB
=================================================

УКРАЇНСЬКА

Що потрібно: годинник Informer Clock UA / Clock Informer на ESP32, USB-кабель з передачею даних,
драйвер USB-UART плати (CP210x або CH340 — див. нижче).

Windows:   двічі клацніть flash.bat і виберіть
             1 — оновлення: записується лише програма, налаштування й Wi-Fi зберігаються
                 (так само при переході з прошивки автора Clock Informer);
             2 — чиста установка: флеш-пам'ять стирається, налаштування скидаються.
           Або з командного рядка: flash.bat update COM3 / flash.bat full COM3
macOS, Linux: у Терміналі в цій теці:  sh flash.sh   (або sh flash.sh update /dev/cu.usbserial-0001)
           Потрібен python3; esptool поставиться сам у ~/.cache/informer-clock-ua.

Якщо «годинник не знайдено»: інший кабель або порт USB; встановіть драйвер;
якщо запис не починається — тримайте кнопку BOOT на платі ESP32, поки не почнеться запис.

Після чистої установки: під'єднайтеся до Wi-Fi «Clock ESP» і відкрийте http://192.168.4.1

Драйвери: CP210x — https://www.silabs.com/developer-tools/usb-to-uart-bridge-vcp-drivers
          CH340  — https://www.wch-ic.com/downloads/CH341SER_EXE.html
Перевірка файлів: firmware/SHA256SUMS.txt
Прошивальник у браузері (Chrome, Edge): https://roman885-85.github.io/informer-clock-ua/flasher/
Документація: https://github.com/roman885-85/informer-clock-ua

ENGLISH

You need: an Informer Clock UA / Clock Informer clock on ESP32, a USB data cable,
the board's USB-UART driver (CP210x or CH340 — see above).

Windows:   double-click flash.bat and choose
             1 — update: only the program is written, settings and Wi-Fi are kept
                 (also when switching from the original Clock Informer firmware);
             2 — clean install: the flash is erased, settings are reset.
           Or from a command prompt: flash.bat update COM3 / flash.bat full COM3
macOS, Linux: in a Terminal in this folder:  sh flash.sh   (or sh flash.sh update /dev/ttyUSB0)
           python3 is required; esptool installs itself into ~/.cache/informer-clock-ua.

"Clock not found": try another cable or USB port; install the driver;
if writing does not start — hold the BOOT button on the ESP32 board until it starts.

After a clean install: join the Wi-Fi network "Clock ESP" and open http://192.168.4.1

Browser flasher (Chrome, Edge): https://roman885-85.github.io/informer-clock-ua/flasher/
Documentation: https://github.com/roman885-85/informer-clock-ua
