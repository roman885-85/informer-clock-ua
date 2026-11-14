// Імітатор часу годинника (tools/sim/ntpsim): справжні time.cpp і timefix.cpp прошивки (з 1.1.6 — і ntp.cpp) зі
// справжньою TimeLib 1.6.1 на моделі мережі. Збирається двічі: -DOLD — код випуску 1.1.5 (git de3803a) з моделлю клієнта
// SNTP ядра за його машинним кодом; без OLD — робоча копія (1.1.6). Сценарії 14.11.2026: годинник показував 06:19:22
// замість 18:43:04 (−44 622 с) і виправився звіркою 18:44:45. Адреси серверів — документаційні (192.0.2.x, 198.51.100.x,
// 203.0.113.x): справжній винуватець 14.11 невідомий.
#include "fw.h"
#include "world.h"
#include <unistd.h>
#include <sys/wait.h>

static const int64_t TZ = 7200;        // у власника tz = 2, sumT = 0
static int bad = 0;
static void check(bool ok, const char *what) {
  printf("    %s %s\n", ok ? "так" : "НІ ", what);
  if (!ok) bad++;
}
static int64_t loc(int h, int mi, int s, int d = 14) {     // місцевий час 14.11.2026 як «секунди від 1970» (як TimeLib)
  tmElements_t e;
  e.Year = 2026 - 1970; e.Month = 11; e.Day = d; e.Hour = h; e.Minute = mi; e.Second = s;
  return (int64_t)makeTime(e);
}
static int64_t true_loc_ms() { return (sim_true_us() / 1000) + TZ * 1000; }
static int32_t clock_off() { return (int32_t)((int64_t)now() - true_loc_ms() / 1000); }   // показ мінус справжній, с
static String hms(int64_t t) {
  tmElements_t e;
  breakTime((time_t)t, e);
  char b[16];
  snprintf(b, sizeof b, "%02u:%02u:%02u", e.Hour, e.Minute, e.Second);
  return String(b);
}

// сервери (документаційні адреси)
static const uint32_t G = sim_ip(198, 51, 100, 10);   // справний сервер пулу
static const uint32_t B = sim_ip(192, 0, 2, 66);      // сервер пулу з хибним годинником
static const uint32_t D = sim_ip(198, 51, 100, 20);   // мовчить
static const uint32_t CF = sim_ip(203, 0, 113, 1);    // «time.cloudflare.com»
static const uint32_t GG = sim_ip(203, 0, 113, 2);    // «time.google.com»
static const uint32_t P1 = sim_ip(198, 51, 100, 7);   // «1.pool.ntp.org»

static void world_std(int32_t bad_off, uint8_t bad_li, uint8_t bad_st, uint32_t rtt) {
  SimSrv g; g.label = "G"; g.rtt_ms = rtt; sim_server(G, g);
  SimSrv b; b.label = "B"; b.off_s = bad_off; b.li = bad_li; b.stratum = bad_st; b.rtt_ms = rtt; sim_server(B, b);
  SimSrv d; d.alive = false; sim_server(D, d);
  SimSrv c; c.rtt_ms = 15; sim_server(CF, c); sim_server(GG, c); sim_server(P1, c);
  sim_dns("pool.ntp.org", G);
  sim_dns("time.cloudflare.com", CF);
  sim_dns("time.google.com", GG);
  sim_dns("1.pool.ntp.org", P1);
}
// «увімкнення» о місцевому часі t_loc; sys_off — похибка системного часу ESP32 (після перезапуску — від RTC), с;
// sys_known = false — увімкнення живлення (системного часу немає)
static void boot(int64_t t_loc, bool sys_known = true, int32_t sys_off = 0) {
  sim_reset((t_loc - TZ) * 1000000LL + 300 * 1000, sys_known);   // + 0,3 с: звірки не на межі секунди
  if (sys_off) {
    struct timeval tv;
    int64_t us = sim_sys_us() + (int64_t)sys_off * 1000000;
    tv.tv_sec = us / 1000000; tv.tv_usec = us % 1000000;
    sim_settimeofday(&tv, nullptr);
    g_fx_sntp_n = 0;                   // це «пам'ять» RTC з попереднього запуску, а не встановлення
  }
  g_c3430 = 2; g_c32e4 = 0; g_c3461 = 10; g_c3434 = "pool.ntp.org"; g_c2d29 = 1; g_c3319 = 0; g_c3468 = 0;
  g_c18a8 = 0; g_c18ad = 0; g_c22b4 = 0; g_c22b5 = 0; g_c33fc = ""; g_c3414 = ""; g_c2eb4 = 0;
  fx_time_boot();                      // setup(): системний час, якщо він є (timefix.cpp)
}
// головний цикл: rtc_detect_sync() (перша звірка + ntp_periodic(t_syn) — як у прошивці) кожні step мс
static void run_to(int64_t t_loc, uint32_t step = 20) {
  while (true_loc_ms() < t_loc * 1000) { world_advance(step); rtc_detect_sync(); }
}
static void show(const char *what) {
  printf("    %s  справжній %s, годинник %s (%+d с), остання звірка NTP «%s»\n", what, hms(true_loc_ms() / 1000).c_str(),
         hms(now()).c_str(), (int)clock_off(), g_c33fc.c_str());
}
static bool evlog_has(const char *s) {
  for (auto &l : sim_evlog()) if (l.find(s) != std::string::npos) return true;
  return false;
}
static int evlog_count(const char *s) {
  int n = 0;
  for (auto &l : sim_evlog()) if (l.find(s) != std::string::npos) n++;
  return n;
}

