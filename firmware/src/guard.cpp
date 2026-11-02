// Сторож головного циклу (нове, не з оригіналу).
// EN: Main-loop watchdog (new): restarts the clock if loop() is stuck for more than 3 minutes.
// Ядро 1.0.4 стежить лише за задачею простою ядра 0, тож loop(), що завис у мережевому очікуванні чи в
// нескінченному циклі, сторож не помічає: годинник «живий», але табло й мережа стоять до ручного перезапуску
// (так оригінал і помирав уночі). Таймер щосекунди дивиться, коли loop() востаннє дійшов до кінця; якщо понад
// LOOP_GUARD_MS — позначка в RTC-пам'яті (її й етап циклу setup() запише в журнал подій) і перезапуск плати.
#include "fw.h"
#include "esp_timer.h"
#include "esp_ipc.h"

RTC_NOINIT_ATTR uint32_t rtc_guard;
uint32_t g_fx_gate;
volatile uint32_t g_fx_alive_ms;        // коли loop() востаннє дійшов до кінця (і для запису аварії)
#define s_alive g_fx_alive_ms
static esp_timer_handle_t s_tmr;
extern "C" void esp_restart_noos() __attribute__((noreturn));   // ядро IDF 3.3: перезапуск без обробників зупинки

void loop_guard_feed() { s_alive = millis(); }

static void guard_cb(void *) {
  if (millis() - s_alive <= LOOP_GUARD_MS) return;
  rtc_guard = LOOP_GUARD_MAGIC;
  fx_rtc_wdt_arm(4000);                          // нове (1.1.2): апаратна страховка (core.cpp)
  fx_stuck_snapshot(1);                          // нове (1.1.3): де стоїть кожна задача
  // Виправлено (1.1.3): esp_restart() спершу зупиняє Wi-Fi (esp_wifi_stop) і з задачі esp_timer, коли годинник
  // під'єднаний до роутера, не завершується: перевірка 02.11 — loop() крутиться, мережа справна, а після запуску
  // однаково «WATCHDOG reset» (перезапустила страховка через 4 с). Тож нічні «WATCHDOG reset» після сторожа нічого не
  // кажуть про мережу. Сторож перезапускає без зупинки Wi-Fi — як обробник аварій ядра (радіо скидається разом із
  // системою); після запуску в журналі — «Software reset».
  esp_restart_noos();
}

// Нове (1.1.3): знімок задач перед перезапуском. Уночі 01–02.11 сторож шість разів перезапускав годинник («loop()
// stuck > 180 s» на етапах ping, scan harvest, brightness), і щоразу esp_restart() сам не завершувався (після запуску —
// «WATCHDOG reset» від апаратної страховки): схоже, стали мережеві задачі ядра (tiT, wifi, події), а loop() чекав на них.
// Де саме — видно лише з того, де стоїть кожна задача. Перед перезапуском — стан, вільний стек і до 8 адрес ланцюжка
// викликів кожної задачі (крім простою, ipc і esp_timer) у RTC-пам'ять; setup() переносить їх у журнал подій
// («[BOOT] stuck …»). Адреси — xtensa-esp32-elf-addr2line з ELF тієї ж збірки. Лише читання пам'яті.
RTC_NOINIT_ATTR FxStuck rtc_stuck;
static TaskSnapshot_t s_ts[32];
static FxStuckTask s_ipc_walk;

static inline uint32_t pc_fix(uint32_t pc) {      // адреса повернення → адреса виклику (як у crash.cpp)
  if (pc & 0x80000000) pc = (pc & 0x3fffffff) | 0x40000000;
  return pc - 3;
}
static inline bool sp_ok(uint32_t sp) { return sp >= 0x3ffae010 && sp <= 0x3ffffff0 && (sp & 0xf) == 0; }
static inline bool pc_ok(uint32_t pc) { return pc >= 0x40000000 && pc < 0x40400000; }

