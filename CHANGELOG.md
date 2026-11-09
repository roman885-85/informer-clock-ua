# Історія змін / Changelog

Формат: нові версії вгорі. Те саме для веб-інтерфейсу годинника — `update/changelog.json`.
Format: newest first. The clock's web interface reads the same history from `update/changelog.json`.

## 1.1.5 — 2026-11-09

**Виправлено / fixed**
- Home Assistant: годинник більше не публікує порожню дату. Стан сутності «Set date» (`text.*_set_date`, топік `d_m`,
  утримуване повідомлення) годинник надсилав порожнім рядком, а Home Assistant відкидає його за шаблоном дати — у журналі
  HA при кожному його запуску й кожному підключенні годинника була помилка «Value error … does not match expected
  pattern». Тепер, коли дату не задано, годинник публікує свою сьогоднішню дату (лише коли час уже відомий). /
  Home Assistant: the clock no longer publishes an empty date. The clock sent the state of the “Set date” entity
  (`text.*_set_date`, topic `d_m`, a retained message) as an empty string, and Home Assistant rejects it by the date
  pattern — the HA log had a “Value error … does not match expected pattern” error at every HA start and every clock
  reconnect. Now, when no date is set, the clock publishes its current date (only once the time is known).

## 1.1.4 — 2026-11-07

**Виправлено / fixed**
- Сторож головного циклу більше не перезапускає справний годинник. У всіх версіях з 1.0.0 годинник приблизно раз на
  кілька годин сам перезапускався з записом «loop guard: loop() stuck > 180 s» на будь-якому етапі, хоча цикл працював:
  сторож читав час раніше за мітку циклу, і коли цикл на іншому ядрі оновлював мітку саме в цю мить, різниця ставала
  від'ємною (≈49 діб). Тепер мітка читається першою, а перезапуск — лише якщо перевищення підтверджує наступна
  перевірка. Це й були «зупинки головного циклу» з розділу «Відомо» 1.1.3. / The main-loop guard no longer restarts a
  working clock. In every version since 1.0.0 the clock restarted by itself about once every few hours with “loop
  guard: loop() stuck > 180 s” at any stage, although the loop was running: the guard read the time before the loop's
  stamp, and when the loop on the other core updated the stamp right then, the difference went negative (≈49 days).
  Now the stamp is read first, and a restart needs the next check to confirm it. These were the “main loop stops” from
  the “Known” section of 1.1.3.
