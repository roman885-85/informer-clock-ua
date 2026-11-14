// Модель світу для імітатора часу (tools/sim/ntpsim): модельний час, системний годинник ESP32, DNS, сервери NTP,
// клієнт SNTP ядра 1.0.4 (як у машинному коді 1.1.5) і сокети lwIP для власного клієнта 1.1.6.
#pragma once
#include <stdint.h>
#include <string>
#include <vector>

struct SimSrv {
  const char *label = "";
  int32_t off_s = 0;               // годинник сервера мінус справжній час, с
  uint8_t li = 0, vn = 4, stratum = 2;
  uint32_t rtt_ms = 30;            // коло запит-відповідь
  bool alive = true;               // відповідає
  uint32_t ref_age_s = 64;         // скільки секунд тому сервер востаннє звірявся (reference timestamp)
  uint32_t rdelay_ms = 10, rdisp_ms = 20;
  const char *kod = nullptr;       // «поцілунок смерті» (страта 0) з цим кодом
  int stray_foreign = 0;           // перед відповіддю — стільки чужих пакетів режиму 4 (інша адреса) з хибним часом
  int stray_stale = 0;             // перед відповіддю — стільки запізнілих (та сама адреса, чужа мітка запиту)
};

extern bool sim_verbose;
void sim_reset(int64_t true_utc_us_at_boot, bool sys_time_known);   // «увімкнення» годинника
void sim_server(uint32_t ip, const SimSrv &s);
void sim_dns(const char *name, uint32_t ip);                       // що DNS відповідає на ім'я зараз (0 — не знає)
void world_advance(uint32_t ms);
int64_t sim_true_us();                                              // справжній UTC, мкс
int64_t sim_sys_us();                                               // системний час ESP32, мкс
std::vector<std::string> &sim_evlog();
void sim_old_inject_foreign(int32_t off_s);                         // 1.1.5: чужий пакет режиму 4 на порт клієнта SNTP
uint32_t sim_ip(uint8_t a, uint8_t b, uint8_t c, uint8_t d);