// ---------- сценарії ----------
// 14.11: на звірці 18:34:45 DNS пулу дав сервер B з годинником на 12 год 23 хв 42 с позаду
static void s_1411(uint8_t li, uint8_t st, uint32_t rtt, int64_t bad_sync) {
  boot(loc(18, 14, 44));
  world_std(-44622, li, st, rtt);
  run_to(bad_sync - 5);
  sim_dns("pool.ntp.org", B);
  run_to(bad_sync + 5);
  sim_dns("pool.ntp.org", G);
  run_to(loc(18, 43, 4)); show("18:43:04");
  int32_t o1843 = clock_off();
  run_to(loc(18, 44, 26)); show("18:44:26");
  run_to(loc(18, 44, 47)); show("18:44:47");
  run_to(loc(18, 46, 8)); show("18:46:08");
  int32_t o1846 = clock_off();
#ifdef OLD
  check(o1843 <= -44621 && o1843 >= -44623, "о 18:43:04 годинник позаду на 44 622 с (±1) — як виміряно 14.11 (06:19:22)");
  check(o1846 >= -1 && o1846 <= 1, "звірка 18:44:45 виправила час — як 14.11");
  check(sim_evlog().empty(), "у журналі подій жодного рядка — як 14.11");
#else
  check(o1843 >= -1 && o1843 <= 1, "о 18:43:04 час правильний (±1 с)");
  check(o1846 >= -1 && o1846 <= 1, "о 18:46:08 час правильний (±1 с)");
  check(evlog_has("[TIME] NTP rejected pool.ntp.org 192.0.2.66"), "відкинута відповідь сервера B — у журналі подій");
  check(!evlog_has("->"), "у журналі подій немає зміни показаного часу");
#endif
}

