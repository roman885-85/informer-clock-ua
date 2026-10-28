// Перевірка черги повідомлень веб-інтерфейсу (справжній код з ws.cpp: від «#define WS_BOX» до кінця ws_async_hook):
// перехід через край кільця, порядок і цілісність повідомлень, відкидання при переповненні, зупинка, коли черга
// клієнта повна (availableForWriteAll), не більше 24 повідомлень за раз. Без мережі: AsyncWebSocket — заглушка.
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
typedef int portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED 0
#define portENTER_CRITICAL(m) (void)(m)
#define portEXIT_CRITICAL(m) (void)(m)
struct String { std::string s; String(const char *c = "") : s(c) {} const char *c_str() const { return s.c_str(); } size_t length() const { return s.size(); } };
struct AsyncWebSocket {
  int clients = 1; bool full = false; int cleanups = 0;
  std::vector<std::string> got;
  void cleanupClients(int) { cleanups++; }
  int count() { return clients; }
  bool availableForWriteAll() { return !full; }
  void textAll(const char *p, size_t n) { got.push_back(std::string(p, n)); }
};
#include "box.inc"
int main() {
  AsyncWebSocket ws; int bad = 0;
  g_fx_ws_n = 1;
  // 1) багато різних повідомлень через край кільця, порядок і вміст
  std::vector<std::string> sent;
  for (int round = 0; round < 200; round++) {
    int k = 1 + rand() % 30;
    for (int i = 0; i < k; i++) {
      std::string m = "{\"dbg\":\"" + std::to_string(round) + "/" + std::to_string(i) + std::string(rand() % 900, 'x') + "\"}";
      if (ws_send(m.c_str(), m.size())) sent.push_back(m);
    }
    while (true) { size_t before = ws.got.size(); ws_async_hook(&ws); if (ws.got.size() == before) break; }
  }
  size_t okn = 0; for (size_t i = 0; i < sent.size() && i < ws.got.size(); i++) okn += sent[i] == ws.got[i];
  printf("надіслано в чергу %zu, доставлено %zu, однакових по порядку %zu, відкинуто %u\n", sent.size(), ws.got.size(), okn, s_drop);
  bad += !(okn == sent.size() && ws.got.size() == sent.size());
  // 2) переповнення: без вивантаження — кладемо, поки не відмовить; кільце не зіпсоване
  ws.got.clear(); s_drop = 0; int put = 0;
  std::string big(5000, 'y');
  while (ws_send(big.c_str(), big.size())) put++;
  printf("переповнення: вмістилось %d по 5000 Б (кільце %d Б), далі відмова, відкинуто %u\n", put, WS_BOX, s_drop);
  ws_async_hook(&ws);
  bool intact = (int)ws.got.size() == put; for (auto &g : ws.got) intact = intact && g == big;
  printf("після вивантаження: доставлено %zu, цілі: %s\n", ws.got.size(), intact ? "так" : "НІ");
  bad += !intact;
  // 3) черга клієнта повна — нічого не береться; не більше 24 за раз
  ws.got.clear();
  for (int i = 0; i < 40; i++) ws_send("{\"a\":1}", 7);
  ws.full = true; ws_async_hook(&ws); size_t a = ws.got.size();
  ws.full = false; ws_async_hook(&ws); size_t b = ws.got.size();
  printf("черга клієнта повна: взято %zu (має 0); далі за один раз %zu (має 24)\n", a, b);
  bad += !(a == 0 && b == 24);
  // 4) немає клієнтів — нічого не кладемо; занадто довге — відмова
  ws_async_hook(&ws); ws_async_hook(&ws);
  ws.clients = 0; ws_async_hook(&ws);
  bool none = !ws_send("{\"a\":1}", 7);
  std::string huge(WS_MSG_MAX + 1, 'z'); g_fx_ws_n = 1; bool toolong = !ws_send(huge.c_str(), huge.size());
  printf("немає клієнтів — відмова: %s; довше %d Б — відмова: %s; cleanupClients викликано %d разів\n", none ? "так" : "НІ", WS_MSG_MAX, toolong ? "так" : "НІ", ws.cleanups);
  bad += !(none && toolong);
  printf(bad ? "РЕЗУЛЬТАТ: Є ПОМИЛКИ\n" : "РЕЗУЛЬТАТ: усе гаразд\n");
  return bad;
}
