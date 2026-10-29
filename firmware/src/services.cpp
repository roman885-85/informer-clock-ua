// Перевірка нової прошивки, оновлення по мережі (OTA), визначення координат за IP. Перенесено з 2.7.1 (адреси в коментарях).
// EN: Firmware update check and OTA from this project's repository (version.h), IP geolocation.
#include "fw.h"

static int s_ota_err;                 // помилка останньої спроби оновлення (ota_fetch)
static String s_ota_errs;

// Оновлення — з репозиторію цього проєкту (version.h), не з сервера автора (http://goldart.ee/api/esp32/). Перевірку
// «адреса оновлень має бути незмінною» (@400e3990 — інакше 25 повідомлень у Serial і перезапуск) прибрано разом з адресою.

bool fx_version_newer(const String &a, const String &b) {
  long x[3] = {0, 0, 0}, y[3] = {0, 0, 0};
  sscanf(a.c_str(), "%ld.%ld.%ld", &x[0], &x[1], &x[2]);
  sscanf(b.c_str(), "%ld.%ld.%ld", &y[0], &y[1], &y[2]);
  for (int i = 0; i < 3; i++) if (x[i] != y[i]) return x[i] > y[i];
  return false;
}

// @400d30c8: відсоток завантаження OTA у веб-інтерфейс (не частіше ніж раз на 250 мс, крок ≥ 2 %)
void ota_progress(size_t done, size_t total) {
  loop_guard_feed();                             // нове: завантаження прошивки може йти довше межі сторожа
  int pct = 0;
  if (0 < (int)total) pct = (int)(((long long)done * 100) / (long long)(int)total);
  uint32_t now = millis();
  if (pct != g_bebe8 && (99 < pct || 1 < pct - g_bebe8 || 249 < now - g_c1734)) {
    g_bebe8 = pct;
    g_c1734 = now;
    char b[48];
    ws_send(b, snprintf(b, sizeof b, "{\"ota_pct\":\"%d\",\"ota_phase\":\"%d\"}", pct, g_c2e98));
  }
}
// @400d312c: етап OTA у веб-інтерфейс
void ota_phase(uint8_t phase) {
  g_c2e98 = phase;
  char b[48];
  ws_send(b, snprintf(b, sizeof b, "{\"ota_pct\":\"0\",\"ota_phase\":\"%d\"}", phase));
}
// @4011629c: перевірка нової версії (раз на 6 год; помилка — 3 год). Змінено: опис версії — FX_UPDATE_URL (HTTPS,
// GitHub) у форматі {"ver":"1.0.1","bin":"https://…/informer-clock-ua-esp32.bin",…}; g_c2ea8 — остання версія,
// g_c2e9c — адреса файла прошивки (у автора там було ім'я файла на його сервері).
void fw_version_check() {
  if (millis() - g_c19c0 < g_c2eb8 || g_c3319 != 0) return;
  dbg_ws_print(String("SERVICES: checking for new firmware"));
  FxStatusScope fxs(FXS_UPDATE);                 // нове: стан на табло під час запуску
  g_c19c0 = millis();
  WiFiClientSecure client;
  HTTPClient http;
  client.setCACert(NULL);
  http.setTimeout(8000);
  http.useHTTP10(true);
  if (!http.begin(client, String(FX_UPDATE_URL))) {
    g_c2eb8 = 21600000;
    return;
  }
  if (http.GET() == 200) {
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream())) {
      http.end();
      g_c2ea8 = doc["ver"].as<String>();
      g_c2e9c = doc["bin"].as<String>();
      dbg_ws_print(String("Firmware: this ") + FX_VERSION + ", latest " + g_c2ea8 + "  file=" + g_c2e9c);
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
    Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", s_ota_err, s_ota_errs.c_str());
    evlog_write(String("[OTA] fail ") + s_ota_err + " " + s_ota_errs);
    ota_phase(4);
  } else if (r == HTTP_UPDATE_OK) {
    dbg_ws_print(String("HTTP_UPDATE_OK"));
    return;
  } else {
    return;
  }
  display_zones_init();
}
// Нове (власник, 29.10.2026: «під час оновлення всі служби годинника мають бути зупинені, щоб не заважали, — крім
// веб-інтерфейсу»). Головний цикл і так стоїть, поки йде завантаження; тут зупиняється те, що працює поза ним або
// встигло б почати: MQTT, стрічка, звук, плавна яскравість (табло — екран оновлення). Веб-сервер працює (відсоток).
void ota_quiesce() {
  if (g_c2628.connected()) g_c2628.disconnect(true);
  led_off();
  myDFPlayer.stop();
  fx_dim_follow(false, 0);
  dbg_ws_print(String("OTA: services stopped (MQTT, LED strip, sound, dimming) — web interface stays"));
}
// Нове: завантаження стійке до пауз зв'язку — у ядра 1.0.4 HTTPUpdate чекає даних 8 с двічі (16 с) і здається
// («Stream Read Timeout», 29.10.2026 у власника — п'ять разів поспіль); тут — 30 с і до 3 спроб.
static t_httpUpdate_return ota_fetch(WiFiClientSecure &client) {
  HTTPUpdate upd(30000);
  t_httpUpdate_return r = HTTP_UPDATE_FAILED;
  for (int a = 1; a <= 3; a++) {
    r = upd.update(client, g_c2e9c, String(FX_VERSION));
    if (r != HTTP_UPDATE_FAILED) break;
    evlog_write(String("[OTA] attempt ") + a + " fail " + upd.getLastError() + " " + upd.getLastErrorString());
    loop_guard_feed();
    delay(2000);
  }
  s_ota_err = upd.getLastError();
  s_ota_errs = upd.getLastErrorString();
  return r;
}
// @40108d70: оновлення прошивки (g_c2eec: 1 — з веб-інтерфейсу, 2 — автоматично). Змінено: файл — за адресою з опису
// версії (g_c2e9c, HTTPS, GitHub), а не goldart.ee/api/esp32/ESP32_<файл>; без адреси (перевірки ще не було) — нічого.
void ota_update() {
  if (!g_c2e9c.startsWith("https://")) {
    config_set(String("fw_up"), String("0"));
    g_c2d28 = 1;
    config_save_atomic();
    g_c2eec = 0;
    dbg_ws_print(String("OTA: no update file yet — check for new firmware first"));
    return;
  }
  WiFiClientSecure client;
  client.setCACert(NULL);
  Update.onProgress(ota_progress);
  if (g_c2eec == 1) {
    ota_screen();
    config_set(String("fw_up"), String("0"));
    g_c2d28 = 1;
    config_save_atomic();
    digitalWrite(0, 0);
    ota_phase(2);
    ota_quiesce();
    t_httpUpdate_return r = ota_fetch(client);
    ota_result(r);
  } else if (g_c2eec == 2) {
    ota_screen();
    digitalWrite(0, 0);
    config_set(String("fw_up"), String("0"));
    g_c2d28 = 1;
    config_save_atomic();
    ota_phase(2);
    ota_quiesce();
    t_httpUpdate_return r = ota_fetch(client);
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