#ifdef OLD
// сервер мовчить, а 1.1.5 однаково пише «отримано» і оновлює «останню звірку NTP» — системним часом
static void s_silent() {
  boot(loc(18, 14, 44));
  world_std(0, 0, 2, 30);
  run_to(loc(18, 34, 40));
  sim_dns("pool.ntp.org", D);
  run_to(loc(18, 34, 50)); show("18:34:50");
  check(g_c33fc.startsWith("18:34:4"), "сервер не відповів, а «остання звірка NTP» — 18:34:4x (взято системний час)");
}
// чужий пакет режиму 4 з будь-якої адреси на порт клієнта SNTP (18:30) — ядро бере його; відповідь справжнього
// сервера 18:34:45 приходить пізніше 100 мс — у TimeLib іде зіпсований системний час
static void s_foreign() {
  boot(loc(18, 14, 44));
  world_std(0, 0, 2, 400);
  run_to(loc(18, 30, 0));
  sim_old_inject_foreign(-44622);
  run_to(loc(18, 30, 1));
  printf("    18:30:01  системний час ESP32 після чужого пакета: %+lld с\n", (long long)((sim_sys_us() - sim_true_us()) / 1000000));
  run_to(loc(18, 43, 4)); show("18:43:04");
  check(clock_off() <= -44621 && clock_off() >= -44623, "чужий пакет з адреси 192.0.2.99 зіпсував час до наступної звірки");
}
#else
// чужі й запізнілі пакети на сокеті NTP ігноруються
static void s_stray() {
  boot(loc(18, 14, 44));
  world_std(0, 0, 2, 30);
  SimSrv g; g.stray_foreign = 1; g.stray_stale = 1; sim_server(G, g);
  run_to(loc(18, 35, 0)); show("18:35:00");
  check(clock_off() >= -1 && clock_off() <= 1, "час правильний");
  check(evlog_count("stray packet(s)") == 1, "чужі пакети — один рядок у журналі подій (не частіше разу на годину)");
}
// основний мовчить, а перед тайм-аутом на сокет приходить чужий пакет — у налагодженні причина «timeout», не порожня
static void s_timeout_reason() {
  boot(loc(18, 14, 44));
  world_std(0, 0, 2, 30);
  SimSrv dd; dd.alive = true; dd.rtt_ms = 5000; dd.stray_foreign = 1; sim_server(D, dd);   // «відповідь» за 5 с — після тайм-ауту
  sim_dns("pool.ntp.org", D);
  sim_verbose = true;
  run_to(loc(18, 14, 50));
  sim_verbose = false;
  check(clock_off() >= -1 && clock_off() <= 1, "час від запасного");
}
// «поцілунок смерті» й сервер, що мовчить: беруться запасні сервери
static void s_kod_silent() {
  boot(loc(18, 14, 44));
  world_std(0, 0, 2, 30);
  SimSrv k; k.kod = "RATE"; sim_server(G, k);
  run_to(loc(18, 24, 50)); show("18:24:50");
  check(clock_off() >= -1 && clock_off() <= 1, "KoD RATE від основного — час від запасного");
  sim_dns("pool.ntp.org", D);
  run_to(loc(18, 34, 50)); show("18:34:50");
  check(clock_off() >= -1 && clock_off() <= 1, "основний мовчить — час від запасного");
  check(sim_evlog().empty(), "у журналі подій нічого (час не змінювався)");
}
// перезапуск: системний час (RTC) поспішає на 1 год 18 хв (як 07.11) — NTP виправляє після підтвердження другим сервером
static void s_restart_drift() {
  boot(loc(18, 14, 44), true, 4680);
  world_std(0, 0, 2, 30);
  show("запуск   ");
  run_to(loc(18, 15, 0)); show("18:15:00");
  check(clock_off() >= -1 && clock_off() <= 1, "перша звірка після запуску виправила 1 год 18 хв");
  check(evlog_has("[TIME] NTP pool.ntp.org 198.51.100.10") && evlog_has("confirmed by time.cloudflare.com 203.0.113.1: 19:32:4") &&
        (evlog_has("(-4679 s)") || evlog_has("(-4680 s)") || evlog_has("(-4681 s)")),
        "у журналі подій: звідки (основний сервер, підтвердив другий), було -> стало");
}
// увімкнення живлення: часу немає — береться перший придатний сервер, у журнал подій нічого
static void s_power_on() {
  boot(loc(18, 14, 44), false);
  world_std(0, 0, 2, 30);
  run_to(loc(18, 15, 0)); show("18:15:00");
  check(clock_off() >= -1 && clock_off() <= 1, "час з першої звірки");
  check(sim_evlog().empty(), "у журналі подій нічого");
}
// другого сервера немає (інші імена не розв'язуються): стрибок не береться одразу; той самий стрибок удруге — береться
static void s_no_second() {
  boot(loc(18, 14, 44));
  world_std(-44622, 0, 2, 30);
  sim_dns("time.cloudflare.com", 0); sim_dns("time.google.com", 0); sim_dns("1.pool.ntp.org", 0);
  run_to(loc(18, 24, 40));
  sim_dns("pool.ntp.org", B);
  run_to(loc(18, 24, 50)); show("18:24:50");
  check(clock_off() >= -1 && clock_off() <= 1, "одна відповідь без підтвердження — час не змінено");
  check(evlog_has("no second server to confirm"), "у журналі подій: стрибок не підтверджено");
  run_to(loc(18, 34, 50)); show("18:34:50");
  check(clock_off() <= -44621, "той самий стрибок на наступній звірці — прийнято (єдиний доступний сервер)");
  check(evlog_has("confirmed by repeat"), "у журналі подій: прийнято після повтору");
}
// основний сервер у налаштуваннях зламаний назавжди (−44 622 с): час тримається, а «rejected» у журналі подій —
// не щозвірки (журнал 4 КБ не має переповнитись), а раз на 6 год
static void s_bad_forever() {
  boot(loc(0, 0, 10));
  world_std(-44622, 0, 2, 30);
  sim_dns("pool.ntp.org", B);
  run_to(loc(12, 0, 0), 100);
  show("12:00:00");
  int n = evlog_count("[TIME] NTP rejected");
  printf("    за 12 год рядків «NTP rejected»: %d (звірок — 72)\n", n);
  check(clock_off() >= -1 && clock_off() <= 1, "час правильний (щоразу від другого сервера)");
  check(n == 2, "у журналі подій 2 рядки за 12 год (о 00:10 і через 6 год)");
}
// встановлення вручну й назад NTP
static void s_manual() {
  boot(loc(18, 14, 44));
  world_std(0, 0, 2, 30);
  run_to(loc(18, 20, 0));
  g_c344c = "10:25"; g_c3440 = "2026-11-14";
  manual_time_set(); show("вручну  ");
  check(evlog_has("[TIME] manual (t_m/d_m): 18:19:") && evlog_has("-> 10:25:00 14.11.2026"), "ручне встановлення — у журналі подій");
  run_to(loc(18, 25, 0)); show("18:25:00");
  check(clock_off() >= -1 && clock_off() <= 1, "звірка NTP повернула час (підтверджено другим сервером)");
}
// зміна поясу tz 2 -> 3 (веб): показаний час +1 год, у журналі подій — зсув і було -> стало; звірка NTP його не «виправляє»
static void s_tz() {
  boot(loc(18, 14, 44));
  world_std(0, 0, 2, 30);
  run_to(loc(18, 20, 0));
  g_c3430 = 3;
  fx_time_offset_apply(); show("tz = 3  ");
  check(evlog_has("[TIME] offset 120 -> 180 min (time zone / summer time): 18:19:") && evlog_has("-> 19:19:"),
        "зміна поясу — у журналі подій з було -> стало");
  run_to(loc(18, 25, 0)); show("18:25:00");
  check(clock_off() >= 3599 && clock_off() <= 3601, "звірка NTP тримає новий пояс (+1 год)");
  check(evlog_count("[TIME]") == 1, "інших рядків [TIME] немає (NTP не вважає пояс стрибком)");
}
// звичайна робота: доба звірок кожні 10 хв — жодного рядка в журналі подій, похибка показу ≤ 1 с
static void s_day() {
  boot(loc(0, 0, 10));
  world_std(0, 0, 2, 30);
  int32_t worst = 0;
  for (int h = 0; h < 24; h++)
    for (int m = 0; m < 60; m += 5) {
      run_to(loc(h, m, 7), 100);
      int32_t o = clock_off();
      if (o < 0 ? -o > worst : o > worst) worst = o < 0 ? -o : o;
    }
  printf("    доба: найбільша похибка показу %d с, рядків у журналі подій %d\n", (int)worst, (int)sim_evlog().size());
  check(worst <= 1, "похибка не більше 1 с");
  check(sim_evlog().empty(), "жодного рядка в журналі подій");
}
// розбір відповіді: перевірки RFC 4330
static void s_parse() {
  uint8_t tx[8] = {1, 2, 3, 4, 5, 6, 7, 8}, p[48];
  auto mk = [&](uint8_t li, uint8_t vn, uint8_t mode, uint8_t st, uint32_t ref_age, uint32_t disp_ms, bool our) {
    memset(p, 0, 48);
    p[0] = (li << 6) | (vn << 3) | mode; p[1] = st;
    uint32_t sec = 1794674584u + 2208988800u, ref = sec - ref_age, d = disp_ms * 65536u / 1000;
    p[8] = d >> 24; p[9] = d >> 16; p[10] = d >> 8; p[11] = d;
    if (st == 0) memcpy(p + 12, "RATE", 4);
    p[16] = ref >> 24; p[17] = ref >> 16; p[18] = ref >> 8; p[19] = ref;
    if (our) memcpy(p + 24, tx, 8);
    p[40] = sec >> 24; p[41] = sec >> 16; p[42] = sec >> 8; p[43] = sec; p[44] = 0x80;   // .5 с
  };
  FxNtpReply r;
  mk(0, 4, 4, 2, 64, 20, true);
  check(fx_ntp_parse(p, 48, tx, r) == 1 && r.utc == 1794674584u && r.us >= 499999 && r.us <= 500000, "придатна відповідь: час і частка секунди");
  check(fx_ntp_parse(p, 47, tx, r) == -1, "коротка — не наша");
  mk(0, 4, 4, 2, 64, 20, false);
  check(fx_ntp_parse(p, 48, tx, r) == -1, "чужа мітка запиту (запізніла/чужа) — не наша");
  mk(3, 4, 4, 2, 64, 20, true);
  check(fx_ntp_parse(p, 48, tx, r) == 0, "LI = 3 (не синхронізований) — відкинуто");
  mk(0, 4, 4, 16, 64, 20, true);
  check(fx_ntp_parse(p, 48, tx, r) == 0, "страта 16 — відкинуто");
  mk(0, 4, 4, 0, 64, 20, true);
  check(fx_ntp_parse(p, 48, tx, r) == 0 && strcmp(r.why, "KoD RATE") == 0, "страта 0 — «KoD RATE»");
  mk(0, 4, 5, 2, 64, 20, true);
  check(fx_ntp_parse(p, 48, tx, r) == 0, "режим 5 (широкомовний) — відкинуто");
  mk(0, 0, 4, 2, 64, 20, true);
  check(fx_ntp_parse(p, 48, tx, r) == 0, "версія 0 — відкинуто");
  mk(0, 4, 4, 2, 64, 1600, true);
  check(fx_ntp_parse(p, 48, tx, r) == 0, "root dispersion 1,6 с — відкинуто (як MAXDIST ntpd)");
  mk(0, 4, 4, 2, 90000, 20, true);
  check(fx_ntp_parse(p, 48, tx, r) == 0, "еталон сервера старший за добу — відкинуто");
  mk(0, 3, 4, 2, 64, 20, true);
  check(fx_ntp_parse(p, 48, tx, r) == 1, "версія 3 — придатна");
}
#endif

