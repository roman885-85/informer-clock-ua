// Модель світу для імітатора часу (tools/sim/ntpsim). Див. world.h.
#include "fw.h"
#include "lwip/sockets.h"
#include "world.h"
#include <map>
#include <deque>
#include <algorithm>

bool sim_verbose = true;
static uint64_t W_ms;                  // millis() від «увімкнення»
static int64_t W_true0_us;             // справжній UTC у мить W_ms = 0
static int64_t W_sys0_us;              // системний час ESP32: W_sys0_us + W_ms·1000 (0 — не поставлено, 1970 р.)
volatile uint32_t g_fx_sntp_n;         // у прошивці — netfix.cpp: обгортка settimeofday рахує встановлення
static std::vector<std::string> W_evlog;
static uint32_t W_rng = 0x2545f491;

uint32_t millis() { return (uint32_t)W_ms; }
void yield() {}
uint32_t esp_random() { W_rng ^= W_rng << 13; W_rng ^= W_rng >> 17; W_rng ^= W_rng << 5; return W_rng; }
int64_t sim_true_us() { return W_true0_us + (int64_t)W_ms * 1000; }
int64_t sim_sys_us() { return W_sys0_us + (int64_t)W_ms * 1000; }
time_t sim_time(time_t *p) {
  time_t t = (time_t)(sim_sys_us() / 1000000);
  if (p) *p = t;
  return t;
}
int sim_settimeofday(const struct timeval *tv, const void *) {
  W_sys0_us = (int64_t)tv->tv_sec * 1000000 + tv->tv_usec - (int64_t)W_ms * 1000;
  g_fx_sntp_n = g_fx_sntp_n + 1;
  return 0;
}
uint32_t sim_ip(uint8_t a, uint8_t b, uint8_t c, uint8_t d) { return (uint32_t)IPAddress(a, b, c, d); }

static String loc_text() {             // справжній місцевий час (пояс +2) — для подій імітатора
  int64_t us = sim_true_us() + 7200LL * 1000000;
  tmElements_t e;
  breakTime((time_t)(us / 1000000), e);
  char b[24];
  snprintf(b, sizeof b, "%02u:%02u:%02u.%03u", e.Hour, e.Minute, e.Second, (unsigned)(us / 1000 % 1000));
  return String(b);
}
void dbg_ws_print(const String &s) { if (sim_verbose) printf("      %s  налагодження: %s\n", loc_text().c_str(), s.c_str()); }
void evlog_write(const String &msg) {
  W_evlog.push_back(std::string(msg.c_str()));
  if (sim_verbose) printf("      %s  ЖУРНАЛ ПОДІЙ: %s\n", loc_text().c_str(), msg.c_str());
}
std::vector<std::string> &sim_evlog() { return W_evlog; }

// ---------- сервери й DNS ----------
static std::map<uint32_t, SimSrv> W_srv;
static std::map<std::string, uint32_t> W_dns;
void sim_server(uint32_t ip, const SimSrv &s) { W_srv[ip] = s; }
void sim_dns(const char *name, uint32_t ip) { W_dns[name] = ip; }
SimWiFi WiFi;
int SimWiFi::hostByName(const char *host, IPAddress &out) {
  world_advance(2);                    // DNS роутера (кеш)
  auto it = W_dns.find(host);
  if (it == W_dns.end() || it->second == 0) { out = (uint32_t)0; return 0; }
  out = it->second;
  return 1;
}

static void put32(uint8_t *p, uint32_t v) { p[0] = v >> 24; p[1] = v >> 16; p[2] = v >> 8; p[3] = v; }
static uint32_t get32(const uint8_t *p) { return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3]; }
// відповідь сервера s на запит з міткою req_tx (8 Б), зібрана в мить відправлення (справжній час t_us)
static std::vector<uint8_t> build_reply(const SimSrv &s, const uint8_t *req_tx, int64_t t_us) {
  std::vector<uint8_t> o(48, 0);
  o[0] = (uint8_t)((s.li << 6) | (s.vn << 3) | 4);
  o[1] = s.kod ? 0 : s.stratum;
  o[2] = 6;
  o[3] = 0xe9;
  put32(&o[4], s.rdelay_ms * 65536u / 1000);
  put32(&o[8], s.rdisp_ms * 65536u / 1000);
  if (s.kod) memcpy(&o[12], s.kod, 4);
  else memcpy(&o[12], "SIM", 3);
  int64_t st = t_us + (int64_t)s.off_s * 1000000;   // годинник сервера
  uint32_t sec = (uint32_t)(st / 1000000) + 2208988800u;
  uint32_t frac = (uint32_t)(((uint64_t)(st % 1000000) << 32) / 1000000);
  put32(&o[16], sec - s.ref_age_s);
  memcpy(&o[24], req_tx, 8);
  put32(&o[32], sec); put32(&o[36], frac);
  put32(&o[40], sec); put32(&o[44], frac);
  return o;
}

