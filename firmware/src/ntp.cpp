// Власний клієнт NTP з перевірками відповіді й правдоподібності часу (нове, 1.1.6; не з оригіналу).
// EN: Own NTP client (new in 1.1.6): RFC 4330/5905 reply checks, a second server confirms any jump of a known time,
// rejected replies go to the event log.
//
// Чому (14.11.2026, 1.1.5): годинник щонайменше кілька хвилин показував час на 12 год 23 хв 42 с назад (та сама дата) і
// сам виправився наступною синхронізацією. Розбір машинного коду 1.1.5 (ЖУРНАЛ.md годинника, 14.11):
//  - системний час ESP32 у прошивці ставить лише клієнт SNTP ядра 1.0.4 (lwIP: sntp_recv → sntp_process →
//    settimeofday), а він зібраний з SNTP_CHECK_RESPONSE 0: бере будь-який пакет довжиною 48 Б з режимом 4 на свій порт —
//    від будь-якої адреси, без звірки мітки запиту (originate), без перевірки LI = 3 («сервер не синхронізований») і
//    страти 16 (відкидає лише страту 0 — «поцілунок смерті»): один сервер пулу з хибним годинником = хибний час;
//  - ntp_request() 1.1.0–1.1.5 чекав свіжої відповіді лише на першому проході: далі t = time(nullptr) щоразу, тож через
//    100 мс у TimeLib ішов системний час, навіть якщо сервер ще не відповів;
//  - другий сервер автора time.nist.gov не використовувався ніколи: ядро тримає один сервер SNTP
//    (CONFIG_LWIP_DHCP_MAX_NTP_SERVERS 1, sntp_setservername(1, …) відкидається).
// Тепер SNTP ядра не запускається зовсім (configTime не викликається): системний час ставить лише fx_ntp_sync() — після
// перевірок, а стрибок відомого часу понад FX_NTP_JUMP_S спершу підтверджує другий, незалежний сервер.
#include "fw.h"
#include "lwip/sockets.h"

// запасні сервери й друга думка: різні мережі (anycast Cloudflare, Google) і ще один випадковий сервер пулу
static const char *const NTP_ALT[] = { "time.cloudflare.com", "time.google.com", "1.pool.ntp.org" };
#define NTP_ALT_N (sizeof NTP_ALT / sizeof NTP_ALT[0])
#define NTP_UNIX_1900 2208988800UL     // секунд від 1900 (епоха NTP) до 1970 (Unix)

static uint32_t be32(const uint8_t *p) { return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) | ((uint32_t)p[2] << 8) | p[3]; }

// Розбір відповіді на наш запит (чиста функція — її перевіряє імітатор tools/sim/ntpsim).
// -1 — не відповідь на наш запит (коротка, чужа мітка запиту): ігнорувати й чекати далі;
//  0 — відповідь наша, але час із неї брати не можна (out.why — чому); 1 — придатна (out.utc/us — час відправлення).
int8_t fx_ntp_parse(const uint8_t *p, size_t n, const uint8_t *req_tx, FxNtpReply &out) {
  static char kod[12];
  if (n < 48) return -1;
  if (memcmp(p + 24, req_tx, 8) != 0) return -1;     // originate ≠ наш transmit: запізніла чи чужа відповідь
  out.why = "";
  out.li = p[0] >> 6;
  out.vn = (p[0] >> 3) & 7;
  out.mode = p[0] & 7;
  out.stratum = p[1];
  uint32_t rdelay = be32(p + 4), rdisp = be32(p + 8);   // 16.16, секунди
  uint32_t ref = be32(p + 16), sec = be32(p + 40), frac = be32(p + 44);
  // відстань до еталона (як у ntpd: root delay / 2 + root dispersion), мс; від'ємна затримка — «нескінченність»
  uint64_t dist = (rdelay & 0x80000000UL) ? 0xffffffffULL : ((uint64_t)rdelay * 500 >> 16) + ((uint64_t)rdisp * 1000 >> 16);
  out.dist_ms = dist > 0xffffffffULL ? 0xffffffffUL : (uint32_t)dist;
  out.utc = sec - NTP_UNIX_1900;
  out.us = (uint32_t)(((uint64_t)frac * 1000000ULL) >> 32);
  out.has_time = sec != 0 && out.stratum != 0;         // є з чим порівняти годинник (для журналу відкинутих)
  if (out.mode != 4) { out.why = "mode"; return 0; }
  if (out.stratum == 0) {                               // «поцілунок смерті»: сервер просить не питати (код у refid)
    snprintf(kod, sizeof kod, "KoD %c%c%c%c", isprint(p[12]) ? p[12] : '?', isprint(p[13]) ? p[13] : '?',
             isprint(p[14]) ? p[14] : '?', isprint(p[15]) ? p[15] : '?');
    out.why = kod;
    return 0;
  }
  if (out.li == 3) { out.why = "LI=3 (unsynchronized)"; return 0; }
  if (out.stratum > 15) { out.why = "stratum>15 (unsynchronized)"; return 0; }
  if (out.vn < 1 || out.vn > 4) { out.why = "version"; return 0; }
  if (sec == 0) { out.why = "no transmit time"; return 0; }
  if (ref == 0) { out.why = "never synchronized"; return 0; }
  if (out.dist_ms > FX_NTP_MAXDIST_MS) { out.why = "root distance"; return 0; }
  if ((int32_t)(sec - ref) > FX_NTP_REF_AGE_S) { out.why = "stale reference"; return 0; }   // еталон давно не оновлювався
  return 1;
}

