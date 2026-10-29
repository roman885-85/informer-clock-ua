// Налагодження по Wi-Fi (нове, не з оригіналу) — вмикається у веб-інтерфейсі (сторінка System, ключ wdbg).
// EN: Wi-Fi debugging (new, switch on the System page): network log on TCP 23 and firmware upload over Wi-Fi (OTA).
// Задача: налагодження по Wi-Fi замість постійного USB; функція вмикається й вимикається у веб-інтерфейсі.
// 1) Сетевий журнал, TCP-порт 23 (до 2 клієнтів; напр. `nc <IP-годинника> 23`): рядки налагодження автора (як у живій
//    консолі веб-інтерфейсу), помилки ядра Arduino (log_e…) й ESP-IDF; при під'єднанні — шапка (версія, час роботи,
//    пам'ять, причина старту, остання аварія) і останні ~8 КБ журналу. Не займає єдине WebSocket-з'єднання
//    веб-інтерфейсу; запис з Mac на всю ніч — `nc <IP-годинника> 23 >> clock.log`.
// 2) Прошивка по Wi-Fi (ArduinoOTA, UDP 3232): `pio run -e clock -t upload --upload-port <IP-годинника>`, пароль —
//    пароль веб-інтерфейсу (AuP), якщо його задано.
// Вимкнено — нічого з цього не працює (порти закриті, рядки не збираються).
#include "fw.h"
#include <ArduinoOTA.h>
#include "lwip/sockets.h"

bool g_fx_wdbg;
static bool s_up;                       // сервер журналу й OTA запущено
static int s_lfd = -1, s_cfd[2] = { -1, -1 };
static uint32_t s_rd[2];                // позиція читання кожного клієнта (наскрізна)
static char s_ring[8192];
static volatile uint32_t s_wr;          // наскрізна позиція запису
static portMUX_TYPE s_mux = portMUX_INITIALIZER_UNLOCKED;
static uint32_t s_t_ota;

void fx_wdbg_line(const char *s, size_t n) {
  if (!g_fx_wdbg || n == 0) return;
  char ts[24];
  int k;
  uint32_t ms = millis();
  if (fx_time_known()) k = snprintf(ts, sizeof ts, "%02d:%02d:%02d.%03u ", hour(), minute(), second(), (unsigned)(ms % 1000));
  else k = snprintf(ts, sizeof ts, "+%u.%03u ", (unsigned)(ms / 1000), (unsigned)(ms % 1000));
  while (n && (s[n - 1] == '\n' || s[n - 1] == '\r')) n--;
  if (n > 1024) n = 1024;
  portENTER_CRITICAL(&s_mux);
  uint32_t w = s_wr;
  for (int i = 0; i < k; i++) s_ring[(w++) & (sizeof s_ring - 1)] = ts[i];
  for (size_t i = 0; i < n; i++) s_ring[(w++) & (sizeof s_ring - 1)] = s[i];
  s_ring[(w++) & (sizeof s_ring - 1)] = '\r';
  s_ring[(w++) & (sizeof s_ring - 1)] = '\n';
  s_wr = w;
  portEXIT_CRITICAL(&s_mux);
}

// помилки ядра Arduino (log_e/log_w…): у порт, як у ядрі (ets_printf), і в сетевий журнал.
// Своя реалізація замість ядра: там спільний статичний буфер на всі задачі й повторне використання va_list.
extern "C" int __wrap_log_printf(const char *fmt, ...) {
  char b[192];
  va_list a;
  va_start(a, fmt);
  int n = vsnprintf(b, sizeof b, fmt, a);
  va_end(a);
  if (n < 0) return 0;
  if (n >= (int)sizeof b) n = sizeof b - 1;
  ets_printf("%s", b);
  fx_wdbg_line(b, n);
  return n;
}
// журнал ESP-IDF (ESP_LOGx): у сетевий журнал, далі як було
static vprintf_like_t s_prev_vprintf;
static int idf_vprintf(const char *fmt, va_list a) {
  char b[192];
  va_list c;
  va_copy(c, a);
  int n = vsnprintf(b, sizeof b, fmt, c);
  va_end(c);
  if (n > 0) fx_wdbg_line(b, n < (int)sizeof b ? n : sizeof b - 1);
  return s_prev_vprintf ? s_prev_vprintf(fmt, a) : vprintf(fmt, a);
}

static void client_close(int i) {
  if (s_cfd[i] >= 0) lwip_close(s_cfd[i]);
  s_cfd[i] = -1;
}

static void send_str(int fd, const String &s) { lwip_send(fd, s.c_str(), s.length(), MSG_DONTWAIT); }

// найстаріша позиція в кільці — з початку рядка (не з середини рядка чи літери UTF-8)
static uint32_t oldest_line(uint32_t w) {
  uint32_t p = w > sizeof s_ring ? w - sizeof s_ring : 0;
  if (p == 0) return 0;
  for (int k = 0; k < 400 && p != w; k++, p++)
    if (s_ring[p & (sizeof s_ring - 1)] == '\n') return p + 1;
  return p;
}

