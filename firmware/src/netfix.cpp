// Мережа: безпечний DNS для ядра 1.0.4 і перевірка TCP з тайм-аутом (нове, не з оригіналу).
// EN: Network fixes (new): safe DNS for core 1.0.4, connect timeouts, TCP probe, SNTP freshness counter (linker --wrap).
#include "fw.h"
#include "lwip/api.h"
#include "lwip/sockets.h"

bool g_fx_dns_on;                       // DNS captive-порталу запущено

// Ядро 1.0.4: WiFiGenericClass::hostByName кличе dns_gethostbyname не з потоку lwIP і чекає відповіді
// лише 4 с. Коли DNS відповідає пізніше (поганий інтернет, повтори запиту), lwIP записує адресу в змінну
// на стеку функції, що вже повернулась, — псування пам'яті й випадкові падіння; до того ж «застарілий»
// прапорець готовності робить наступний запит хибно невдалим. Підміна через -Wl,--wrap (platformio.ini):
// запит іде через потік lwIP (netconn_gethostbyname) і чекає його завершення (успіх або тайм-аут DNS).
extern "C" int __wrap__ZN16WiFiGenericClass10hostByNameEPKcR9IPAddress(const char *host, IPAddress &out) {
  out = (uint32_t)0;
  if (host == nullptr || *host == 0) return 0;
  ip_addr_t a;
  if (netconn_gethostbyname_addrtype(host, &a, NETCONN_DNS_IPV4) != ERR_OK || !IP_IS_V4(&a)) return 0;
  out = ip_2_ip4(&a)->addr;
  return out != (uint32_t)0;
}

// Ядро 1.0.4: HTTPS-клієнт (ssl_client.cpp — погода, новини, курси) з'єднується блокуючим lwip_connect без
// тайм-ауту. Коли інтернет зник між перевірками, головний цикл стояв, поки lwIP вичерпає повтори SYN (від ~20 с
// до кількох хвилин). Для блокуючих сокетів — те саме з'єднання, але не довше NET_CONNECT_TIMEOUT_S;
// неблокуючі (WiFiClient, проби) — без змін.
extern "C" int __real_lwip_connect(int s, const struct sockaddr *name, socklen_t namelen);
extern "C" int __wrap_lwip_connect(int s, const struct sockaddr *name, socklen_t namelen) {
  int fl = lwip_fcntl(s, F_GETFL, 0);
  if (fl < 0 || (fl & O_NONBLOCK)) return __real_lwip_connect(s, name, namelen);
  lwip_fcntl(s, F_SETFL, fl | O_NONBLOCK);
  int r = __real_lwip_connect(s, name, namelen);
  if (r < 0 && errno == EINPROGRESS) {
    fd_set w;
    FD_ZERO(&w);
    FD_SET(s, &w);
    struct timeval tv;
    tv.tv_sec = NET_CONNECT_TIMEOUT_S;
    tv.tv_usec = 0;
    if (lwip_select(s + 1, nullptr, &w, nullptr, &tv) > 0) {
      int e = 0;
      socklen_t l = sizeof e;
      lwip_getsockopt(s, SOL_SOCKET, SO_ERROR, &e, &l);
      if (e == 0) r = 0;
      else errno = e;
    } else errno = ETIMEDOUT;
  }
  lwip_fcntl(s, F_SETFL, fl);
  return r;
}

// Лічильник встановлень системного часу: його кличе лише клієнт SNTP lwIP (lwipopts.h: SNTP_SET_SYSTEM_TIME_US) —
// так ntp_request() відрізняє свіжу відповідь сервера від старого системного часу.
volatile uint32_t g_fx_sntp_n;
extern "C" int __real_settimeofday(const struct timeval *tv, const struct timezone *tz);
extern "C" int __wrap_settimeofday(const struct timeval *tv, const struct timezone *tz) {
  int r = __real_settimeofday(tv, tz);
  g_fx_sntp_n = g_fx_sntp_n + 1;
  return r;
}

int8_t net_tcp_probe(IPAddress ip, uint16_t port, uint32_t ms) {
  int s = lwip_socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0) return -1;
  lwip_fcntl(s, F_SETFL, lwip_fcntl(s, F_GETFL, 0) | O_NONBLOCK);
  struct sockaddr_in a;
  memset(&a, 0, sizeof a);
  a.sin_family = AF_INET;
  a.sin_port = htons(port);
  a.sin_addr.s_addr = (uint32_t)ip;
  int8_t r = -1;
  if (lwip_connect(s, (struct sockaddr *)&a, sizeof a) == 0) r = 1;
  else if (errno == EINPROGRESS) {
    fd_set w;
    FD_ZERO(&w);
    FD_SET(s, &w);
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    if (lwip_select(s + 1, nullptr, &w, nullptr, &tv) > 0) {
      int e = 0;
      socklen_t l = sizeof e;
      lwip_getsockopt(s, SOL_SOCKET, SO_ERROR, &e, &l);
      if (e == 0) r = 1;
      else if (e == ECONNREFUSED || e == ECONNRESET) r = 0;
    }
  } else if (errno == ECONNREFUSED) r = 0;
  lwip_close(s);
  return r;
}

bool net_gateway_alive() {
  IPAddress gw = WiFi.gatewayIP();
  if ((uint32_t)gw == 0) return false;
  return net_tcp_probe(gw, 80, 1500) >= 0;
}
