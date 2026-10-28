// Перевірка нової прошивки, оновлення по мережі (OTA), визначення координат за IP. Перенесено з 2.7.1 (адреси в коментарях).
#include "fw.h"

static const char FW_URL_B64[] = "aHR0cDovL2dvbGRhcnQuZWUvYXBpL2VzcDMyLw==";   // http://goldart.ee/api/esp32/

// @400e3990: адреса оновлень має бути незмінною, інакше — 25 повідомлень у Serial і перезапуск.
// (Текст повідомлення автора тут замінено нейтральним; у копії адреса та сама, тож гілка не виконується.)
void fw_url_check(const String &url) {
  String u(url);
  if (u != String("http://goldart.ee/api/esp32/")) {
    Serial.begin(115200);
    delay(300);
    for (int i = 25; i != 0; i--) Serial.println("firmware update URL was changed");
    delay(5000);
    df_silence();
    ESP.restart();
  }
}
// @400d30c8: відсоток завантаження OTA у веб-інтерфейс (не частіше ніж раз на 250 мс, крок ≥ 2 %)
void ota_progress(size_t done, size_t total) {
  int pct = 0;
  if (0 < (int)total) pct = (int)(((long long)done * 100) / (long long)(int)total);
  uint32_t now = millis();
  if (pct != g_bebe8 && (99 < pct || 1 < pct - g_bebe8 || 249 < now - g_c1734)) {
    g_bebe8 = pct;
    g_c1734 = now;
    ws.printfAll("{\"ota_pct\":\"%d\",\"ota_phase\":\"%d\"}", pct, g_c2e98);
  }
}
// @400d312c: етап OTA у веб-інтерфейс
void ota_phase(uint8_t phase) {
  g_c2e98 = phase;
  ws.printfAll("{\"ota_pct\":\"0\",\"ota_phase\":\"%d\"}", phase);
}
// @4011629c: перевірка версії на сервері автора (раз на 6 год; помилка — 3 год)
void fw_version_check() {
  if (millis() - g_c19c0 < g_c2eb8 || g_c3319 != 0) return;
  dbg_ws_print(String("SERVICES: checking for new firmware"));
  g_c19c0 = millis();
  WiFiClient client;
  HTTPClient http;
  client.setTimeout(5000);
  http.setTimeout(5000);
  http.useHTTP10(true);
  char url[128];
  base64_decode(url, (char *)String(FW_URL_B64).c_str(), String(FW_URL_B64).length());
  fw_url_check(String(url));
  if (!http.begin(client, String(url))) {
    g_c2eb8 = 21600000;
    return;
  }
  http.addHeader(String("Content-Type"), String("text/plain"), false, true);
  if (http.GET() == 200) {
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream())) {
      http.end();
      g_c2ea8 = doc["ver"].as<String>();
      g_c2e9c = doc["ru"].as<String>();
      dbg_ws_print(String("Firmware ver=") + g_c2ea8 + "  file=" + g_c2e9c);
      g_c2eb8 = 21600000;
      return;
    }
    http.end();
    g_c2eb8 = 10800000;
  } else {
    http.end();
    g_c2eb8 = 10800000;
  }
}
// підготувати табло до оновлення (обидві гілки ota_update)
static void ota_screen() {
  P.setFont(0, (MD_MAX72XX::fontType_t *)font_icons_status);
  P.setFont(1, (MD_MAX72XX::fontType_t *)font_icons_status);
  display_clear_all();
  P.setCharSpacing(0);
  if (g_c34ba == 8) {
    if (g_c34b8 == 8) {
      uint8_t a, b;
      if (g_c34b7 == 0) {
        P.setZone(3, 0, 3);
        P.setZone(1, 0xc, 0xf);
        P.setZone(0, 4, 7);
        a = 8; b = 0xb;
      } else {
        P.setZone(3, 4, 7);
        P.setZone(1, 8, 0xb);
        P.setZone(0, 0, g_c34b8 - 5);
        a = 0xc; b = 0xf;
      }
      P.setZone(2, a, b);
    }
    P.displayZoneText(1, "\x1f", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
    P.displayZoneText(0, " ", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
    delay(50);
  } else if (g_c34ba == 4) {
    P.displayZoneText(0, "'", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
    delay(50);
  }
  display_clear_all();
  P.displayReset();
  P.displayAnimate();
}
static void ota_result(t_httpUpdate_return r) {
  if (r == HTTP_UPDATE_NO_UPDATES) {
    dbg_ws_print(String("HTTP_UPDATE_NO_UPDATES"));
  } else if (r == HTTP_UPDATE_FAILED) {
    Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    evlog_write(String("[OTA] fail ") + httpUpdate.getLastError() + " " + httpUpdate.getLastErrorString());
    ota_phase(4);
  } else if (r == HTTP_UPDATE_OK) {
    dbg_ws_print(String("HTTP_UPDATE_OK"));
    return;
  } else {
    return;
  }
  display_zones_init();
}
// @40108d70: оновлення прошивки (g_c2eec: 1 — з веб-інтерфейсу, 2 — автоматично) з goldart.ee/api/esp32/ESP32_<файл>
void ota_update() {
  WiFiClient client;
  char url[128];
  base64_decode(url, (char *)String(FW_URL_B64).c_str(), String(FW_URL_B64).length());
  Update.onProgress(ota_progress);
  if (g_c2eec == 1) {
    ota_screen();
    config_set(String("fw_up"), String("0"));
    g_c2d28 = 1;
    config_save_atomic();
    digitalWrite(0, 0);
    ota_phase(2);
    t_httpUpdate_return r = httpUpdate.update(client, String(String(url)) + String("ESP32_") + String(g_c2e9c), String(""));
    ota_result(r);
  } else if (g_c2eec == 2) {
    ota_screen();
    digitalWrite(0, 0);
    config_set(String("fw_up"), String("0"));
    g_c2d28 = 1;
    config_save_atomic();
    ota_phase(2);
    t_httpUpdate_return r = httpUpdate.update(client, String(String(url)) + String("ESP32_") + String(g_c2e9c), String(""));
    ota_result(r);
  }
}
// @400e3a60
void geo_error() { dbg_ws_print(String("Get location: error")); }
// @4011bec4: координати за зовнішньою IP-адресою (один раз після старту)
void geolocate_ip() {
  if (g_c19b0 != 0 || g_c3319 != 0) return;
  g_c19b0 = 1;
  String ip(""), lat(""), lon("");
  WiFiClient client;
  HTTPClient http;
  client.setTimeout(5000);
  http.setTimeout(5000);
  http.useHTTP10(true);
  if (!http.begin(client, String("http://api.ipify.org/?format=json"))) {
    geo_error();
    return;
  }
  http.addHeader(String("Content-Type"), String("text/plain"), false, true);
  if (http.GET() != 200) {
    http.end();
    geo_error();
    return;
  }
  JsonDocument doc;
  if (deserializeJson(doc, http.getStream())) {
    http.end();
    geo_error();
    return;
  }
  http.end();
  ip = doc["ip"].as<String>();
  dbg_ws_print(String("My WAN IP address: ") + ip);
  // помилка автора в дужках: begin() без IP, а результат (bool) додається до рядка — умова завжди істинна
  if (!(http.begin(client, String("http://ip-api.com/json/")) + ip)) {
    geo_error();
    return;
  }
  http.setTimeout(5000);
  http.addHeader(String("Content-Type"), String("text/plain"), false, true);
  if (http.GET() != 200) {
    http.end();
    client.flush();
    client.stop();
    geo_error();
    return;
  }
  JsonDocument doc2;
  if (!deserializeJson(doc2, http.getStream())) {
    http.end();
    client.flush();
    client.stop();
    lat = doc2["lat"].as<String>();
    lon = doc2["lon"].as<String>();
    dbg_ws_print(String("Location: lat=") + lat + "  lon=" + lon);
    config_set(String("lat"), lat);
    config_set(String("lon"), lon);
    g_c2d28 = 1;
  } else {
    http.end();
    client.flush();
    client.stop();
    geo_error();
  }
}
