// GET /content: живі значення й ключі конфігурації (перенесено з 2.7.1: @400fab4c, @400f9430)
// EN: GET /content: live values and configuration keys (from 2.7.1).
#include "fw.h"

static String two(uint8_t v) { return v < 10 ? String("0") + String(v, 10) : String(v, 10); }

// @400f9430: значення «живого» ключа
void content_json_build(String &out, const String &key) {
  DateTime rt = g_c346c.now();
  uint8_t rY = rt.year() - 2000, rM = rt.month(), rD = rt.day(), rS = rt.second();
  const String *ps = nullptr;
  const char *pc = nullptr;
  float fv; unsigned int dec;
  if (key.equals("get_ip")) { pc = g_c2d29 ? "1" : "0"; goto setc; }
  if (key.equals("networks")) { mass_json(out, 3); return; }
  if (key.equals("saved_networks")) { ps = &g_c2c28; goto sets; }
  if (key.equals("scanIPdev")) { g_c3340 = ""; g_c3359 = 0; pc = "ok"; g_c335a = 1; g_c3358 = 0; goto setc; }
  if (key.equals("scanedIPdev")) { ps = &g_c334c; goto sets; }
  if (key.equals("lsynNtpNet")) {
    if (g_c2eb4 == 0) { pc = "No data"; goto setc; }
    String t; time_hm_text(t, 2);
    out = t + ":" + String(g_c3460 / 10) + String((uint32_t)g_c3460 % 10) + " - " + two(g_c345d) + "." + two(g_c345c) + "." + String(g_c3458);
    return;
  }
  if (key.equals("rtc_detect")) { pc = g_c3468 ? "1" : "0"; goto setc; }
  if (key.equals("lsynNtpSys")) { ps = &g_c33fc; goto sets; }
  if (key.equals("lsynRtcNet")) { ps = &g_c3414; goto rtcs; }
  if (key.equals("lsynRtcSys")) { ps = &g_c3408; goto rtcs; }
  if (key.equals("rtc_time")) {
    if (g_c3468 == 0) { out = "-"; return; }
    String t; time_hm_text(t, 3);
    out = t + ":" + String(rS / 10) + String((uint32_t)rS % 10) + " - " + two(rD) + "." + two(rM) + "." + String(rY + 2000);
    return;
  }
  if (key.equals("sys_time")) { out = String((unsigned long)now(), 10); return; }
  if (key.equals("devmac")) { ps = &g_c338c; goto sets; }
  if (key.equals("valuta_rate")) { fv = g_bf048; goto nanchk; }
  if (key.equals("btc_rate")) { fv = g_bf044; goto nanchk; }
  if (key.equals("news_now")) { ps = &g_c30fc; goto sets; }
  if (key.equals("heap_diag")) {
    out = String("free=") + String(ESP.getFreeHeap(), 10) + String(" maxBlock=") + String(ESP.getMaxAllocHeap(), 10) + String(" total=") + String(ESP.getHeapSize(), 10);
    return;
  }
  if (key.equals("flash_mb")) { out = String(ESP.getFlashChipSize() >> 0x14, 10); return; }
  if (key.equals("bme280_t")) { fv = g_c3314; dec = 1; goto fl; }
  if (key.equals("bme280_h")) { out = String(round((double)g_c3310), 0); return; }
  if (key.equals("bme280_pr")) { fv = g_c330c; dec = 0; goto fl; }
  if (key.equals("tds")) { fv = g_c3300; dec = 1; goto fl; }
  if (key.equals("dht_t")) { fv = g_c3308; dec = 1; goto fl; }
  if (key.equals("dht_hum")) { fv = g_c3304; dec = 0; goto fl; }
  if (key.equals("SeS_t")) { fv = g_c32fc; dec = 1; goto fl; }
  if (key.equals("SeS_h")) { fv = g_c32f8; dec = 0; goto fl; }
  if (key.equals("TVOCs")) { out = String(g_c32f4, 10); return; }
  if (key.equals("eCO2s")) { out = String(g_c32f0, 10); return; }
  if (key.equals("mq_stat")) { pc = g_c30c5 ? "1" : "0"; goto setc; }
  if (key.equals("mqt1")) { if (g_c30c5) { mqtt_text(out, 0x1f); return; } goto empty; }
  if (key.equals("mqt2")) { if (g_c30c5) { mqtt_text(out, 0x20); return; } goto empty; }
  if (key.equals("mqt3")) { if (g_c30c5) { mqtt_text(out, 0x21); return; } goto empty; }
  if (key.equals("mqt4")) { if (g_c30c5) { mqtt_text(out, 0x22); return; } goto empty; }
  if (key.equals("narodData")) { ps = &g_c2ff0; goto sets; }
  if (key.equals("weather_now")) {
    if ((g_c3157 & 0xfd) != 1) { pc = "[{\"event\":\"off\"}]"; goto setc; }
    if (g_c3148 != "") { weather_now_json(out, g_c3224); return; }
    pc = "[{\"event\":\"No API\"}]"; goto setc;
  }
  if (key.equals("weather_now_wb")) {
    if (g_c3157 != 2) { pc = "[{\"event\":\"off\"}]"; goto setc; }
    if (g_c313c != "") { weather_now_json(out, g_c31c4); return; }
    pc = "[{\"event\":\"No API\"}]"; goto setc;
  }
  if (key.equals("weather_tom")) {
    if (g_c3157 != 2) { pc = "[{\"event\":\"off\"}]"; goto setc; }
    if (g_c313c != "") { weather_tom_json(out, g_c3170); return; }
    pc = "[{\"event\":\"No API\"}]"; goto setc;
  }
  if (key.equals("latest_version_fw")) { ps = &g_c2ea8; goto sets; }
  if (key.equals("current_controller_version")) { out = FX_VERSION; return; }
  if (key.equals("CPU")) { pc = "32"; goto setc; }
  if (key.equals("chip")) { pc = ""; goto setc; }
  if (key.equals("test")) {
    uint8_t gol = g_c2f1b;
    if (g_c2fc9 == 1) myDFPlayer.playFolder(gol, random(1, 0x18));
    else if (g_c2fc9 == 2) g_c2f21 = 99;
    goto empty;
  }
  if (key.equals("local_ip")) { out = WiFi.localIP().toString(); return; }
  if (key.equals("connected_ssid")) { out = WiFi.SSID(); return; }
  if (key.equals("wifi_connect_status")) { ps = &g_c3374; goto sets; }
empty:
  out = "";
  return;
nanchk:
  dec = 2;
  if (isnan(fv)) goto empty;
fl:
  out = String(fv, dec);
  return;
rtcs:
  if (g_c3468 != 0) goto sets;
  pc = "-";
setc:
  out = pc;
  return;
sets:
  out = *ps;
}