// ланцюжок викликів задачі, що не виконується: з кадру, збереженого при перемиканні (pxTopOfStack — перше поле TCB);
// вікна регістрів при перемиканні вже скинуто в стек, тож далі — як у обробнику аварій
static void walk(TaskHandle_t h, FxStuckTask &o) {
  o.n = 0;
  uint32_t top = *(volatile uint32_t *)h;
  if (top < 0x3ffae010 || top > 0x3fffffe0 || (top & 3)) return;
  const volatile uint32_t *f = (const volatile uint32_t *)top;
  uint32_t sp, next;
  if (f[0] != 0) {                               // кадр переривання (XtExcFrame): exit, pc, ps, a0, a1
    if (pc_ok(f[1])) o.pc[o.n++] = f[1];
    next = f[3];
    sp = f[4];
  } else {                                       // кадр vPortYield (XtSolFrame): pc — адреса повернення в того, хто кликав
    next = f[1];
    sp = top;
  }
  while (o.n < 8 && sp_ok(sp)) {
    const volatile uint32_t *b = (const volatile uint32_t *)sp;
    uint32_t pc = next;
    next = b[-4];
    sp = b[-3];
    if (pc == 0 || !pc_ok(pc_fix(pc))) break;
    o.pc[o.n++] = pc_fix(pc);
  }
}
// задача, що саме виконується на іншому ядрі (зазвичай loop() у нескінченному циклі): задача ipc того ядра її витісняє —
// кадр збережено, його й читаємо
static void ipc_walk(void *h) { walk((TaskHandle_t)h, s_ipc_walk); }

void fx_stuck_snapshot(uint8_t why) {
  FxStuck &s = rtc_stuck;
  s.magic = 0;
  s.why = why;
  s.up = millis() / 1000;
  s.heap = ESP.getFreeHeap();
  s.stage = (uint8_t)rtc_stage;
  s.wst = (uint8_t)WiFi.status();
  wifi_evt_snap(s.evt);
  s.cnt = 0;
  s.magic = FX_STUCK_MAGIC;                      // далі запис доповнюється задача за задачею: що встигли — те й буде
  // перелік задач — як для дампу ядра (без блокувань; масив обмежує прохід, навіть якщо список змінюється на ходу)
  UBaseType_t tcb_sz, n = uxTaskGetSnapshotAll(s_ts, sizeof s_ts / sizeof s_ts[0], &tcb_sz);
  // задача, що виконується саме зараз, для FreeRTOS — «ready»: своя (стек живий, де вона — і так відомо) і та, що на
  // іншому ядрі, — її кадр переписується, тож її знімаємо останньою, через задачу ipc того ядра
  BaseType_t other = xPortGetCoreID() == 0 ? 1 : 0;
  TaskHandle_t me = xTaskGetCurrentTaskHandle(), run = xTaskGetCurrentTaskHandleForCPU(other);
  int run_i = -1;
  for (int pass = 0; pass < 2; pass++)           // спершу loop(), далі решта (журнал подій обмежений)
    for (UBaseType_t i = 0; i < n && s.cnt < FX_STUCK_TASKS; i++) {
      TaskHandle_t h = (TaskHandle_t)s_ts[i].pxTCB;
      if ((uint32_t)h < 0x3ffae000 || (uint32_t)h >= 0x40000000) continue;
      const char *nm = pcTaskGetTaskName(h);
      if ((pass == 0) != (strcmp(nm, "loopTask") == 0)) continue;
      if (!strncmp(nm, "IDLE", 4) || !strncmp(nm, "ipc", 3) || !strcmp(nm, "esp_timer")) continue;
      FxStuckTask &o = s.t[s.cnt];
      strncpy(o.name, nm, sizeof o.name - 1);
      o.name[sizeof o.name - 1] = 0;
      o.st = (uint8_t)eTaskGetState(h);
      o.free = (uint16_t)uxTaskGetStackHighWaterMark(h);
      o.n = 0;
      if (h == me) o.st = eRunning;
      else if (h == run) { o.st = eRunning | 0x40; run_i = s.cnt; }   // 0x40 — ipc ще не відповів
      else walk(h, o);
      s.cnt++;
    }
  if (run_i < 0) return;
  s_ipc_walk.n = 0;
  if (esp_ipc_call_blocking(other, ipc_walk, run) == ESP_OK) {   // ядро з вимкненими перериваннями не відповість —
    FxStuckTask &o = s.t[run_i];                                  // тоді апаратна страховка, а в записі лишиться 0x40
    memcpy(o.pc, s_ipc_walk.pc, sizeof o.pc);
    o.n = s_ipc_walk.n;
    o.st = eRunning | 0x80;
  }
}

