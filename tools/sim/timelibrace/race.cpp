// Гонка TimeLib 1.6.1 між двома задачами (tools/sim/timelibrace). У годиннику now() кличуть головний цикл (табло, ядро 1)
// і потік AsyncTCP (веб: /content?sys_time=, ручне встановлення часу; журнал подій з MQTT) — без жодного замка.
// now(): while (millis() - prevMillis >= 1000) { sysTime++; prevMillis += 1000; }. Якщо задача прочитала millis(), а
// інша до того, як перша прочитала prevMillis, перейшла межу секунди й збільшила prevMillis, різниця стає від'ємною
// (беззнакова — ≈4,29 млрд) і цикл додає ≈4 294 967 с — годинник стрибає на 49,7 доби вперед до наступної звірки NTP.
// Тут: справжній Time.cpp, дві нитки кличуть now(), третя веде модельний millis(); вікно між читанням millis() і
// prevMillis розширене коротким очікуванням після читання лічильника (на ESP32 так буває, коли між цими командами
// приходить переривання — Wi-Fi, таймери). Друга перевірка — setTime() з іншої задачі (як ручне встановлення з веба).
#include "TimeLib.h"
#include <atomic>
#include <thread>
#include <cstdio>
#include <cstdlib>
#include <chrono>

#include <mutex>
static std::mutex g_tl;                                // для копії з замком (firmware/lib/Time): TL_LOCK() на комп'ютері
void tl_lock() { g_tl.lock(); }
void tl_unlock() { g_tl.unlock(); }

static std::atomic<uint32_t> g_ms{1000};
static std::atomic<bool> g_stop{false};
static int g_spin = 200;
uint32_t millis() {
  uint32_t v = g_ms.load(std::memory_order_relaxed);
  for (volatile int i = 0; i < g_spin; i++) {}         // «переривання» одразу після читання лічильника
  return v;
}
static std::atomic<long long> g_worst{0};
static void watcher(int id, bool setter) {
  while (!g_stop.load()) {
    if (setter && (rand() & 1023) == 0) setTime((time_t)(1794674584 + g_ms.load() / 1000));   // «ручне встановлення»
    long long d = (long long)now() - (long long)(1794674584 + g_ms.load() / 1000);
    long long a = d < 0 ? -d : d;
    if (a > g_worst.load()) g_worst.store(a);
    if (a > 1000) { printf("    нитка %d: now() відійшов від модельного часу на %+lld с (%.1f доби)\n", id, d, d / 86400.0); g_stop = true; }
  }
}
int main(int argc, char **argv) {   // [секунд] [s — ще й setTime() з другої нитки]
  int secs = argc > 1 ? atoi(argv[1]) : 20;
  bool with_setter = argc > 2 && argv[2][0] == 's';
  setTime((time_t)1794674585);
  std::thread tick([] { while (!g_stop.load()) { g_ms.fetch_add(1); for (volatile int i = 0; i < 300; i++) {} } });
  std::thread a(watcher, 1, false), b(watcher, 2, with_setter);
  auto t0 = std::chrono::steady_clock::now();
  while (!g_stop.load() && std::chrono::steady_clock::now() - t0 < std::chrono::seconds(secs)) std::this_thread::sleep_for(std::chrono::milliseconds(50));
  g_stop = true;
  tick.join(); a.join(); b.join();
  printf("    модельних секунд: %u; найбільше відхилення now(): %lld с\n", g_ms.load() / 1000, g_worst.load());
  return g_worst.load() > 1000 ? 1 : 0;
}