// ---------- події (прихід пакетів, повтори SNTP) ----------
struct Ev { uint64_t at; int kind; int dst; uint32_t src_ip; uint16_t src_port; std::vector<uint8_t> pkt; };
enum { EV_PCB = 1, EV_SOCK = 2, EV_SNTP_RETRY = 3 };
static std::vector<Ev> W_ev;
static void ev_add(const Ev &e) { W_ev.push_back(e); }

// ---------- клієнт SNTP ядра 1.0.4 (модель за машинним кодом 1.1.5: sntp_recv @401b2170, sntp_process @401b2140) ----------
static int W_pcb_gen;                  // поточний pcb клієнта SNTP; sntp_stop знищує його — відповіді старому губляться
static std::string W_sntp_name;
static uint32_t W_sntp_retry_ms = 3000;
static void sntp_request_model() {
  IPAddress ip;
  if (!WiFi.hostByName(W_sntp_name.c_str(), ip)) {          // sntp_request: адреса невідома — повтор через 3 с
    ev_add({W_ms + 3000, EV_SNTP_RETRY, W_pcb_gen, 0, 0, {}});
    return;
  }
  auto it = W_srv.find((uint32_t)ip);
  if (it != W_srv.end() && it->second.alive) {
    const SimSrv &s = it->second;
    uint8_t zero[8] = {0};             // sntp_initialize_request: memset 0 + 0x23 — мітка запиту нульова
    std::vector<uint8_t> r = build_reply(s, zero, sim_true_us() + (int64_t)s.rtt_ms * 500);
    ev_add({W_ms + s.rtt_ms, EV_PCB, W_pcb_gen, (uint32_t)ip, 123, r});
  }
  // тайм-аут відповіді 3 с → sntp_try_next_server: сервер лише один (SNTP_MAX_SERVERS 1) → sntp_retry (подвоєння)
  ev_add({W_ms + 3000, EV_SNTP_RETRY, W_pcb_gen, 0, 0, {}});
}
static void sntp_recv_model(const std::vector<uint8_t> &p) {
  // перевірки — рівно ті, що в машинному коді: довжина 48, режим 4 (опитування), страта ≠ 0. Адреса, порт, мітка
  // запиту, LI, страта 16 — не перевіряються (SNTP_CHECK_RESPONSE 0).
  if (p.size() != 48) return;
  if ((p[0] & 7) != 4) return;
  if (p[1] == 0) return;                                      // KoD: sntp_try_next_server
  uint32_t sec = get32(&p[40]) - 2208988800u;                 // sntp_process: + 0x7c558180 (mod 2^32)
  uint32_t us = (uint32_t)(((uint64_t)get32(&p[44]) * 1000000ULL) >> 32);
  struct timeval tv;
  tv.tv_sec = sec;
  tv.tv_usec = us;
  sim_settimeofday(&tv, nullptr);                            // SNTP_SET_SYSTEM_TIME_US → settimeofday (обгортка)
  W_ev.erase(std::remove_if(W_ev.begin(), W_ev.end(), [](const Ev &e) { return e.kind == EV_SNTP_RETRY; }), W_ev.end());
}
void configTime(long, int, const char *server1, const char *server2, const char *server3) {
  (void)server2; (void)server3;        // sntp_setservername(1/2, …) у ядрі відкидається: SNTP_MAX_SERVERS 1
  W_pcb_gen++;                         // sntp_stop(): udp_remove(pcb)
  W_ev.erase(std::remove_if(W_ev.begin(), W_ev.end(), [](const Ev &e) { return e.kind == EV_SNTP_RETRY; }), W_ev.end());
  W_sntp_name = server1;               // ім'я тримається вказівником; тут — копія
  W_sntp_retry_ms = 3000;
  sntp_request_model();                // sntp_init() → sntp_request() одразу, із задачі циклу
}
void sim_old_inject_foreign(int32_t off_s) {   // чужий пакет режиму 4 на поточний порт клієнта SNTP
  SimSrv s;
  s.off_s = off_s;
  uint8_t zero[8] = {0};
  ev_add({W_ms + 1, EV_PCB, W_pcb_gen, sim_ip(192, 0, 2, 99), 123, build_reply(s, zero, sim_true_us())});
}

