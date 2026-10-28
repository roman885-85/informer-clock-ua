// Історія датчиків: 5 джерел, до 48 точок по 30 хв, файли /h_<ім'я>.bin, JSON для /hist (перенесено з 2.7.1)
#include "fw.h"

// функції «увімкнено» @400d2918…@400d2968 і читання @400d2880…@400d28ec
static bool he_ds() { return g_c32e8 != 0; }
static bool he_dht() { return g_c32e7 != 0; }
static bool he_bme() { return g_c32e5 != 0; }
static bool he_oth() { return g_c32e2 != 0; }
static bool he_air() { return g_c32e1 != 0; }
static void hr_ds(float *v) { v[0] = g_c3300; }
static void hr_dht(float *v) { v[0] = g_c3308; v[1] = g_c3304; }
static void hr_bme(float *v) { v[0] = g_c3314; v[1] = (g_c2ee5 == 0) ? g_c3310 : NAN; v[2] = g_c330c; }
static void hr_oth(float *v) { v[0] = g_c32fc; v[1] = g_c32f8; }
static void hr_air(float *v) { v[0] = (float)g_c32f0 / 1.0; v[1] = (g_c32e0 < 2) ? (float)g_c32f4 / 1.0 : NAN; }

struct HistSrc { const char *name; uint8_t count; const char *const *keys; const float *scale; bool (*enabled)(); void (*read)(float *); int16_t *buf; uint8_t n; uint32_t base; };
static const char *const K_T[] = { "temp" };
static const char *const K_TH[] = { "temp", "hum" };
static const char *const K_THP[] = { "temp", "hum", "pressure" };
static const char *const K_AIR[] = { "co2", "tvoc" };
static const float S1[] = { 100.0 }, S2[] = { 100.0, 100.0 }, S3[] = { 100.0, 100.0, 1.0 }, SA[] = { 1.0, 1.0 };
// таблиця @3ffbec2c
HistSrc HIST[5] = {
  { "ds", 1, K_T, S1, he_ds, hr_ds, nullptr, 0, 0 },
  { "dht", 2, K_TH, S2, he_dht, hr_dht, nullptr, 0, 0 },
  { "bme", 3, K_THP, S3, he_bme, hr_bme, nullptr, 0, 0 },
  { "oth", 2, K_TH, S2, he_oth, hr_oth, nullptr, 0, 0 },
  { "air", 2, K_AIR, SA, he_air, hr_air, nullptr, 0, 0 },
};

// @400db1c0
void hist_file_name(String &out, const char *name) { out = String("/h_") + name + ".bin"; }
// @400fd8c0: прочитати файл історії
static void hist_load(HistSrc &h) {
  if (h.buf == nullptr) return;
  String fn; hist_file_name(fn, h.name);
  File f = SPIFFS.open(fn, "r");
  if (!f) return;
  uint8_t hd[8];
  if (f.read(hd, 8) == 8 && hd[0] == 'H' && hd[1] == '3' && h.count == hd[3]) {
    h.n = hd[2] < 0x30 ? hd[2] : 0x30;
    h.base = hd[4] | (hd[5] << 8) | (hd[6] << 16) | ((uint32_t)hd[7] << 24);
    f.read((uint8_t *)h.buf, h.count * 0x60);
  }
  f.close();
}
// @400fda78: записати файл історії
static void hist_save(HistSrc &h) {
  if (h.buf == nullptr) return;
  String fn; hist_file_name(fn, h.name);
  File f = SPIFFS.open(fn, "w");
  if (!f) return;
  uint8_t hd[8] = { 'H', '3', h.n, h.count, (uint8_t)h.base, (uint8_t)(h.base >> 8), (uint8_t)(h.base >> 16), (uint8_t)(h.base >> 24) };
  f.write(hd, 8);
  f.write((uint8_t *)h.buf, h.count * 0x60);
  f.close();
}
// @400fd9a0: виділити/звільнити буфери за ввімкненими датчиками
void hist_init() {
  for (int i = 0; i < 5; i++) {
    HistSrc &h = HIST[i];
    if (!h.enabled()) {
      if (h.buf != nullptr) {
        free(h.buf);
        h.buf = nullptr; h.n = 0; h.base = 0;
        String fn; hist_file_name(fn, h.name);
        SPIFFS.remove(fn);
        g_bed08 = 1;
      }
    } else if (h.buf == nullptr) {
      h.buf = (int16_t *)malloc(h.count * 0x60);
      if (h.buf != nullptr) {
        for (uint16_t k = 0; k < h.count * 0x30; k++) h.buf[k] = (int16_t)0x8000;
        h.n = 0; h.base = 0;
        hist_load(h);
        g_bed08 = 1;
      }
    }
  }
}
// @400fdb54: точка раз на 30 хв
void hist_sample() {
  uint32_t t = now();
  if (t <= 0x5f5e0fff) return;
  for (int i = 0; i < 5; i++) {
    HistSrc &h = HIST[i];
    if (h.buf == nullptr || (h.n != 0 && t - h.base <= 0x707)) continue;
    float v[3];
    int16_t s[3];
    h.read(v);
    if (h.n == 0 && isnan(v[0])) continue;
    uint8_t c = h.count;
    for (uint8_t k = 0; k < c; k++) {
      int16_t q = (int16_t)-0x8000;
      if (!isnan(v[k])) {
        long r = lround((double)(v[k] * h.scale[k]));
        if (r < 0x8000) q = (r < -0x7fff) ? -0x7fff : (int16_t)r;
        else q = 0x7fff;
      }
      s[k] = q;
    }
    if (0x2f < h.n) { memmove(h.buf, h.buf + c, c * 0x5e); h.n = 0x2f; }
    memcpy(h.buf + h.n * c, s, c * 2);
    h.base = t;
    h.n++;
    hist_save(h);
  }
  g_bed08 = 1;
}
// @400db250: JSON історії
void hist_json(String &out) {
  out = "";
  out.reserve(0x4b0);
  out += "{\"step\":";
  out += String(0x708, 10);
  out += ",\"cap\":";
  out += String(0x30, 10);
  for (int i = 0; i < 5; i++) {
    HistSrc &h = HIST[i];
    if (h.buf == nullptr) continue;
    out += ",\"";
    out += h.name;
    out += "\":{\"n\":";
    out += String(h.n, 10);
    out += ",\"base\":";
    out += String(h.base, 10);
    for (uint8_t k = 0; k < h.count; k++) {
      out += ",\"";
      out += h.keys[k];
      out += "\":[";
      float sc = h.scale[k];
      for (uint8_t j = 0; j < h.n; j++) {
        if (j != 0) out += (char)0x2c;
        int v = h.buf[j * h.count + k];
        if (v == -0x8000) out += "null";
        else if (sc < 100.0) out += String(v);
        else out += String((float)v / 1.0f / 100.0f, 2);
      }
      out += (char)0x5d;
    }
    out += (char)0x7d;
  }
  out += (char)0x7d;
}