// налагодження (лише з wdbg): /post?wdbg_loop_spin=N — loop() крутиться N с, не віддаючи керування; за 180 с — сторож
// і знімок задач (loop() виконується на іншому ядрі — його кадр знімає задача ipc того ядра)
volatile uint32_t g_fx_loop_spin_req;
void fx_loop_spin_tick() {
  if (g_fx_loop_spin_req == 0) return;
  uint32_t s = g_fx_loop_spin_req;
  g_fx_loop_spin_req = 0;
  evlog_write(String("[TEST] loop() spinning for ") + s + " s");
  uint32_t t0 = millis();
  while (millis() - t0 < s * 1000) { }
}

// setup(): знімок попереднього перезапуску — у журнал подій
void fx_stuck_log() {
  if (rtc_stuck.magic != FX_STUCK_MAGIC) return;
  rtc_stuck.magic = 0;
  const FxStuck &s = rtc_stuck;
  char b[220];
  uint8_t cur = s.evt[1] & 0xff, step = (s.evt[1] >> 8) & 0xff, last = (s.evt[1] >> 16) & 0xff;
  int k = snprintf(b, sizeof b, "[BOOT] stuck tasks (%s): up %u s, stage %u, Wi-Fi status %u, heap %u, events %u",
                   s.why == 1 ? "loop guard" : "Wi-Fi watchdog", (unsigned)s.up, (unsigned)s.stage, (unsigned)s.wst,
                   (unsigned)s.heap, (unsigned)s.evt[0]);
  if (last != 0xff && k < (int)sizeof b) k += snprintf(b + k, sizeof b - k, ", last %u %u s ago", last, (unsigned)(s.evt[3] / 1000));
  if (cur != 0xff && k < (int)sizeof b) snprintf(b + k, sizeof b - k, ", handler busy: event %u step %u for %u s", cur, step, (unsigned)(s.evt[2] / 1000));
  evlog_write(String(b));
  static const char ST[] = "RrBSD";             // running, ready, blocked, suspended, deleted
  for (uint8_t i = 0; i < s.cnt && i < FX_STUCK_TASKS; i++) {
    const FxStuckTask &t = s.t[i];
    uint8_t st = t.st & 0x3f;
    k = snprintf(b, sizeof b, "[BOOT] stuck %.11s %c%s free %u:", t.name, st < 5 ? ST[st] : '?',
                 (t.st & 0x80) ? " (ipc)" : (t.st & 0x40) ? " (ipc timeout)" : "", (unsigned)t.free);
    for (uint8_t j = 0; j < t.n && j < 8 && k < (int)sizeof b; j++) k += snprintf(b + k, sizeof b - k, " %08x", (unsigned)t.pc[j]);
    evlog_write(String(b));
  }
}

void loop_guard_start() {
  s_alive = millis();
  if (s_tmr != nullptr) return;
  esp_timer_create_args_t a = {};
  a.callback = guard_cb;
  a.name = "loopguard";
  if (esp_timer_create(&a, &s_tmr) == ESP_OK) esp_timer_start_periodic(s_tmr, 1000000);
}
