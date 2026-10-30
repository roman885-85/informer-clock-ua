// Облік сокетів lwIP (нове, 30.10.2026, діагностика). Уночі на rc4/rc5 годинник щоразу через ~74 хв після запуску
// лишався без жодного вихідного з'єднання: socket() → errno 23 (ENFILE) — зайняті всі 10 місць сокетів lwIP. Облік
// за кілька хвилин показав причину: прямий lwip_close у цьому ядрі не звільняє місце (лише lwip_close_r, див.
// netfix.cpp) — так закривала сокет проба інтернету (з 1.0.0) і журнал на порту 23. Облік лишається: обгортки lwip_socket,
// lwip_accept(_r), lwip_close(_r) (-Wl,--wrap; close() ядра йде через VFS у lwip_close_r) пам'ятають для кожного місця,
// хто його відкрив чи закрив (5 адрес ланцюжка викликів, задача, етап головного циклу) і коли; закриття з помилкою —
// з errno. Щохвилини — перепис: видимі через API сокети (тип, порти, адреса), скільки місць вільно (пробні UDP-сокети,
// одразу закриті) і скільки «зниклих» (не видно й не вільне). Перепис — у журнал налагодження («[SOCK] …»), у ключ
// /content sock_diag, а перед перезапуском «немає сокетів» — у журнал подій. Адреси розшифровує addr2line з .elf збірки.
// EN: lwIP socket ledger (diagnostics): who opened/closed each of the 10 socket slots, plus a census every minute.
#include "fw.h"
#include "lwip/sockets.h"

extern "C" void xthal_window_spill(void);

#define SD_N CONFIG_LWIP_MAX_SOCKETS
#define SD_BT 5                         // адрес ланцюжка викликів над обгорткою
enum { SD_NONE, SD_OPEN, SD_CLOSED, SD_CLOSE_FAIL };

struct SdSlot { uint32_t bt[SD_BT], t; int16_t err; uint8_t st, stage; char task[5]; };
static SdSlot s_sd[SD_N];
static portMUX_TYPE s_sd_mux = portMUX_INITIALIZER_UNLOCKED;
static TaskHandle_t s_sd_quiet;         // задача, що зараз рахує вільні місця (її пробні сокети не записуються)
static char s_sd_txt[720];              // останній перепис — для /content (читає потік веб-сервера)
static uint32_t s_sd_t, s_sd_print;
static String s_sd_head;               // підсумок останнього рядка в журналі налагодження

static inline uint32_t sd_pc(uint32_t pc) {            // адреса повернення → адреса виклику (як у crash.cpp)
  if (pc & 0x80000000) pc = (pc & 0x3fffffff) | 0x40000000;
  return pc - 3;
}
static inline bool sd_sp_ok(uint32_t sp) { return sp >= 0x3ffae010 && sp <= 0x3ffffff0 && (sp & 0xf) == 0; }

// ланцюжок викликів того, хто покликав обгортку: вікна регістрів — у стек, далі кадри як у crash.cpp
static void __attribute__((noinline)) sd_trace(uint32_t *out) {
  uint32_t sp, next;
  xthal_window_spill();
  __asm__ __volatile__("mov %0, a1" : "=r"(sp));
  __asm__ __volatile__("mov %0, a0" : "=r"(next));
  int8_t n = -2;                                        // перші два кадри — sd_set і сама обгортка, пропускаємо
  while (n < SD_BT && sd_sp_ok(sp)) {
    uint32_t *base = (uint32_t *)sp, pc = next;
    next = base[-4];
    sp = base[-3];
    if (pc == 0) break;
    pc = sd_pc(pc);
    if (pc < 0x40000000 || pc >= 0x40400000) break;
    if (n >= 0) out[n] = pc;
    n++;
  }
  for (; n < SD_BT; n++) if (n >= 0) out[n] = 0;
}

static void __attribute__((noinline)) sd_set(int fd, uint8_t st, int err) {
  int i = fd - LWIP_SOCKET_OFFSET;
  if (i < 0 || i >= SD_N) return;
  if (s_sd_quiet && s_sd_quiet == xTaskGetCurrentTaskHandle()) return;
  uint32_t bt[SD_BT];
  sd_trace(bt);
  const char *tn = pcTaskGetTaskName(nullptr);
  uint32_t now = millis();
  portENTER_CRITICAL(&s_sd_mux);
  SdSlot &e = s_sd[i];
  memcpy(e.bt, bt, sizeof bt);
  e.t = now; e.err = (int16_t)err; e.st = st; e.stage = (uint8_t)rtc_stage;
  uint8_t k = 0;
  if (tn) for (; k < 4 && tn[k]; k++) e.task[k] = tn[k];
  e.task[k] = 0;
  portEXIT_CRITICAL(&s_sd_mux);
}

