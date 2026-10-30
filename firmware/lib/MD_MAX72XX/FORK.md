# MD_MAX72XX 3.2.3 — форк / fork

Оригінал / original: [MD_MAX72XX](https://github.com/MajicDesigns/MD_MAX72XX) 3.2.3, © majicDesigns (marco_c),
ліцензія / license **LGPL-2.1** (див. / see `LICENSE`).

Змінено / modified (2026-10-28, Informer Clock UA):

| Файл / file | Зміна / change |
|---|---|
| `src/MD_MAX72xx.cpp`, `spiSend()` | «слабкі» гачки `md_spi_lock()` / `md_spi_unlock()` навколо передачі по SPI. Прошивка (`firmware/src/dim.cpp`) визначає їх як замок: потік плавної яскравості пише регістр яскравості MAX7219 до тисячі разів на секунду, і передача кадру табло не повинна перериватись посередині / weak hooks around the SPI transfer; the firmware defines them as a lock, because the smooth-brightness thread writes the MAX7219 intensity register up to a thousand times per second and a frame transfer must not be interrupted |

Без визначення гачків бібліотека працює як оригінал / without the hooks defined the library behaves as the original.
Решта файлів — без змін / all other files are unchanged.
