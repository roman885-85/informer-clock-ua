# Clock Informer 2.7.1 — відтворений код / reconstructed source

**UA.** Цей стан репозиторію (тег `reconstructed-2.7.1`) — код прошивки **Clock Informer 2.7.1** для ESP32 автора
**Semen_p** (<https://www.thingiverse.com/thing:3924947>, CC BY-NC 4.0), відтворений з машинного коду оригіналу з
поведінкою оригіналу, разом з його помилками. Автор не публікує вихідного коду ESP32-версії; це не код автора, а
реконструкція. Проєкт **Informer Clock UA** (виправлення, нові функції, власні оновлення) починається з наступного
коміту — див. гілку `main`. Порівняння: `git diff reconstructed-2.7.1 v1.0.0`.

Збирання: `cd firmware && pio run -e clock` (PlatformIO, Arduino-ESP32 1.0.4). Веб-інтерфейс у
`firmware/src/web_assets.h` і `web/original/` — оригінальний інтерфейс автора без змін.

**EN.** This state of the repository (tag `reconstructed-2.7.1`) is the source of the **Clock Informer 2.7.1** ESP32
firmware by **Semen_p** (<https://www.thingiverse.com/thing:3924947>, CC BY-NC 4.0), reconstructed from the original
machine code with the original's behaviour, bugs included. The author does not publish the ESP32 source; this is not the
author's code but a reconstruction. The **Informer Clock UA** project (fixes, new features, its own updates) starts with
the next commit — see the `main` branch. Compare: `git diff reconstructed-2.7.1 v1.0.0`.

Build: `cd firmware && pio run -e clock` (PlatformIO, Arduino-ESP32 1.0.4). The web interface in
`firmware/src/web_assets.h` and `web/original/` is the author's original interface, unchanged.

License: [CC BY-NC 4.0](LICENSE) (attribution, non-commercial). `firmware/lib/MD_Parola` and `firmware/lib/MD_MAX72XX` —
LGPL-2.1 (majicDesigns; MD_Parola with the author's effects, see `FORK.md`); `MHZ19_uart`, `Base64` — MIT.