extern "C" {
int __real_lwip_socket(int domain, int type, int protocol);
int __wrap_lwip_socket(int domain, int type, int protocol) {
  int r = __real_lwip_socket(domain, type, protocol);
  if (r >= 0) sd_set(r, SD_OPEN, 0);
  return r;
}
int __real_lwip_accept(int s, struct sockaddr *a, socklen_t *l);
int __wrap_lwip_accept(int s, struct sockaddr *a, socklen_t *l) {
  int r = __real_lwip_accept(s, a, l);
  if (r >= 0) sd_set(r, SD_OPEN, 0);
  return r;
}
int __real_lwip_accept_r(int s, struct sockaddr *a, socklen_t *l);
int __wrap_lwip_accept_r(int s, struct sockaddr *a, socklen_t *l) {
  int r = __real_lwip_accept_r(s, a, l);
  if (r >= 0) sd_set(r, SD_OPEN, 0);
  return r;
}
// прямий lwip_close тут не звільняє місце сокета — будь-який такий виклик (і в майбутньому коді) веде до lwip_close_r
int __wrap_lwip_close(int s) { return lwip_close_r(s); }
// «закрито» — до справжнього закриття: після нього місце може одразу взяти інша задача
int __real_lwip_close_r(int s);
int __wrap_lwip_close_r(int s) {
  sd_set(s, SD_CLOSED, 0);
  int r = __real_lwip_close_r(s);
  if (r != 0) sd_set(s, SD_CLOSE_FAIL, errno);
  return r;
}
}

static void sd_bt_str(String &o, const uint32_t *bt) {
  char b[12];
  for (uint8_t k = 0; k < SD_BT && bt[k]; k++) { snprintf(b, sizeof b, "%s%x", k ? "<" : " @", (unsigned)bt[k]); o += b; }
}

// перепис: «used=видимих free=вільних lost=зниклих | місце тип :порт >адреса вік задача/етап @ланцюжок …»;
// free_probe — рахувати вільні місця пробними UDP-сокетами (лише з головного циклу)
static String sd_census(bool free_probe) {
  SdSlot cp[SD_N];
  portENTER_CRITICAL(&s_sd_mux);
  memcpy(cp, s_sd, sizeof cp);
  portEXIT_CRITICAL(&s_sd_mux);
  uint32_t now = millis();
  String body;
  uint8_t vis = 0;
  char b[48];
  for (uint8_t i = 0; i < SD_N; i++) {
    int fd = LWIP_SOCKET_OFFSET + i, type = 0;
    socklen_t l = sizeof type;
    bool v = lwip_getsockopt(fd, SOL_SOCKET, SO_TYPE, &type, &l) == 0;
    const SdSlot &e = cp[i];
    if (!v && e.st != SD_OPEN && e.st != SD_CLOSE_FAIL) continue;
    snprintf(b, sizeof b, " | %d ", fd);
    body += b;
    if (v) {
      vis++;
      body += type == SOCK_STREAM ? "T" : type == SOCK_DGRAM ? "U" : type == SOCK_RAW ? "R" : "?";
      struct sockaddr_in a;
      socklen_t al = sizeof a;
      if (lwip_getsockname(fd, (struct sockaddr *)&a, &al) == 0) { snprintf(b, sizeof b, " :%u", ntohs(a.sin_port)); body += b; }
      al = sizeof a;
      if (lwip_getpeername(fd, (struct sockaddr *)&a, &al) == 0) {
        uint32_t ip = a.sin_addr.s_addr;
        snprintf(b, sizeof b, " >%u.%u.%u.%u:%u", ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, ip >> 24, ntohs(a.sin_port));
        body += b;
      }
      if (e.st != SD_OPEN) body += " (no open)";       // відкрито не через обгортку або запис перезаписано
    } else body += e.st == SD_OPEN ? "LOST" : "CLOSEFAIL";
    if (e.st != SD_NONE) {
      snprintf(b, sizeof b, " %us %s/%u", (unsigned)((now - e.t) / 1000), e.task, e.stage);
      body += b;
      if (e.st == SD_CLOSE_FAIL) { snprintf(b, sizeof b, " err=%d", e.err); body += b; }
      sd_bt_str(body, e.bt);
    }
  }
  int fr = -1;
  if (free_probe) {
    int fds[SD_N];
    fr = 0;
    s_sd_quiet = xTaskGetCurrentTaskHandle();
    while (fr < SD_N) { int f = lwip_socket(AF_INET, SOCK_DGRAM, 0); if (f < 0) break; fds[fr++] = f; }
    for (int k = 0; k < fr; k++) lwip_close_r(fds[k]);
    s_sd_quiet = nullptr;
  }
  String s = String("used=") + vis + String(" free=") + (fr < 0 ? String("?") : String(fr));
  if (fr >= 0) s += String(" lost=") + (SD_N - vis - fr);
  return s + body;
}

// щохвилини; пробні сокети для ліку вільних місць — лише з налагодженням по Wi-Fi; у журнал налагодження — коли
// змінився підсумок (used/free/lost) або раз на 10 хв
void fx_sock_tick() {
  if (s_sd_t != 0 && millis() - s_sd_t < 60000) return;
  s_sd_t = millis();
  String s = sd_census(g_fx_wdbg);
  int bar = s.indexOf(" |");
  String head = bar < 0 ? s : s.substring(0, bar);
  if (head != s_sd_head || s_sd_t - s_sd_print >= 600000) {
    s_sd_head = head;
    s_sd_print = s_sd_t;
    dbg_ws_print(String("[SOCK] ") + s);
  }
  portENTER_CRITICAL(&s_sd_mux);
  strlcpy(s_sd_txt, s.c_str(), sizeof s_sd_txt);
  portEXIT_CRITICAL(&s_sd_mux);
}

void fx_sock_diag(String &out) {
  char b[sizeof s_sd_txt];
  portENTER_CRITICAL(&s_sd_mux);
  memcpy(b, s_sd_txt, sizeof b);
  portEXIT_CRITICAL(&s_sd_mux);
  b[sizeof b - 1] = 0;
  out = b;
}

String fx_sock_census_now() { return sd_census(false); }