static void run(const char *title, void (*fn)()) {
  printf("\n== %s\n", title);
  fflush(stdout);
  pid_t pid = fork();                  // кожен сценарій — окремий процес: статичний стан TimeLib і прошивки з нуля
  if (pid == 0) { bad = 0; fn(); fflush(stdout); _exit(bad); }
  int st = 0;
  waitpid(pid, &st, 0);
  bad += WIFEXITED(st) ? WEXITSTATUS(st) : 100;
}
static void s1() { s_1411(0, 2, 30, loc(18, 34, 45)); }
static void s2() { s_1411(3, 16, 30, loc(18, 34, 45)); }
static void s3() { s_1411(0, 2, 400, loc(18, 24, 45)); }

int main(int argc, char **argv) {
  sim_verbose = argc > 1 && strcmp(argv[1], "-v") == 0;
#ifdef OLD
  printf("Код випуску 1.1.5 (time.cpp, timefix.cpp з git) + модель клієнта SNTP ядра 1.0.4 за машинним кодом 1.1.5\n");
#else
  printf("Робоча копія (1.1.6: time.cpp, timefix.cpp, ntp.cpp)\n");
#endif
  run("14.11: сервер пулу з хибним годинником (−44 622 с, LI 0, страта 2), відповідь за 30 мс на звірці 18:34:45", s1);
  run("те саме, сервер «не синхронізований» (LI 3, страта 16)", s2);
  run("те саме, відповіді за 400 мс (пізніше 100 мс), хибна — на звірці 18:24:45", s3);
#ifdef OLD
  run("сервер мовчить на звірці 18:34:45", s_silent);
  run("чужий пакет режиму 4 з іншої адреси на порт SNTP о 18:30, відповіді за 400 мс", s_foreign);
#else
  run("чужі й запізнілі пакети на сокеті NTP", s_stray);
  run("KoD від основного, потім основний мовчить", s_kod_silent);
  run("основний мовчить, чужий пакет перед тайм-аутом (рядок налагодження)", s_timeout_reason);
  run("перезапуск: системний час (RTC) поспішає на 1 год 18 хв", s_restart_drift);
  run("увімкнення живлення без часу", s_power_on);
  run("другого сервера немає", s_no_second);
  run("основний сервер зламаний назавжди: журнал подій не переповнюється", s_bad_forever);
  run("встановлення вручну, потім NTP", s_manual);
  run("зміна поясу 2 -> 3", s_tz);
  run("доба звичайної роботи", s_day);
  run("розбір відповіді NTP", s_parse);
#endif
  printf("\n%s: %d невідповідностей\n", bad ? "ПОМИЛКИ" : "усе так", bad);
  return bad ? 1 : 0;
}
