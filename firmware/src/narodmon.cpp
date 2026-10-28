// narodmon.ru: показання сусідніх датчиків (API) і відправка своїх (TCP 8283). Перенесено з 2.7.1 (адреси в коментарях).
// EN: narodmon.ru: nearby sensors (API) and sending own readings (TCP 8283), from 2.7.1.
#include "fw.h"

static void nm_mac_id() {
  if (g_c338c.length() == 0) {
    g_c338c = String("ESP") + WiFi.macAddress();
    g_c338c.replace(String(":"), String(""));
  }
}
// @40107924: показання датчиків пристрою devId (раз на 12 хв; помилка/немає даних — 2 хв; 429 — 10 хв)
void narodmon_read(const String &apiKey, String &devId) {
  if (millis() - g_c1aac < g_c1aa8) return;
  g_c1aac = millis();
  if (g_c2fcc.equals("") && g_c2fd8.equals("") && g_c34a9 != 3) return;
  nm_mac_id();
  String uuid("");
  if (g_c338c != "") {
    char buf[g_c338c.length() + 1];
    g_c338c.getBytes((unsigned char *)buf, g_c338c.length() + 1);
    unsigned char *hash = MD5::make_hash(buf);
    char *md5str = MD5::make_digest(hash, 16);
    uuid = String(md5str);
    dbg_ws_print(String(" - HASH MD5: ") + buf + String(" = ") + String(md5str));
    free(hash);
    free(md5str);
    devId.replace(String("D"), String(""));
    String lang("");
    lang = (g_c2eef == 0) ? "ru" : "en";
    WiFiClient client;
    HTTPClient http;
    String url = String("http://narodmon.ru/api/sensorsOnDevice?id=") + devId + String("&uuid=") + uuid + String("&api_key=") + apiKey + String("&lang=") + lang;
    if (!http.begin(client, String(url))) {
      g_c1aa8 = 720000;
      return;
    }
    http.addHeader(String("Content-Type"), String("text/plain"), false, true);
    int code = http.GET();
    String s = http.getString();
    http.end();
    if (code == 429) {
      g_c1aa8 = 600000;
      g_c2fe4 = String("Narodmon: Too many requests");
      dbg_ws_print(String("Narodmon: too many requests"));
      return;
    }
    g_c2ff0 = s;
    s.replace(String("’"), String("'"));
    s.replace(String("“"), String("'"));
    s.replace(String("”"), String("'"));
    s.replace(String("`"), String("'"));
    s.replace(String("‘"), String("'"));
    s.replace(String("„"), String("'"));
    s.replace(String("•"), String("-"));
    s.replace(String("»"), String("'"));
    s.replace(String("«"), String("'"));
    s.replace(String("…"), String("..."));
    s.replace(String("–"), String("-"));
    s.replace(String("—"), String("-"));
    JsonDocument doc;
    deserializeJson(doc, s);
    String err = doc["error"].as<String>();
    if (!err.equals("null")) {
      g_c1aa8 = 120000;
      return;
    }
    int n = 0;
    for (;;) {
      int k = n + 1;
      dbg_ws_print(String("Parameter: ") + String(k, 10) + String(" = ") + doc["sensors"][n]["name"].as<String>());
      if (doc["sensors"][n]["name"].as<String>().equals(String("null"))) break;
      if (k == 10) {
        n = 0;
        goto have;
      }
      n = k;
    }
    dbg_ws_print(String("NULL data — exiting cycle, parameters = ") + String(n, 10));
  have:
    g_c2fe4 = String("");
    if (n != 0) {
      g_c2fe4.reserve(0x100);
      g_c2fe4 += String("Narodmon: ") + doc["location"].as<String>();
      for (int i = 0; i < n; i++) {
        g_c2fe4 += String(",  ") + doc["sensors"][i]["name"].as<String>();
        g_c2fe4 += String(": ") + String(doc["sensors"][i]["value"].as<float>(), 1) + " " + doc["sensors"][i]["unit"].as<String>();
      }
      dbg_ws_print(String(g_c2fe4) + "  | frontend=" + g_c2ff0);
      ws.textAll(g_c2ff0);
      g_c1aa8 = 720000;
      return;
    }
    g_c2fe4 = String("");
    g_c2ff0 = String("");
    g_c1aa8 = 120000;
    dbg_ws_print(String("Exit Narodmon cycle"));
  }
}
// @40109804: відправка своїх показань (#MAC … ##); true — надіслано
bool narodmon_send() {
  String buf("");
  buf.reserve(0x100);
  g_c1ab0 = 0;
  buf = String("#") + g_c338c + String("\r\n");
  dbg_ws_sep(1);
  dbg_ws_print(String("==================== NARODMON ====================="));
  if (g_c32e8 != 0 && g_c32ed == 1 && g_c2280.getTempCByIndex(0) != -127.0) {
    g_c2280.requestTemperatures();
    delay(20);
    uint8_t addr[8];
    g_c2280.getAddress(addr, 0);
    buf += String("#");
    for (uint8_t i = 0; i < 8; i++) {
      if (addr[i] < 16) buf.concat("0");
      buf += String(addr[i], 16);
    }
    buf += String("#") + String(g_c3300, 2) + String("\r\n");
    dbg_ws_print(String("Narodmon: DS18B20 data ready"));
    g_c1ab0 = 1;
  }
  if (g_c32e7 != 0 && g_c32ec == 1 && !String(g_c3308, 0).equals(String("nan"))) {
    if (g_c32e6 == 11) {
      buf += String("#DHT11temp#") + String(g_c3308, 1) + String("\r\n");
      buf += String("#DHT11hum#") + String(g_c3304, 0) + String("\r\n");
    } else if (g_c32e6 == 22) {
      buf += String("#DHT22temp#") + String(g_c3308, 1) + String("\r\n");
      buf += String("#DHT22hum#") + String(g_c3304, 0) + String("\r\n");
    }
    dbg_ws_print(String("Narodmon: DHT11/12/22 data ready"));
    g_c1ab0 = 1;
  }
  if (g_c32e5 != 0 && g_c32eb == 1) {
    if (g_c2ee5 == 0) {
      if (g_c3314 != 0.0 || g_c330c != 0.0 || g_c3310 != 0.0) {
        buf += String("#BME280temp#") + String(g_c3314, 1) + String("\r\n");
        buf += String("#BME280hum#") + String(round(g_c3310), 0) + String("\r\n");
        buf += String("#BME280pres#") + String(g_c330c, 0) + String("\r\n");
        dbg_ws_print(String("Narodmon: BME280 data ready"));
        g_c1ab0 = 1;
      }
    } else if (g_c2ee5 == 1) {
      if (g_c3314 != 0.0 || g_c330c != 0.0) {
        buf += String("#BMP280temp#") + String(g_c3314, 1) + String("\r\n");
        buf += String("#BMP280pres#") + String(g_c330c, 0) + String("\r\n");
        dbg_ws_print(String("Narodmon: BMP280 data ready"));
        g_c1ab0 = 1;
      }
    } else if (g_c2ee5 == 2 && (g_c3314 != 0.0 || g_c330c != 0.0)) {
      buf += String("#BMP180temp#") + String(g_c3314, 1) + String("\r\n");
      buf += String("#BMP180pres#") + String(g_c330c, 0) + String("\r\n");
      dbg_ws_print(String("Narodmon: BMP180 data ready"));
      g_c1ab0 = 1;
    }
  }
  if (g_c32e2 != 0 && g_c32ea == 1) {
    if (g_c32e3 == 1) {
      if (g_c32fc != 0.0 && g_c32f8 != 0.0) {
        buf += String("#HDC1080temp#") + String(g_c32fc, 1) + String("\r\n");
        buf += String("#HDC1080hum#") + String(g_c32f8, 0) + String("\r\n");
        dbg_ws_print(String("Narodmon: HDC1080 data ready"));
        g_c1ab0 = 1;
      }
    } else if (g_c32e3 == 0) {
      if (!String(g_c32fc, 2).equals(String("nan"))) {
        buf += String("#AM2320temp#") + String(g_c32fc, 1) + String("\r\n");
        buf += String("#AM2320hum#") + String(g_c32f8, 0) + String("\r\n");
        dbg_ws_print(String("Narodmon: AM2320 data ready"));
        g_c1ab0 = 1;
      }
    } else if (g_c32e3 == 2 && g_c32fc != 0.0 && g_c32f8 != 0.0) {
      buf += String("#AHT10temp#") + String(g_c32fc, 1) + String("\r\n");
      buf += String("#AHT10hum#") + String(g_c32f8, 0) + String("\r\n");
      dbg_ws_print(String("Narodmon: AHT10 data ready"));
      g_c1ab0 = 1;
    }
  }
  if (g_c32e1 != 0 && g_c32e9 == 1) {
    switch (g_c32e0) {
      case 0:
        if (g_c32f4 == 0 || g_c32f0 == 0) break;
        buf += String("#SGP30eco2#") + String(g_c32f0, 10) + String("\r\n");
        buf += String("#SGP30tvoc#") + String(g_c32f4, 10) + String("\r\n");
        dbg_ws_print(String("Narodmon: SGP30 data ready"));
        g_c1ab0 = 1;
        break;
      case 1:
        if (g_c32f4 == 0 || g_c32f0 == 0) break;
        buf += String("#CCS811eco2#") + String(g_c32f0, 10) + String("\r\n");
        buf += String("#CCS811tvoc#") + String(g_c32f4, 10) + String("\r\n");
        dbg_ws_print(String("Narodmon: CCS811 data ready"));
        g_c1ab0 = 1;
        break;
      case 2:
        if (0x122a < g_c32f0 - 350U) break;
        buf += String("#MHZ19Bco2#") + String(g_c32f0, 10) + String("\r\n");
        dbg_ws_print(String("Narodmon: MH-Z19B data ready"));
        g_c1ab0 = 1;
        break;
      case 3:
        if (0x122a < g_c32f0 - 350U) break;
        buf += String("#SCD40co2#") + String(g_c32f0, 10) + String("\r\n");
        dbg_ws_print(String("Narodmon: SCD40 data ready"));
        g_c1ab0 = 1;
        break;
      case 4:
        if (0x122a < g_c32f0 - 350U) break;
        buf += String("#SCD41co2#") + String(g_c32f0, 10) + String("\r\n");
        dbg_ws_print(String("Narodmon: SCD41 data ready"));
        g_c1ab0 = 1;
        break;
    }
  }
  if (g_c1ab0 == 0) {
    dbg_ws_print(String("Narodmon: no sensors active/selected — nothing to send"));
    return false;
  }
  g_c1ab0 = 0;
  dbg_ws_print(String(""));
  buf = buf + String("##\r\n");
  WiFiClient client;
  if (!client.connect("narodmon.ru", 8283)) {
    dbg_ws_print(String("connection failed"));
    return false;
  }
  client.print(buf);
  dbg_ws_print(String("Sending: ") + buf);
  while (client.available()) {
  }
  client.flush();
  client.stop();
  return true;
}
// @4010bbf8: відправка раз на 6 хв (якщо нема чого/не вдалося — повтор за 15 с)
void narodmon_tick() {
  if (millis() - g_c1ab8 < g_c1ab4) return;
  nm_mac_id();
  g_c1ab8 = millis();
  uint32_t ms;
  if (g_c32ed == 0 && g_c32ec == 0 && g_c32eb == 0 && g_c32ea == 0 && g_c32e9 == 0) {
    dbg_ws_print(String("NARODMON: all sensors are blocked from sending"));
    ms = 15000;
  } else if (g_c32e8 == 0 && g_c32e5 == 0 && g_c32e7 == 0 && g_c32e2 == 0 && g_c32e1 == 0) {
    dbg_ws_print(String("NARODMON: all sensors are disabled"));
    ms = 15000;
  } else if (narodmon_send()) {
    ms = 360000;
  } else {
    dbg_ws_print(String("NARODMON: send failed"));
    ms = 15000;
  }
  g_c1ab4 = ms;
}
