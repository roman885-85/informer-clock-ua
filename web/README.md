# web — веб-інтерфейс / web interface

**UA.** `original/index.html.gz` — веб-інтерфейс Clock Informer 2.7.1 автора Semen_p (зібраний React, без змін,
CC BY-NC 4.0). `webui_patch.py` застосовує до нього точні правки (нові поля на існуючих сторінках, адреси цього
проєкту, назва, історія змін з `update/changelog.json`) і пише результат у `firmware/src/web_assets.h` та `build/`:

```sh
python3 web/webui_patch.py
python3 tools/sim/mockclock.py --port 8791     # перегляд без годинника: http://localhost:8791
```

Кожна правка має збігтися точно; якщо лишилась адреса сервера автора — скрипт зупиняється.

**EN.** `original/index.html.gz` is the Clock Informer 2.7.1 web interface by Semen_p (built React, unchanged,
CC BY-NC 4.0). `webui_patch.py` applies exact patches (new fields on existing pages, this project's addresses, name,
change history from `update/changelog.json`) and writes `firmware/src/web_assets.h` and `build/`. Every patch must match
exactly; the script stops if any address of the author's server is left. Preview without a clock:
`python3 tools/sim/mockclock.py --port 8791`.
