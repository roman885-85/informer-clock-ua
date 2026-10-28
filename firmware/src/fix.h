// Виправлення помилок оригіналу 2.7.1 (нове, не з прошивки автора). Точна копія оригіналу — тег reconstructed-2.7.1.
// EN: Fixes and additions to the original 2.7.1 firmware: declarations and constants.
// Fixes and additions to the original 2.7.1 firmware (new code, not from the original author).
#pragma once
#include "version.h"

// ---------- мережа (netfix.cpp) ----------
#define NET_CONNECT_TIMEOUT_S 10        // межа блокуючого з'єднання (HTTPS-клієнт ядра)
// TCP-з'єднання з тайм-аутом: 1 — з'єднались, 0 — вузол відповів відмовою (живий), -1 — тиша або помилка.
int8_t net_tcp_probe(IPAddress ip, uint16_t port, uint32_t ms);
// Чи відповідає роутер (шлюз): TCP на порт 80, «з'єднались» або «відмова» — так.
bool net_gateway_alive();

// ---------- головний цикл (core.cpp) ----------
extern bool g_fx_dns_on;                // DNS captive-порталу запущено (wifi.cpp, captive_dns_start)
#define NET_DEFER_MAX_MS 300000UL       // мережеві задачі чекають кінця анімації не довше 5 хв
extern uint32_t g_fx_gate;              // коли востаннє виконано блок мережевих задач

// ---------- час (netfix.cpp, timefix.cpp) ----------
extern volatile uint32_t g_fx_sntp_n;   // скільки разів SNTP встановив системний час
bool fx_time_known();                   // TimeLib має справжній час (не 1970 р.)
void fx_time_boot();                    // після перезапуску — системний час ESP32 до першої синхронізації
int32_t fx_tz_offset();                 // пояс tz, с
int32_t fx_dst_offset(time_t utc);      // літній час: sumT 0 — ні, 1 — завжди +1 год, 2 — авто за правилами ЄС
extern int32_t g_fx_off_applied;        // зсув (пояс + літній), з яким востаннє ставили TimeLib
extern bool g_fx_off_known;             // TimeLib ставили в місцевому часі (NTP, системний час, вручну)
extern uint32_t g_fx_dst_t;             // коли востаннє перевіряли перехід літнього часу
void fx_time_offset_set();              // TimeLib щойно поставлено в місцевому часі з поточними tz/sumT
void fx_time_offset_apply();            // tz/sumT змінились або настав перехід — перерахувати показаний час

// ---------- голос щогодини (sound.cpp, timefix.cpp) ----------
extern bool g_fx_voice_init;            // автомат оголошення підготовлено
extern bool g_fx_voice_test;            // кнопка «Тест голоса»: повне оголошення поточного часу зараз

// ---------- стан запуску на табло (status.cpp) ----------
enum { FXS_BOOT, FXS_TIME, FXS_NET, FXS_UPDATE, FXS_LANG, FXS_WEATHER, FXS_NEWS, FXS_RATES, FXS_DATA, FXS_FLASH };
#define FX_STATUS_DELAY_MS 400          // коротші очікування не показуємо (без миготіння)
#define FX_BOOT_MAX_MS 300000UL         // «запуск» — не довше 5 хв від старту
extern bool g_fx_booting;               // до першого звичайного вмісту після запуску
void fx_status_begin(uint8_t kind, bool always = false);   // always — і поза запуском
void fx_status_end();
// показ стану на час блоку коду (вкладені блоки — один показ, зовнішній)
struct FxStatusScope {
  FxStatusScope(uint8_t kind, bool always = false) { fx_status_begin(kind, always); }
  ~FxStatusScope() { fx_status_end(); }
};

// ---------- плавна яскравість (bright.cpp) ----------
#define FX_ADC_MS 100                   // замір фоторезистора
#define FX_ADC_K 0.08f                  // згладжування (≈1,2 с)
#define FX_MAP_MS 500                   // перерахунок рівня за таблицею автора
#define FX_HOLD_MS 3000                 // різниця в один рівень має протриматись стільки, щоб стати новою ціллю
#define FX_ADC_MARGIN 20                // запас АЦП біля межі рівнів: у межах ±запасу ціль не міняється
#define FX_DIM_FULL_MS 2000             // плавна яскравість (dim.cpp): весь діапазон MAX7219 (1/32 → 31/32) за стільки мс
#define FX_DIM_DQ (3.4339872f / FX_DIM_FULL_MS)   // крок за 1 мс у ln шпаруватості (ln 31 — весь діапазон)
void fx_dim_begin();                    // потік плавної яскравості (setup, після P.begin)
void fx_dim_follow(bool on, uint8_t tgt);   // on — веде фоторезистор; tgt — ціль 0..15
void fx_dim_jump(uint8_t lvl);          // поставити рівень одразу (старт, пробудження)
uint8_t fx_dim_level();                 // найближчий цілий рівень зараз
uint8_t fx_dim_step_ms();               // один крок потоку (1 мс) — для імітатора tools/sim/dimsim
uint16_t fx_photo_adc();                // згладжене значення фоторезистора
void fx_bright_tick();
void fx_photo_print();                  // рядок налагодження автора «Display INFO: Photoresistor data …»
uint8_t fx_led_ramp(uint8_t cur, uint8_t tgt);
void fx_bright_actual(uint8_t lvl);     // хтось інший поставив табло цей рівень (пробудження — 0)

// ---------- нічна тиша (night.cpp) ----------
extern bool g_fx_quiet;                 // табло погашене (розклад D=1 або MQTT): вміст і його завантаження стоять
void fx_night_tick();

// ---------- запис причини аварії (crash.cpp) ----------
#define FX_CRASH_MAGIC 0xc4a5ed01UL
struct FxCrash { uint32_t magic, kind, cause, pc, vaddr, up, n, bt[8]; };   // kind: 1 — виняток, 2 — panic
extern FxCrash rtc_crash;
extern String g_fx_crash_last;          // запис попередньої аварії (перенесено з RTC-пам'яті при старті)
void fx_crash_text(String &out);

// ---------- налагодження по Wi-Fi (wdbg.cpp) ----------
extern bool g_fx_wdbg;                  // увімкнено у веб-інтерфейсі (ключ wdbg, сторінка System)
void fx_wdbg_set(bool on);
void fx_wdbg_tick();
void fx_wdbg_line(const char *s, size_t n);   // рядок у сетевий журнал (будь-яка задача)

// ---------- сторож головного циклу (guard.cpp) ----------
#define LOOP_GUARD_MS 180000UL          // loop() не доходить до кінця довше — перезапуск (запас: дві повільні HTTPS-загрузки за цикл)
#define LOOP_GUARD_MAGIC 0x600df00dUL   // позначка в RTC-пам'яті: перезапуск зробив сторож циклу
extern uint32_t rtc_guard;
void loop_guard_start();
void loop_guard_feed();                 // «цикл живий» — наприкінці loop() і в довгих циклах очікування
extern volatile uint32_t g_fx_alive_ms;
