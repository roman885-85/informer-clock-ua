# Історія змін / Changelog

Формат: нові версії вгорі. Те саме для веб-інтерфейсу годинника — `update/changelog.json`.
Format: newest first. The clock's web interface reads the same history from `update/changelog.json`.

## 1.0.0 — 2026-10-28 (попередній випуск / pre-release)

Перший випуск Informer Clock UA на основі Clock Informer 2.7.1 (Semen_p, CC BY-NC 4.0).
First Informer Clock UA release, based on Clock Informer 2.7.1 by Semen_p (CC BY-NC 4.0).

**Окремий проєкт / independent project**
- Оновлення прошивки, мовні пакети, історія змін, файли для SD-карти й корпусів — з цього репозиторію; власні номери
  версій. / Firmware updates, language packs, change history, SD-card and case files come from this repository; own
  version numbers.

**Виправлено / fixed**
- Зависання без інтернету, «годинник не видно в мережі»: перевірка інтернету з тайм-аутами, Wi-Fi не перепідключається,
  коли інтернету немає лише в провайдера, безпечний DNS, з'єднання HTTPS з тайм-аутом, сторож головного циклу.
  / Hangs without internet and dropping off the network: timed internet checks, no Wi-Fi resets when only the provider
  is down, safe DNS, HTTPS connect timeout, main-loop watchdog.
- Службові задачі (яскравість, ніч, ранок, відновлення мережі) більше не чекають кінця прокрутки новин.
  / Service tasks (brightness, night, morning, network recovery) no longer wait for news scrolling to finish.
- Час не «стрибає» після перезапуску (потрібна свіжа відповідь NTP); пояс і літній час діють одразу.
  / No time jumps after restarts (a fresh NTP reply is required); time zone and summer time apply immediately.
- Голос щогодини завжди називає годину, «Тест голосу» — поточний час; кнопка вимикає мелодію будильника DFPlayer;
  будильник уночі вмикає табло. / The hourly voice always says the hour, the voice test announces the current time, the
  button stops the DFPlayer alarm melody, a night alarm turns the display on.
- Повідомлення вночі не лишає табло ввімкненим; немає зайвої точки о 9:59→10:00; рівніша прокрутка; інтерфейс не
  береться зі старого кешу; паролі Wi-Fi приховано в консолі. / Night messages no longer leave the display on; no stray
  dot at 9:59→10:00; smoother scrolling; no stale cached interface; Wi-Fi passwords masked in the console.

**Нове / added**
- Стан запуску на табло: значок із рухом і напис (і на двох поверхах). / Start-up status on the display: animated icon
  and text (also on two floors).
- Плавна яскравість за датчиком світла; нічна тиша (без завантажень, поки табло погашене). / Smooth light-sensor
  brightness; night quiet (no downloads while the display is off).
- Літній час «Авто» за правилами ЄС; системний час одразу після перезапуску. / “Auto” summer time by EU rules; system
  time right after a restart.
- Налагодження по Wi-Fi: журнал на порту 23, прошивка по Wi-Fi; причина аварії в журналі подій. / Wi-Fi debugging:
  log on port 23, firmware upload over Wi-Fi; crash cause in the event log.

## reconstructed-2.7.1

Відтворений код Clock Informer 2.7.1 з поведінкою оригіналу — основа проєкту (тег `reconstructed-2.7.1`).
Reconstructed Clock Informer 2.7.1 code with the original's behaviour — the base of the project (tag `reconstructed-2.7.1`).
