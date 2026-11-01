# Історія змін / Changelog

Формат: нові версії вгорі. Те саме для веб-інтерфейсу годинника — `update/changelog.json`.
Format: newest first. The clock's web interface reads the same history from `update/changelog.json`.

## 1.1.1 — 2026-11-01

**Виправлено / fixed**
- Годинник більше не «застрягає» без роутера. Якщо після зникнення Wi-Fi (роутер вимикали чи перезапускали, не було
  світла) він кілька циклів перепідключення поспіль не може повернутися, а до його точки доступу ніхто не під'єднаний,
  — годинник сам перезапускається: через ~10 хв, а якщо роутера все ще немає — рідше (~20, ~40 хв, далі раз на
  годину). Раніше він міг годинами (у власника — понад добу) лишатися поза мережею до вимкнення живлення, хоча свіжий
  запуск підключається за секунди. / The clock no longer gets stuck without the router. If after Wi-Fi disappears
  (the router was switched off or restarted, a power cut) it fails several reconnect cycles in a row and nobody is
  connected to its own access point, the clock restarts itself: after ~10 min, and less often while the router is still
  gone (~20, ~40 min, then once an hour). Before, it could stay off the network for hours (over a day at the owner's)
  until a power cycle, although a fresh start connects within seconds.

**Діагностика / diagnostics**
- Запис «AP restored» у журналі подій — з номером циклу, причиною останньої відмови й результатом пошуку мереж (чи
  видно збережену мережу); перед перезапуском — `[WIFI] Restart: router unreachable (…)`. / The “AP restored” event-log
  record now has the cycle number, the last failure reason and the network scan result (whether a saved network is
  visible); before the restart — `[WIFI] Restart: router unreachable (…)`.
- Налагодження по Wi-Fi: `/post?wdbg_wifi_off=N` — імітація зникнення роутера на N с. / Wi-Fi debugging:
  `/post?wdbg_wifi_off=N` simulates the router disappearing for N s.

## 1.1.0 — 2026-10-31

**Виправлено / fixed**
- Годинник більше не втрачає інтернет через ~1,5 год роботи. Перевірка інтернету (з 1.0.0) і журнал на порту 23
  закривали з'єднання функцією `lwip_close`, яка в ядрі Arduino-ESP32 1.0.4 не звільняє місце сокета; після 8–10
  перевірок усі 10 місць були зайняті, і жодне вихідне з'єднання (час, погода, новини, оновлення) не відкривалось до
  перезапуску. Тепер — `lwip_close_r`, як у самого ядра. / The clock no longer loses internet after ~1.5 h of work: the
  internet check (since 1.0.0) and the port-23 log closed connections with `lwip_close`, which in the Arduino-ESP32
  1.0.4 core does not free the socket slot; after 8–10 checks all 10 slots were taken and no outbound connection (time,
  weather, news, updates) could be opened until a restart. Now `lwip_close_r` is used, as in the core itself.
- Веб-інтерфейс більше не валить годинник під навантаженням: його повідомлення розсилає потік веб-сервера, а не
  головний цикл; виправлено два падіння в бібліотеці AsyncTCP (FIN під час закриття з'єднання, нестача місць для
  з'єднань) і зависання її черги подій. / The web interface no longer crashes the clock under load: its messages are
  sent from the web-server thread, not the main loop; two crashes in AsyncTCP (FIN while closing, out of connection
  slots) and a stall of its event queue are fixed.
- Оновлення по Wi-Fi стійке до пауз зв'язку: на час оновлення зупиняються всі служби, крім веб-інтерфейсу; дані
  чекаються до 30 с, до 3 спроб. / Wi-Fi updates survive link pauses: all services except the web interface stop
  during an update; data is awaited up to 30 s, up to 3 attempts.
- Wi-Fi без енергозбереження: швидша відповідь веб-інтерфейсу, менше втрат пакетів. / Wi-Fi power saving is off:
  faster web interface, fewer lost packets.

**Нове / added**
- Великий циферблат перелистує цифри, що змінились, вгору — на всю висоту двох поверхів (коли ввімкнено «зсув цифр»).
  / The big clock face rolls the changed digits upward over the full height of both floors (with “sliding digits” on).
- Плавна яскравість без сходинок: між 16 рівнями MAX7219 — змішування сусідніх рівнів, увесь діапазон за 2 с.
  / Smooth brightness without steps: neighbouring MAX7219 levels are blended, the whole range takes 2 s.
- Діагностика: сила сигналу Wi-Fi (дБм) у журналі подій і в `/content` (`wifi_rssi`), причина невдалої перевірки
  мережі в журналі, перепис сокетів (`sock_diag`); якщо сокети все ж закінчились — запис і перезапуск.
  / Diagnostics: Wi-Fi signal strength in the event log and `/content` (`wifi_rssi`), the reason of a failed network
  check in the log, a socket census (`sock_diag`); if sockets ever run out — a log record and a restart.

**Порада / tip**
- Не ставте годинник упритул до роутера: поруч приймач перевантажений (33–67 % втрачених пакетів, обриви оновлення);
  на відстані 1,5–2 м — 0 %. / Don't put the clock right next to the router: up close its receiver is overloaded
  (33–67 % packet loss, broken updates); at 1.5–2 m — 0 %.

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