// ---------- сокети lwIP (власний клієнт 1.1.6) ----------
struct Sock { bool open; std::deque<Ev> rx; };
static std::map<int, Sock> W_sock;
static int W_next_sock = 54;
int lwip_socket(int, int, int) { int s = W_next_sock++; W_sock[s].open = true; return s; }
int lwip_close_r(int s) { W_sock.erase(s); return 0; }
int lwip_sendto(int s, const void *data, size_t size, int, const struct sockaddr *to, socklen_t) {
  if (size != 48) return -1;
  const struct sockaddr_in *a = (const struct sockaddr_in *)to;
  uint32_t ip = a->sin_addr.s_addr;
  const uint8_t *req = (const uint8_t *)data;
  auto it = W_srv.find(ip);
  if (it == W_srv.end() || !it->second.alive) return (int)size;   // відправили — відповіді не буде
  const SimSrv &sv = it->second;
  for (int i = 0; i < sv.stray_foreign; i++) {                   // чужа адреса, режим 4, хибний час, будь-яка мітка
    SimSrv f;
    f.off_s = -44622;
    uint8_t junk[8] = {1, 2, 3, 4, 5, 6, 7, (uint8_t)i};
    ev_add({W_ms + 3 + i, EV_SOCK, s, sim_ip(192, 0, 2, 99), 123, build_reply(f, junk, sim_true_us())});
  }
  for (int i = 0; i < sv.stray_stale; i++) {                     // та сама адреса, мітка попереднього запиту
    SimSrv f = sv;
    f.off_s = -44622;
    uint8_t old[8] = {9, 9, 9, 9, 9, 9, 9, (uint8_t)i};
    ev_add({W_ms + 4 + i, EV_SOCK, s, ip, 123, build_reply(f, old, sim_true_us())});
  }
  ev_add({W_ms + sv.rtt_ms, EV_SOCK, s, ip, 123, build_reply(sv, req + 40, sim_true_us() + (int64_t)sv.rtt_ms * 500)});
  return (int)size;
}
static int sock_in(fd_set *rd, int maxfd) {
  for (int s = 0; s < maxfd; s++) if (FD_ISSET(s, rd)) return s;
  return -1;
}
int lwip_select(int maxfdp1, fd_set *readset, fd_set *, fd_set *, struct timeval *tv) {
  int s = sock_in(readset, maxfdp1);
  uint64_t deadline = W_ms + (uint64_t)tv->tv_sec * 1000 + tv->tv_usec / 1000;
  for (;;) {
    auto it = W_sock.find(s);
    if (it != W_sock.end() && !it->second.rx.empty()) return 1;
    uint64_t next = UINT64_MAX;
    for (auto &e : W_ev) next = std::min(next, e.at);
    if (next > deadline) { W_ms = deadline; FD_ZERO(readset); return 0; }
    world_advance((uint32_t)(next - W_ms));
  }
}
int lwip_recvfrom(int s, void *mem, size_t len, int, struct sockaddr *from, socklen_t *fromlen) {
  auto it = W_sock.find(s);
  if (it == W_sock.end() || it->second.rx.empty()) return -1;
  Ev e = it->second.rx.front();
  it->second.rx.pop_front();
  size_t n = std::min(len, e.pkt.size());
  memcpy(mem, e.pkt.data(), n);
  struct sockaddr_in *a = (struct sockaddr_in *)from;
  memset(a, 0, sizeof *a);
  a->sin_family = AF_INET;
  a->sin_port = htons(e.src_port);
  a->sin_addr.s_addr = e.src_ip;
  *fromlen = sizeof *a;
  return (int)n;
}

// ---------- плин часу ----------
void world_advance(uint32_t ms) {
  uint64_t end = W_ms + ms;
  for (;;) {
    size_t k = SIZE_MAX;
    for (size_t i = 0; i < W_ev.size(); i++)
      if (W_ev[i].at <= end && (k == SIZE_MAX || W_ev[i].at < W_ev[k].at)) k = i;
    if (k == SIZE_MAX) break;
    Ev e = W_ev[k];
    W_ev.erase(W_ev.begin() + k);
    if (e.at > W_ms) W_ms = e.at;
    if (e.kind == EV_PCB) { if (e.dst == W_pcb_gen) sntp_recv_model(e.pkt); }
    else if (e.kind == EV_SOCK) { auto it = W_sock.find(e.dst); if (it != W_sock.end()) it->second.rx.push_back(e); }
    else if (e.kind == EV_SNTP_RETRY && e.dst == W_pcb_gen) {
      W_sntp_retry_ms = std::min<uint32_t>(W_sntp_retry_ms * 2, 30000);
      sntp_request_model();
    }
  }
  W_ms = end;
}
void delay(uint32_t ms) { world_advance(ms); }

// ---------- глобальні змінні прошивки ----------
uint32_t g_c18a0, g_c18a4, g_c18a8, g_c22b8, g_c2ed4, g_c2edc, g_c3458, g_c3464;
uint8_t g_c18ac, g_c18ad, g_c22b4, g_c22b5, g_c22bc, g_c22bd, g_c22be, g_c22bf, g_c22c0, g_c2d29, g_c2eb4, g_c2ed8,
    g_c2ed9, g_c2ee0, g_c2ee1, g_c32e4, g_c3319, g_c342c, g_c345c, g_c345d, g_c345e, g_c345f, g_c3460, g_c3461, g_c3468;
String g_c22c4, g_c22d0, g_c2ebc, g_c2ec8, g_c33fc, g_c3408, g_c3414, g_c3434, g_c3440, g_c344c;
float g_c3430;
RTC_DS3231 g_c346c;

void sim_reset(int64_t true_utc_us_at_boot, bool sys_time_known) {
  W_ms = 0;
  W_true0_us = true_utc_us_at_boot;
  W_sys0_us = sys_time_known ? true_utc_us_at_boot : 0;
  W_ev.clear();
  W_sock.clear();
  W_evlog.clear();
  W_srv.clear();
  W_dns.clear();
  W_pcb_gen++;
  setTime(0);                          // TimeLib з нуля (prevMillis = millis() = 0)
}
