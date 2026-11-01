# AsyncTCP-esphome 1.2.2 — форк / fork

Оригінал / original: [AsyncTCP-esphome](https://github.com/esphome/AsyncTCP) 1.2.2 (Hristo Gochkov, ESPHome),
ліцензія / license **LGPL-3.0** (див. / see `LICENSE`).

Змінено / modified (2026-10-28…30, Informer Clock UA):

| Файл / file | Зміна / change |
|---|---|
| `src/AsyncTCP.cpp`, `_tcp_recv` | FIN, що приходить між `tcp_arg(NULL)` і `tcp_recv(NULL)` під час закриття, кличе обробник з `arg == NULL` — тепер звільняє буфер і виходить, а не падає / a FIN arriving while a connection is being closed calls the handler with `arg == NULL`: the buffer is freed instead of crashing |
| `src/AsyncTCP.cpp`, `AsyncServer::_accept` | lwIP кличе з `pcb == NULL` (ERR_MEM), коли скінчились місця для з'єднань — тепер виходить без падіння / lwIP calls it with a NULL pcb (ERR_MEM) when connection slots run out: now returns instead of crashing |
| `src/AsyncTCP.cpp`, `_init_async_event_queue` | черга подій — 128 замість 32: під навантаженням повна черга з `portMAX_DELAY` зупиняла потік lwIP, і сторож задач перезапускав годинник / event queue 128 instead of 32: a full queue with `portMAX_DELAY` stalled the lwIP thread and the task watchdog restarted the clock |
| `src/AsyncTCP.cpp`, `src/AsyncTCP.h` | нова функція `async_tcp_queue_waiting()` — скільки подій чекає в черзі (веб-сервер годинника притримує відправку, коли черга наповнюється) / new `async_tcp_queue_waiting()`: events waiting in the queue (the clock's web server holds back sending when it fills up) |
| `library.json` | без залежностей: бібліотека береться з цієї теки, а не з реєстру PlatformIO / no dependencies: the library is used from this folder, not from the PlatformIO registry |

Решта файлів — без змін / all other files are unchanged.
