# MD_Parola 3.5.2 — форк із ефектами Clock Informer / fork with the Clock Informer effects

Оригінал / original: [MD_Parola](https://github.com/MajicDesigns/MD_Parola) 3.5.2, © majicDesigns (marco_c),
ліцензія / license **LGPL-2.1** (див. / see `LICENSE`).

Змінено / modified (2026-10-28, Informer Clock UA):

| Файл / file | Зміна / change |
|---|---|
| `src/MD_Parola_ci.cpp` | **новий / new**: 10 ефектів автора прошивки Clock Informer (Semen_p) — Neon, Melt, Rain, Shatter, Spiral, Pixelate, Jitter, Cascade, Comb, Iris; відтворено з машинного коду прошивки 2.7.1, поведінку звірено в емуляторі / reconstructed from the 2.7.1 machine code, behaviour verified in an emulator |
| `src/MD_Parola.h` | коди ефектів `PA_NEON`…`PA_IRIS` (29–38) у `textEffect_t`, оголошення `effectCi*`, `getZoneCount()` / effect codes, declarations, `getZoneCount()` |
| `src/MD_PZone.cpp` | виклик нових ефектів у `zoneAnimate()` / dispatch of the new effects in `zoneAnimate()` |

Решта файлів — без змін / all other files are unchanged. MD_MAX72XX (`../MD_MAX72XX`) — 3.2.3 без змін / unchanged.