// ---------- мережа ----------
static uint32_t s_foreign_log_ms;                       // коли востаннє писали в журнал подій про чужі пакети
// Запит одному серверу. 1 — придатна відповідь (r), 0 — сервер відповів, але відкинуто (r.why), -1 — немає відповіді.
// avoid_ip ≠ 0 — друга думка: сервер з тієї самої мережі /16, що й перший, не питаємо — це не незалежна перевірка
// (14.11, на годиннику: пул дав 162.159.200.123, а «другим» став time.cloudflare.com = 162.159.200.1 — той самий Cloudflare).
static int8_t ntp_ask(const char *host, uint32_t avoid_ip, FxNtpReply &r) {
  memset(&r, 0, sizeof r);
  r.why = "";
  r.host = host;
  IPAddress ip;
  if (!WiFi.hostByName(host, ip)) { r.why = "DNS"; return -1; }
  r.ip = (uint32_t)ip;
  if (avoid_ip != 0 && (r.ip & 0xffff) == (avoid_ip & 0xffff)) { r.why = "same network as the first server"; return -1; }   // перші два октети
  int s = lwip_socket(AF_INET, SOCK_DGRAM, 0);
  if (s < 0) { r.why = "socket"; return -1; }
  uint8_t req[48], buf[68];
  memset(req, 0, sizeof req);
  req[0] = 0x23;                                        // LI 0, версія 4, режим 3 (клієнт)
  uint32_t n1 = esp_random(), n2 = esp_random();        // мітка запиту — випадкова: відповідь мусить повернути її
  memcpy(req + 40, &n1, 4);
  memcpy(req + 44, &n2, 4);
  struct sockaddr_in to;
  memset(&to, 0, sizeof to);
  to.sin_family = AF_INET;
  to.sin_port = htons(123);
  to.sin_addr.s_addr = r.ip;
  int8_t res = -1;
  uint32_t t1 = millis();
  if (lwip_sendto(s, req, sizeof req, 0, (struct sockaddr *)&to, sizeof to) == (int)sizeof req) {
    r.why = "timeout";
    for (;;) {
      int32_t left = (int32_t)FX_NTP_REPLY_MS - (int32_t)(millis() - t1);
      if (left <= 0) break;
      fd_set rd;
      FD_ZERO(&rd);
      FD_SET(s, &rd);
      struct timeval tv;
      tv.tv_sec = left / 1000;
      tv.tv_usec = (left % 1000) * 1000;
      if (lwip_select(s + 1, &rd, nullptr, nullptr, &tv) <= 0) break;
      struct sockaddr_in from;
      socklen_t fl = sizeof from;
      int n = lwip_recvfrom(s, buf, sizeof buf, MSG_DONTWAIT, (struct sockaddr *)&from, &fl);
      if (n <= 0) continue;
      if (from.sin_addr.s_addr != r.ip || from.sin_port != htons(123)) { r.foreign++; continue; }   // чужа адреса
      int8_t k = fx_ntp_parse(buf, (size_t)n, req + 40, r);
      if (k < 0) { r.foreign++; continue; }             // не відповідь на цей запит — чекаємо далі
      res = k;
      break;
    }
  } else r.why = "send";
  r.rtt_ms = millis() - t1;
  r.ms = millis();
  lwip_close_r(s);                                      // не lwip_close: у ядрі 1.0.4 той не звільняє місце (netfix.cpp)
  if (res == 1) {                                       // час у мить приходу ≈ час відправлення + половина кола
    uint32_t us = r.us + (r.rtt_ms / 2) * 1000;
    r.utc += us / 1000000;
    r.us = us % 1000000;
  }
  if (r.foreign) {
    String m = String("NTP ") + host + ": " + String(r.foreign) + " stray packet(s) on the NTP socket ignored";
    dbg_ws_print(m);
    if (s_foreign_log_ms == 0 || millis() - s_foreign_log_ms > 3600000UL) { s_foreign_log_ms = millis(); evlog_write(String("[TIME] ") + m); }
  }
  return res;
}