// перелік «живих» ключів (масив String на стеку оригіналу, 47 шт.)
static const char *const LIVE[47] = { "scanIPdev", "scanedIPdev", "bme280_t", "bme280_h", "bme280_pr", "tds", "dht_t", "dht_hum", "SeS_t", "SeS_h",
  "mass_temp_bme", "weather_now", "weather_now_wb", "weather_tom", "time_line", "current_controller_version", "latest_version_fw", "devmac",
  "rtc_detect", "sys_time", "rtc_time", "lsynRtcSys", "lsynRtcNet", "lsynNtpSys", "lsynNtpNet", "networks", "saved_networks", "CPU", "chip",
  "get_ip", "TVOCs", "eCO2s", "test", "mq_stat", "mqt1", "mqt2", "mqt3", "mqt4", "narodData", "local_ip", "connected_ssid",
  "wifi_connect_status", "heap_diag", "flash_mb", "valuta_rate", "btc_rate", "news_now" };

// @400d2c44
bool is_secret_key(const String &k) { return k.equals("pass") || k.equals("secwp"); }

// @400fb790 → @400fab4c
void h_content(AsyncWebServerRequest *r) {
  g_c34c0 = millis();
  int n = r->params();
  String live[47];
  for (int i = 0; i < 47; i++) live[i] = LIVE[i];
  String cur("");
  JsonDocument out, cfg, filter;
  for (int i = 0; i < n; i++) {
    const String &nm = r->getParam(i)->name();
    bool skip = nm.equals("hist") ? true : is_secret_key(nm);
    if (!skip) {
      bool isLive = false;
      for (int j = 0; j < 47; j++) if (nm.equals(live[j])) { isLive = true; break; }
      if (!isLive) filter[nm] = true;
    }
  }
  bool loaded = false;
  for (int i = 0; i < n; i++) {
    AsyncWebParameter *p = r->getParam(i);
    cur = p->name();
    if (cur.equals("networks") && p->value().equals("force")) g_c34c5 = 1;
    if (!cur.equals("hist")) {
      bool isLive = false;
      for (int j = 0; j < 47; j++) {
        if (cur.equals(live[j])) {
          String v; content_json_build(v, cur);
          out[cur] = v;
          isLive = true;
        }
      }
      if (!isLive) {
        if (!is_secret_key(cur)) {
          if (!loaded) {
            cfg.clear();
            deserializeJson(cfg, g_c2ef0, DeserializationOption::Filter(filter), DeserializationOption::NestingLimit(10));
          }
          out[String(cur)] = cfg[cur];
          loaded = true;
        } else {
          out[cur] = "";
        }
      }
    } else {
      JsonDocument h;
      String hs; hist_json(hs);
      deserializeJson(h, hs);
      out["hist"] = h;
    }
  }
  if (g_c2ea8 != "" && fx_version_newer(g_c2ea8, FX_VERSION)) out["update_version"] = g_c2ea8;   // змінено: лише новіша
  AsyncResponseStream *resp = r->beginResponseStream(String("application/json"), 0x5b4);
  serializeJson(out, *resp);
  r->send(resp);
}
