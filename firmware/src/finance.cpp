// Курс валюти (Frankfurter) і біткоїна (CoinGecko). Перенесено з 2.7.1 (адреси в коментарях).
#include "fw.h"

// @401016a4: курс g_c312c → g_c3120 (раз на 30 хв; помилка — 3 год)
void currency_fetch() {
  if (millis() - g_c1ae8 < g_c1ae4) return;
  if (heap_max_block() < 30000) {
    dbg_ws_print(String("Currency: skip TLS — low contiguous memory"));
    return;
  }
  g_c1ae8 = millis();
  WiFiClientSecure client;
  HTTPClient http;
  https_prepare(http, client);
  String url = String("https://api.frankfurter.dev/v2/rates?base=") + g_c312c + String("&quotes=") + g_c3120;
  String p;
  if (!http.begin(client, String(url))) {
    g_c1ae4 = 10800000;
    led_phrase(p, g_c34b0, 2);
    g_c1aec = String(p) + String(": Server not responding");
    return;
  }
  http.addHeader(String("Content-Type"), String("text/plain"), false, true);
  int code = http.GET();
  if (code != 200) {
    http.end();
    if (code < 1) g_c3318 = 1;
    g_c1ae4 = 10800000;
    led_phrase(p, g_c34b0, 2);
    g_c1aec = String(p) + String(": Server not responding");
    return;
  }
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, http.getStream());
  http.end();
  if (err) {
    g_c1ae4 = 10800000;
    led_phrase(p, g_c34b0, 2);
    g_c1aec = String(p) + String(": Invalid server response");
    dbg_ws_print(String("JSON parse error: ") + err.c_str());
    return;
  }
  if (!doc.is<JsonArray>() || doc.as<JsonArray>().size() == 0 || !doc[0]["rate"].is<float>()) {
    g_c1ae4 = 10800000;
    led_phrase(p, g_c34b0, 2);
    g_c1aec = String(p) + String(": Invalid rate format");
    dbg_ws_print(String("Unexpected Frankfurter response format"));
    return;
  }
  float rate = doc[0]["rate"].as<float>();
  g_bf048 = rate;
  ws.printfAll(String(String("{\"valuta_rate\": \"") + String(g_bf048, 2) + String("\"}")).c_str());
  led_phrase(p, g_c34b0, 0x31);
  g_c1aec = String(p) + g_c312c;
  int ph = -1;
  const char *suf = nullptr;
  if (g_c3120.equals(String("UAH"))) ph = 0x32;
  else if (g_c3120.equals(String("BYN"))) ph = 0x33;
  else if (g_c3120.equals(String("KZT"))) ph = 0x34;
  else if (g_c3120.equals(String("RUB"))) ph = 0x35;
  else if (g_c3120.equals(String("RON"))) ph = 0x2e;
  else if (g_c3120.equals(String("BGN"))) suf = " BGN";
  else if (g_c3120.equals(String("PLN"))) suf = " PLN";
  if (ph >= 0) {
    led_phrase(p, g_c34b0, ph);
    g_c1aec += String(" = ") + String(rate, 2) + p;
  } else if (suf != nullptr) {
    g_c1aec += String(" = ") + String(rate, 2) + String(suf);
  } else {
    g_c1aec += String(" = ") + String(rate, 2) + String(" ") + g_c3120;
  }
  dbg_ws_print(g_c1aec);
  g_c1ae4 = 1800000;
}
// @40102a1c: біткоїн у валюті g_c3114 (раз на 6 год; помилка — 3 год)
void bitcoin_fetch() {
  if (millis() - g_c1ad4 < g_c1ad0) return;
  if (heap_max_block() < 30000) {
    dbg_ws_print(String("Bitcoin: skip TLS — low contiguous memory"));
    return;
  }
  g_c1ad4 = millis();
  WiFiClientSecure client;
  HTTPClient http;
  https_prepare(http, client);
  String cur = g_c3114;
  cur.toLowerCase();
  String url = String("https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=") + cur;
  String p;
  if (!http.begin(client, String(url))) {
    g_c1ad0 = 10800000;
    led_phrase(p, g_c34b0, 0x31);
    g_c1ad8 = String("    ") + p + String("Bitcoin: Connection failed");
    return;
  }
  http.addHeader(String("Content-Type"), String("text/plain"), false, true);
  int code = http.GET();
  if (code != 200) {
    http.end();
    if (code < 1) g_c3318 = 1;
    g_c1ad0 = 10800000;
    led_phrase(p, g_c34b0, 0x31);
    g_c1ad8 = String("    ") + p + String("Bitcoin: Server not responding");
    return;
  }
  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, http.getStream());
  http.end();
  if (err) {
    g_c1ad0 = 10800000;
    led_phrase(p, g_c34b0, 0x31);
    g_c1ad8 = String("    ") + p + String("Bitcoin: Invalid server response");
    dbg_ws_print(String("JSON parse error: ") + err.c_str());
    return;
  }
  if (!doc["bitcoin"][cur].is<float>() && !doc["bitcoin"][cur].is<double>()) {
    g_c1ad0 = 10800000;
    led_phrase(p, g_c34b0, 0x31);
    g_c1ad8 = String("    ") + p + String("Bitcoin: Invalid rate format");
    dbg_ws_print(String("Unexpected CoinGecko response format"));
    return;
  }
  float rate = doc["bitcoin"][cur].as<float>();
  g_bf044 = rate;
  ws.printfAll(String(String("{\"btc_rate\": \"") + String(g_bf044, 2) + String("\"}")).c_str());
  led_phrase(p, g_c34b0, 0x31);
  g_c1ad8 = String("    ") + p + String("Bitcoin");
  if (g_c3114.equals(String("EUR"))) {
    led_phrase(p, g_c34b0, 0x28);
    g_c1ad8 += String(" = ") + String(rate, 2) + p;
  } else if (g_c3114.equals(String("USD"))) {
    g_c1ad8 += String(" = ") + String(rate, 2) + String(" $");
  } else if (g_c3114.equals(String("UAH"))) {
    led_phrase(p, g_c34b0, 0x32);
    g_c1ad8 += String(" = ") + String(rate, 2) + p;
  } else {
    g_c1ad8 += String(" = ") + String(rate, 2) + String(" ") + g_c3114;
  }
  dbg_ws_print(g_c1ad8);
  g_c1ad0 = 21600000;
}