// ---------- стан для діагностики (/content?ntp_diag=) ----------
static portMUX_TYPE s_diag_mux = portMUX_INITIALIZER_UNLOCKED;
static char s_diag_ok[112], s_diag_rej[192];
static bool s_pend;                                     // стрибок, якого не підтвердив другий сервер
static int32_t s_pend_d;
static uint32_t s_pend_ms;

static String ip_text(uint32_t ip) { return IPAddress(ip).toString(); }
static String reply_text(const FxNtpReply &r) {
  return String(r.host) + " " + ip_text(r.ip) + " LI=" + String(r.li) + " st=" + String(r.stratum) + " dist=" +
         String(r.dist_ms) + "ms rtt=" + String(r.rtt_ms) + "ms";
}
static void diag_set(char *dst, size_t cap, const String &s) {
  portENTER_CRITICAL(&s_diag_mux);
  strncpy(dst, s.c_str(), cap - 1);
  dst[cap - 1] = 0;
  portEXIT_CRITICAL(&s_diag_mux);
}
void fx_ntp_diag(String &out) {                         // будь-яка задача (веб — потік AsyncTCP)
  char a[sizeof s_diag_ok], b[sizeof s_diag_rej];
  portENTER_CRITICAL(&s_diag_mux);
  memcpy(a, s_diag_ok, sizeof a);
  memcpy(b, s_diag_rej, sizeof b);
  portEXIT_CRITICAL(&s_diag_mux);
  out = String("ok: ") + (a[0] ? a : "-") + "; rejected: " + (b[0] ? b : "-");
}

// UTC, який зараз показує годинник (TimeLib у місцевому часі мінус застосований пояс); 0 — час ще невідомий
static uint32_t clock_utc() {
  if (!fx_time_known() || !g_fx_off_known) return 0;
  return (uint32_t)(now() - g_fx_off_applied);
}
// різниця «сервер − годинник», с, у мить приходу відповіді r (показ годинника взято в clk_ms: clk_utc)
static int32_t vs_clock(const FxNtpReply &r, uint32_t clk_ms, uint32_t clk_utc) {
  return (int32_t)(r.utc - clk_utc) - (int32_t)((r.ms - clk_ms) / 1000);
}
static String signed_s(int32_t d) { return String(d > 0 ? "+" : "") + String((long)d) + " s"; }
// відкинута відповідь — у діагностику; у журнал подій — лише коли вона змінила б показаний час більш ніж на хвилину.
// Той самий хибний сервер щозвірки (напр. заданий у налаштуваннях) не має витіснити з журналу (4 КБ) усе інше: та сама
// похибка (±5 хв) — не частіше разу на 6 год, інша — одразу.
static uint32_t s_rej_log_ms;
static int32_t s_rej_log_d;
static bool s_rej_logged;
static void log_rejected(const FxNtpReply &r, int32_t d, bool known, const String &why) {
  String m = reply_text(r) + ": " + why + (known ? String(", server-clock ") + signed_s(d) : String(""));
  diag_set(s_diag_rej, sizeof s_diag_rej, fx_time_text(now()) + " " + m);
  dbg_ws_print(String("NTP        : rejected ") + m);
  if (!known || (d <= 60 && d >= -60)) return;
  if (s_rej_logged && millis() - s_rej_log_ms < 6 * 3600000UL && d - s_rej_log_d <= 300 && s_rej_log_d - d <= 300) return;
  s_rej_logged = true;
  s_rej_log_ms = millis();
  s_rej_log_d = d;
  evlog_write(String("[TIME] NTP rejected ") + m);
}

