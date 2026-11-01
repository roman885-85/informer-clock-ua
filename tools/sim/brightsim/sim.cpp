// Симуляція плавної яскравості: справжні bright.cpp, photo.cpp і dim.cpp з firmware/src (крок 5 мс; потік dim — щомс)
#include "mock/fw.h"
uint32_t g_now; int g_adc; MockP P; MockSPI SPI; MockMX MX; MockMX *g_c34bc = &MX;
uint8_t g_c2ee9 = 1, g_c2f25 = 0, g_bf041 = 7, g_c34b2 = 12, g_c2ee3, g_c34b3 = 0x16, g_c3080;
uint16_t g_c3284;
void run(const char *name, int ms, int (*adc)(uint32_t), int printEvery) {
  printf("== %s\n", name);
  int last = -2, changes = 0, w0 = SPI.writes;
  for (int t = 0; t < ms; t += 5) {
    g_now += 5; g_adc = adc(t);
    fx_bright_tick();
    for (int k = 0; k < 5; k++) fx_dim_step_ms();
    int lv = fx_dim_level();                          // найближчий цілий рівень (на табло — з проміжними між ними)
    if (lv != last) { changes++; if (printEvery) printf("  t=%5.2f s  АЦП=%4d (згладж. %4d)  рівень≈%d  ціль=%d\n", t / 1000.0, g_adc, fx_photo_adc(), lv, g_bf041); last = lv; }
  }
  printf("  змін рівня: %d, на табло зараз %d, ціль %d, записів інтенсивності %d\n", changes, SPI.level, g_bf041, SPI.writes - w0);
}
static uint32_t rnd = 1;
int noise(int a) { rnd = rnd * 1103515245 + 12345; return (int)((rnd >> 16) % (2 * a + 1)) - a; }
int dark_to_light(uint32_t t) { return t < 3000 ? 300 : 1100; }             // увімкнули світло
int light_to_dark(uint32_t t) { return t < 3000 ? 1100 : 150; }             // вимкнули
int boundary(uint32_t t) { (void)t; return 600 + noise(25); }               // шум ±25 на межі рівнів 6/7
int flicker(uint32_t t) { return (t / 500) % 2 ? 700 : 560; }               // хтось ходить повз (півсекундні тіні)
int drift(uint32_t t) { return 200 + (int)(t / 60); }                       // повільний світанок
int main() {
  g_now = 1000;
  fx_dim_begin();
  run("темно → світло (300 → 1100)", 12000, dark_to_light, 1);
  run("світло → темно (1100 → 150)", 12000, light_to_dark, 1);
  run("шум ±25 на межі рівнів (600)", 60000, boundary, 1);
  run("тіні по 0,5 с (560 ↔ 700)", 30000, flicker, 1);
  run("світанок (200 → 1200 за 60 с)", 60000, drift, 1);
}