- MQTT: годинник більше не рве з'єднання з брокером при кожній синхронізації часу. Від оригіналу 2.7.1 перед
  синхронізацією він відписувався від усіх 12 топіків (порожні брокер Mosquitto вважає зіпсованим пакетом) і
  від'єднувався — у Home Assistant усі сутності годинника з інтервалом синхронізації (у власника — кожні 10 хв) на
  кілька секунд ставали «недоступні». / MQTT: the clock no longer drops the broker connection at every time sync. Since
  the original 2.7.1, before a sync it unsubscribed from all 12 topics (Mosquitto treats empty ones as a malformed
  packet) and disconnected — in Home Assistant all clock entities became “unavailable” for a few seconds at every sync
  interval (every 10 min at the owner's).

**Діагностика / diagnostics**
- Журнал подій: `[GUARD] loop() running, false alarm skipped (N since start)` — сторож відкинув хибну тривогу (до 1.1.4
  тут був би перезапуск); `[MQTT] Display off/on by command (display/set)` — табло вимкнено чи ввімкнено командою MQTT
  (наприклад, з Home Assistant): погасле табло тепер має видиму причину. Перевірка сторожа: `/post?wdbg_guard_race=N`
  (лише з налагодженням по Wi-Fi). / Event log: `[GUARD] loop() running, false alarm skipped (N since start)` — the
  guard rejected a false alarm (before 1.1.4 this was a restart); `[MQTT] Display off/on by command (display/set)` — the
  display was switched off or on by an MQTT command (for example from Home Assistant): a blank display now has a
  visible cause. Guard test: `/post?wdbg_guard_race=N` (Wi-Fi debugging only).

## 1.1.3 — 2026-11-02

**Виправлено / fixed**
- Табло більше не спалахує на кілька секунд на повну яскравість перед бігучим рядком і після нього, а половини
  великого циферблата не світять по-різному. Ефект циферблата «Фэйд» (у випадковому виборі — кожен сьомий перехід) вів
  яскравість до значення, збереженого в зонах табло, а його в режимі фоторезистора ніхто не оновлював. / The display no
  longer flashes at full brightness for a few seconds before and after the scrolling line, and the halves of the big
  clock face no longer differ in brightness. The “Fade” clock-face effect (every seventh transition when random) faded
  to the intensity stored in the display zones, which nobody updated in photoresistor mode.
- Новина на табло більше не закінчується хвостом «... [2072 chars]». Його дописує GNews: на безкоштовному тарифі
  сервіс дає лише початок тексту статті (≈260 знаків, обірваний посеред слова). Тепер годинник прибирає хвіст і
  закінчує новину на останньому повному реченні (або на цілому слові з «...»). / A news item on the display no longer
  ends with a “... [2072 chars]” tail. GNews adds it: on the free plan the service gives only the beginning of the
  article text (≈260 characters, cut mid-word). Now the clock removes the tail and ends the news at the last complete
  sentence (or at a whole word with “...”).
- Сторінка «Новини» веб-інтерфейсу: бігучий рядок іде зі швидкістю табло («Швидкість прокрутки»). Раніше прохід
  завжди тривав 18 с, тож довга новина мчала в кілька разів швидше, а коротка повзла. / The “News” page of the web
  interface: the scrolling line moves at the display's speed (“Scrolling speed”). Before, a pass always took 18 s, so a
  long news item raced several times faster and a short one crawled.
- Сторож головного циклу перезапускає годинник одразу (`Software reset`). Раніше перезапуск із задачі таймера при
  з'єднанні з роутером не завершувався, і годинник перезапускала лише апаратна страховка через кілька секунд
  (`WATCHDOG reset`). / The main-loop guard restarts the clock right away (`Software reset`). Before, the restart from
  the timer task did not finish while connected to the router, and only the hardware backup restarted the clock a few
  seconds later (`WATCHDOG reset`).

**Змінено / changed**
- Новини GNews для країни «Україна» — українською (`lang=uk`); раніше сервіс давав і російськомовні статті. Для інших
  країн — як раніше. / GNews news for the country “Ukraine” are in Ukrainian (`lang=uk`); before, the service also gave
  Russian-language articles. Other countries — as before.

**Діагностика / diagnostics**
- Перед перезапуском сторожем циклу чи вартовим подій Wi-Fi годинник записує стан задач: після запуску в журналі
  подій — `[BOOT] stuck tasks (…)` і рядок на задачу (стан, запас стека, адреси ланцюжка викликів; розшифровка — ELF
  випуску). Перевірка: `/post?wdbg_loop_spin=N` (лише з налагодженням по Wi-Fi). / Before a restart by the loop guard
  or the Wi-Fi event watchdog the clock records the task state: after the start the event log shows
  `[BOOT] stuck tasks (…)` and a line per task (state, free stack, call-chain addresses; decode with the release ELF).
  Test: `/post?wdbg_loop_spin=N` (Wi-Fi debugging only).

**Відомо / known**
- Зрідка головний цикл зупиняється (у власника — кілька разів за ніч); сторож перезапускає годинник за 3 хв,
  налаштування не губляться. Причину шукаємо: знімок задач покаже, де стоїть цикл, — такий журнал подій варто надіслати
  в Issues. / Occasionally the main loop stops (a few times a night at the owner's); the guard restarts the clock
  within 3 min, settings are kept. The cause is being investigated: the task snapshot shows where the loop stops —
  please send such an event log to Issues.

## 1.1.2 — 2026-11-01

**Виправлено / fixed**
- Годинник більше не «зависає в мережі». Буває, що він утрачає роутер, а ядро Wi-Fi про це не дізнається (обробка
  подій Wi-Fi стала): годинник і далі вважає себе підключеним, кожні 6 хв даремно перепідключається й лишається поза
  мережею до вимкнення живлення (у власника — 1,5 год; у журналі подій — «Forced re-association … router silent» по
  колу). Тепер він це помічає й перезапускається сам: за 8 с після такого перепідключення, коли події Wi-Fi не
  прийшли, або за хвилину, коли ядро каже «підключено», а драйвер — ні; запасне правило — роутер мовчить, а з
  попереднього перепідключення годинник жодного разу не підключився. / The clock no longer “hangs on the network”.
  Sometimes it loses the router while the Wi-Fi core never learns about it (Wi-Fi event handling stops): the clock
  still believes it is connected, re-associates every 6 min to no avail and stays off the network until a power cycle
  (1.5 h at the owner's; the event log shows “Forced re-association … router silent” over and over). Now it notices
  and restarts by itself: 8 s after such a re-association if no Wi-Fi events arrive, or after a minute if the core
  says “connected” while the driver does not; the fallback rule — the router is silent and the clock has not
  connected even once since the previous re-association.
- Перезапуск більше не може «зависнути» в очікуванні Wi-Fi: плановий перезапуск і сторож циклу страхує апаратний
  сторож RTC (3 с). / A restart can no longer hang waiting for Wi-Fi: planned restarts and the loop guard are backed
  by the hardware RTC watchdog (3 s).
- Після планового перезапуску (оновлення, кнопка, /reboot, збереження налаштувань, «router unreachable») журнал подій
  більше не пише хибне «loop was stalled in: …» — воно лишилося для справжніх зависань і аварій. / After a planned
  restart (update, button, /reboot, saving settings, “router unreachable”) the event log no longer shows a false
  “loop was stalled in: …” — it stays for real hangs and crashes.

**Діагностика / diagnostics**
- Записи «Internet offline» і «Forced re-association» — зі станом Wi-Fi: скільки подій оброблено, яка й коли остання,
  на якій події й на якому кроці стоїть обробка, стан задачі подій, чи асоційований драйвер, стан ядра, вільна
  пам'ять. / The “Internet offline” and “Forced re-association” records carry the Wi-Fi state: events handled, the
  last one and when, which event and step the handling is stuck on, the event task state, whether the driver is
  associated, the core status, free memory.
- Налагодження по Wi-Fi: `/post?wdbg_evt_reassoc=N` і `/post?wdbg_evt_block=N` — імітація «застиглої» обробки подій
  (з примусовим перепідключенням або з обривом зв'язку). / Wi-Fi debugging: `/post?wdbg_evt_reassoc=N` and
  `/post?wdbg_evt_block=N` simulate stuck event handling (with a forced re-association or a link drop).

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
