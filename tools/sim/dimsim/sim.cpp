// Симуляція плавної яскравості: справжній dim.cpp з firmware/src, крок 1 мс (tools/sim/dimsim/run.sh).
// Для кожного переходу: скільки триває, чи рівно ціль у кінці, чи «на око» яскравість іде монотонно (ковзне середнє
// шпаруватості за 20 мс), скільки записів у табло, найрідша частота чергування рівнів, і що в спокої записів немає.
// Вихід CSV (мс;шпаруватість;середня) — для графіка в звіті: sim csv <з> <до>
#include "fw.h"
#include <cstdlib>
#include <vector>
int g_locks; MockSPI SPI; MockMX MX; MockMX *g_c34bc = &MX;
static double duty(int l) { return (2 * l + 1) / 32.0; }
struct Res { int ms_to_target; bool exact; bool monotonic; int writes; bool adjacent; int hold_ms; int rest_writes; };
static Res fade(int from, int to, std::vector<double> *trace) {
  fx_dim_follow(true, from); fx_dim_jump(from);
  for (int i = 0; i < 3000; i++) fx_dim_step_ms();      // устоялось на from
  int w0 = SPI.writes;
  fx_dim_follow(true, to);
  std::vector<int> lv;
  int reached = -1;
  for (int ms = 0; ms < 6000; ms++) {
    fx_dim_step_ms();
    lv.push_back(SPI.level);
    if (reached < 0 && fx_dim_level() == to && SPI.level == to) {
      bool stable = true;                                // ще 50 мс без змін — дійшли
      for (int k = 0; k < 50; k++) { fx_dim_step_ms(); lv.push_back(SPI.level); stable = stable && SPI.level == to; }
      if (stable) { reached = ms; break; }
    }
  }
  Res r{};
  r.ms_to_target = reached;
  r.exact = SPI.level == to;
  r.writes = SPI.writes - w0;
  // монотонність ковзного середнього (20 мс) з допуском на сигма-дельта
  std::vector<double> avg;
  for (size_t i = 0; i < lv.size(); i++) {
    double s = 0; int n = 0;
    for (size_t k = i >= 19 ? i - 19 : 0; k <= i; k++) { s += duty(lv[k]); n++; }
    avg.push_back(s / n);
    if (trace) { trace->push_back(duty(lv[i])); trace->push_back(s / n); }
  }
  r.monotonic = true;
  for (size_t i = 25; i < avg.size(); i++) {
    double d = avg[i] - avg[i - 5];
    if ((to > from && d < -0.012) || (to < from && d > 0.012)) r.monotonic = false;
  }
  // без перескоків: сусідні мілісекунди — той самий рівень або сусідній; найдовше утримання рівня посеред переходу
  // (яскравість іде між рівнями повільно — рівень тримається, це не мерехтіння)
  int end = reached > 0 ? reached : (int)lv.size();
  r.adjacent = true;
  for (int i = 1; i < end; i++) if (abs(lv[i] - lv[i - 1]) > 1) r.adjacent = false;
  int longest = 0, run = 1;
  for (int i = 1; i < end; i++) { if (lv[i] == lv[i - 1]) run++; else { if (run > longest) longest = run; run = 1; } }
  r.hold_ms = longest;
  int w1 = SPI.writes;
  for (int i = 0; i < 5000; i++) fx_dim_step_ms();
  r.rest_writes = SPI.writes - w1;
  return r;
}
int main(int argc, char **argv) {
  fx_dim_begin();                                        // замок шини — як на годиннику
  if (argc > 3 && !strcmp(argv[1], "csv")) {
    std::vector<double> tr;
    fade(atoi(argv[2]), atoi(argv[3]), &tr);
    printf("ms;duty;avg20\n");
    for (size_t i = 0; i + 1 < tr.size(); i += 2) printf("%zu;%.5f;%.5f\n", i / 2, tr[i], tr[i + 1]);
    return 0;
  }
  int pairs[][2] = { {0, 12}, {12, 0}, {0, 1}, {1, 0}, {5, 6}, {6, 5}, {0, 15}, {15, 0}, {3, 9}, {14, 15} };
  bool ok = true;
  printf("перехід   до цілі, мс  рівно ціль  монотонно  записів  без перескоків рівня  найдовше утримання, мс  записів у спокої (5 с)\n");
  for (auto &p : pairs) {
    Res r = fade(p[0], p[1], nullptr);
    printf("%2d → %-2d   %10d  %10s  %9s  %7d  %25s  %22d  %8d\n", p[0], p[1], r.ms_to_target, r.exact ? "так" : "НІ",
           r.monotonic ? "так" : "НІ", r.writes, r.adjacent ? "так" : "НІ", r.hold_ms, r.rest_writes);
    ok = ok && r.ms_to_target > 0 && r.exact && r.monotonic && r.adjacent && r.rest_writes == 0;
  }
  // керування забрали посеред переходу — більше жодного запису
  fx_dim_follow(true, 0); fx_dim_jump(0); for (int i = 0; i < 100; i++) fx_dim_step_ms();
  fx_dim_follow(true, 12); for (int i = 0; i < 300; i++) fx_dim_step_ms();
  int w = SPI.writes; fx_dim_follow(false, 0); for (int i = 0; i < 2000; i++) fx_dim_step_ms();
  bool quiet = SPI.writes == w;
  printf("керування забрали посеред переходу: записів після цього %d — %s\n", SPI.writes - w, quiet ? "так і має бути" : "ПОМИЛКА");
  // повернули керування: з того, що було, без стрибка вгору (jump не викликали — рівень потік знає)
  printf("замок шини брали %d разів (кожен запис — під замком)\n", g_locks);
  ok = ok && quiet;
  printf(ok ? "РЕЗУЛЬТАТ: усе гаразд\n" : "РЕЗУЛЬТАТ: Є ПОМИЛКИ\n");
  return ok ? 0 : 1;
}
