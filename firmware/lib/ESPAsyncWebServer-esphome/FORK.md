# ESPAsyncWebServer-esphome 2.1.0 — форк / fork

Оригінал / original: [ESPAsyncWebServer-esphome](https://github.com/esphome/ESPAsyncWebServer) 2.1.0 (Hristo Gochkov,
ESPHome), ліцензія / license **LGPL-3.0** (див. / see `LICENSE`; у пакеті з реєстру PlatformIO файла ліцензії немає,
текст LGPL-3.0 додано сюди / the registry package has no license file, the LGPL-3.0 text is added here).

Змінено / modified (2026-10-28, Informer Clock UA):

| Файл / file | Зміна / change |
|---|---|
| `src/AsyncWebSocket.h` | оголошення «слабкого» гачка `ws_async_hook(AsyncWebSocket *)` — його визначає прошивка (`firmware/src/ws.cpp`) / declaration of the weak hook `ws_async_hook(AsyncWebSocket *)`, defined by the firmware |
| `src/AsyncWebSocket.cpp`, `AsyncWebSocketClient::_onPoll`, `_onAck` | виклик гачка з потоку AsyncTCP: прошивка кладе повідомлення веб-інтерфейсу в чергу, а розсилає їх цей потік. `AsyncWebSocket` не захищений від інших потоків — розсилка з головного циклу псувала купу й валила годинник / the hook is called from the AsyncTCP thread: the firmware queues web-interface messages and this thread sends them. `AsyncWebSocket` is not thread-safe — sending from the main loop corrupted the heap and crashed the clock |
| `library.json` | без залежностей (AsyncTCP — форк у `../AsyncTCP-esphome`) / no dependencies (AsyncTCP is the fork in `../AsyncTCP-esphome`) |

Решта файлів — без змін / all other files are unchanged.