static void banner(int fd) {
  String c; fx_crash_text(c);
  String b = String("\r\n== " FX_NAME " " FX_VERSION " — сетевий журнал / network log, ") + String(__DATE__) + " " + String(__TIME__) +
             "\r\n== робота " + String(millis() / 1000) + " с, пам'ять " + String(ESP.getFreeHeap()) + " (найбільший блок " +
             String(ESP.getMaxAllocHeap()) + "), причина старту " + String((int)esp_reset_reason()) +
             "\r\n== остання аварія: " + (g_fx_crash_last.length() ? g_fx_crash_last : String("немає")) + "\r\n";
  send_str(fd, b);
}

static void start() {
  s_lfd = lwip_socket(AF_INET, SOCK_STREAM, 0);
  if (s_lfd >= 0) {
    int one = 1;
    lwip_setsockopt(s_lfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
    struct sockaddr_in a;
    memset(&a, 0, sizeof a);
    a.sin_family = AF_INET;
    a.sin_port = htons(23);
    a.sin_addr.s_addr = INADDR_ANY;
    if (lwip_bind(s_lfd, (struct sockaddr *)&a, sizeof a) != 0 || lwip_listen(s_lfd, 2) != 0) { lwip_close(s_lfd); s_lfd = -1; }
    else lwip_fcntl(s_lfd, F_SETFL, lwip_fcntl(s_lfd, F_GETFL, 0) | O_NONBLOCK);
  }
  ArduinoOTA.setMdnsEnabled(false);
  ArduinoOTA.setHostname("clock-informer");
  if (g_c33a4 != "" && g_c3398 != "") ArduinoOTA.setPassword(g_c3398.c_str());
  ArduinoOTA.onStart([]() { fx_wdbg_line("OTA: start", 10); ota_quiesce(); fx_status_begin(FXS_FLASH, true); });
  ArduinoOTA.onProgress([](unsigned int, unsigned int) { loop_guard_feed(); });
  ArduinoOTA.onEnd([]() { fx_wdbg_line("OTA: done, restart", 18); fx_status_end(); });
  ArduinoOTA.onError([](ota_error_t e) { char b[32]; fx_wdbg_line(b, snprintf(b, sizeof b, "OTA: error %d", (int)e)); fx_status_end(); });
  ArduinoOTA.begin();
  s_prev_vprintf = esp_log_set_vprintf(idf_vprintf);
  s_up = true;
  dbg_ws_print(String("WiFi debug  : log on TCP 23, OTA on UDP 3232"));
}

static void stop() {
  for (int i = 0; i < 2; i++) client_close(i);
  if (s_lfd >= 0) lwip_close(s_lfd);
  s_lfd = -1;
  ArduinoOTA.end();
  if (s_prev_vprintf) esp_log_set_vprintf(s_prev_vprintf);
  s_prev_vprintf = nullptr;
  s_up = false;
}

void fx_wdbg_set(bool on) {
  if (on == g_fx_wdbg) return;
  g_fx_wdbg = on;
  if (on) fx_wdbg_line("WiFi debug on", 13);
  else if (s_up) stop();
}

void fx_wdbg_tick() {
  if (!g_fx_wdbg) return;
  if (!s_up) {
    if (WiFi.status() != WL_CONNECTED) return;
    start();
  }
  if (millis() - s_t_ota >= 50) { s_t_ota = millis(); ArduinoOTA.handle(); }   // parsePacket ядра виділяє 1460 Б щоразу
  if (s_lfd >= 0) {
    int fd = lwip_accept(s_lfd, nullptr, nullptr);
    if (fd >= 0) {
      int i = s_cfd[0] < 0 ? 0 : s_cfd[1] < 0 ? 1 : -1;
      if (i < 0) { send_str(fd, String("busy: 2 clients already\r\n")); lwip_close(fd); }
      else {
        lwip_fcntl(fd, F_SETFL, lwip_fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
        int one = 1;
        lwip_setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);
        s_cfd[i] = fd;
        banner(fd);
        s_rd[i] = oldest_line(s_wr);
      }
    }
  }
  for (int i = 0; i < 2; i++) {
    int fd = s_cfd[i];
    if (fd < 0) continue;
    char in[32];
    int r = lwip_recv(fd, in, sizeof in, MSG_DONTWAIT);
    if (r == 0 || (r < 0 && errno != EAGAIN && errno != EWOULDBLOCK)) { client_close(i); continue; }
    uint32_t w = s_wr;
    if (w - s_rd[i] > sizeof s_ring - 512) {   // клієнт не встигає: пропускаємо до найстарішого цілого рядка
      s_rd[i] = oldest_line(w + 512);
      send_str(fd, String("\r\n...(lost)...\r\n"));
    }
    while (s_rd[i] != w) {
      uint32_t off = s_rd[i] & (sizeof s_ring - 1);
      uint32_t len = w - s_rd[i];
      if (len > sizeof s_ring - off) len = sizeof s_ring - off;
      if (len > 1024) len = 1024;
      int k = lwip_send(fd, s_ring + off, len, MSG_DONTWAIT);
      if (k < 0) { if (errno != EAGAIN && errno != EWOULDBLOCK) client_close(i); break; }
      s_rd[i] += k;
      if ((uint32_t)k < len) break;
    }
  }
}