// Синхронізація. 1 — час прийнято (utc — секунди UTC; системний час ESP32 поставлено; src — звідки, для журналу подій);
// 0 — відповідь є, але час не міняти (відкинуто, у журналі — чому); -1 — жоден сервер не відповів.
static uint32_t s_sets;                                 // скільки разів системний час ставила ця функція (лічильник netfix.cpp)
int8_t fx_ntp_sync(const char *srv, uint32_t &utc, String &src) {
  uint32_t t0 = millis();
  if (g_fx_sntp_n != s_sets) {                          // сигналізація: системний час поставив хтось інший (не має бути)
    evlog_write(String("[TIME] system clock was set outside the NTP check (") + String(g_fx_sntp_n - s_sets) + " times)");
    s_sets = g_fx_sntp_n;
  }
  uint32_t clk_ms = millis(), clk = clock_utc();        // показаний час на початку
  bool known = clk != 0;
  FxNtpReply a, b;
  int8_t ra = ntp_ask(srv, 0, a);
  int8_t any = ra;                                      // чи відповів хоч один сервер
  size_t ai = 0;
  // основний не відповів або відкинутий — запасні (по черзі, у межах FX_NTP_BUDGET_MS)
  while (ra != 1) {
    if (ra == 0) log_rejected(a, known && a.has_time ? vs_clock(a, clk_ms, clk) : 0, known && a.has_time, String(a.why));
    else dbg_ws_print(String("NTP        : ") + a.host + " - " + a.why);
    if (ai >= NTP_ALT_N || millis() - t0 > FX_NTP_BUDGET_MS) return any < 0 ? -1 : 0;
    ra = ntp_ask(NTP_ALT[ai++], 0, a);
    if (ra > any) any = ra;
  }
  int32_t d = known ? vs_clock(a, clk_ms, clk) : 0;
  const FxNtpReply *use = &a;
  src = String("NTP ") + reply_text(a);
  if (known && (d > FX_NTP_JUMP_S || d < -FX_NTP_JUMP_S)) {
    // стрибок відомого часу: спершу друга думка незалежного сервера (інша адреса)
    int8_t rb = -1;
    while (rb != 1 && ai < NTP_ALT_N && millis() - t0 <= FX_NTP_BUDGET_MS) {
      rb = ntp_ask(NTP_ALT[ai++], a.ip, b);
      if (rb == 0) log_rejected(b, b.has_time ? vs_clock(b, clk_ms, clk) : 0, b.has_time, String(b.why));
    }
    if (rb == 1) {
      int32_t ab = (int32_t)(b.utc - a.utc) - (int32_t)((b.ms - a.ms) / 1000);   // b проти a в ту саму мить
      int32_t db = vs_clock(b, clk_ms, clk);
      if (ab <= FX_NTP_AGREE_S && ab >= -FX_NTP_AGREE_S) {
        use = &b;                                       // згодні: час — від свіжішої відповіді
        src += String(", confirmed by ") + b.host + " " + ip_text(b.ip);
      } else if (db <= FX_NTP_JUMP_S && db >= -FX_NTP_JUMP_S) {
        log_rejected(a, d, true, String("second server ") + b.host + " " + ip_text(b.ip) + " agrees with the clock");
        use = &b;                                       // другий сервер справний — беремо його (поправка до хвилини)
        src = String("NTP ") + reply_text(b);
      } else {
        log_rejected(a, d, true, String("second server ") + b.host + " " + ip_text(b.ip) + " differs too (" + signed_s(db) +
                     "), time kept");
        return 0;
      }
    } else if (s_pend && millis() - s_pend_ms < 3600000UL && d - s_pend_d <= FX_NTP_AGREE_S && s_pend_d - d <= FX_NTP_AGREE_S) {
      src += String(", confirmed by repeat");          // другого сервера немає, але той самий стрибок удруге поспіль
    } else {
      s_pend = true;
      s_pend_d = d;
      s_pend_ms = millis();
      log_rejected(a, d, true, String("no second server to confirm, time kept until next sync"));
      return 0;
    }
  }
  s_pend = false;
  uint32_t us = use->us + (millis() - use->ms) * 1000;  // скільки минуло від приходу відповіді
  struct timeval tv;
  tv.tv_sec = use->utc + us / 1000000;
  tv.tv_usec = us % 1000000;
  settimeofday(&tv, nullptr);                           // системний час (переживає програмний перезапуск)
  s_sets = g_fx_sntp_n;
  utc = (uint32_t)tv.tv_sec + (tv.tv_usec >= 500000 ? 1 : 0);   // TimeLib — цілі секунди: до найближчої
  time_t loc = (time_t)utc + fx_tz_offset() + fx_dst_offset((time_t)utc);   // новий місцевий час — для діагностики
  diag_set(s_diag_ok, sizeof s_diag_ok, fx_time_text(loc) + " " + use->host + " " + ip_text(use->ip) + " st=" +
           String(use->stratum) + " rtt=" + String(use->rtt_ms) + "ms" + (use == &b ? ", 2nd server" : ""));
  return 1;
}
