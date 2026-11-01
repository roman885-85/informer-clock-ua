// Погода: OpenWeatherMap і Яндекс («зараз»), Weatherbit («зараз» і «завтра»).
// EN: Weather: OpenWeatherMap and Yandex (now), Weatherbit (now and tomorrow), from 2.7.1.
// Текст для табло (з гліфами), поля для веб-інтерфейсу {"weather_now": …} тощо. Перенесено з 2.7.1 (адреси в коментарях).
#include "fw.h"

// @400eea84: поля «зараз» у JSON
void weather_now_json(String &out, const String *w) {
  JsonDocument doc;
  out = "";
  out.reserve(0xc0);
  doc["event"] = w[0];
  if (w[1] != "") doc["city"] = w[1];
  if (w[2] != "") doc["wnow"] = w[2];
  if (w[3] != "") doc["temp"] = w[3];
  if (w[4] != "") doc["press"] = w[4];
  if (w[5] != "") doc["wind"] = w[5];
  if (w[6] != "") doc["wspeed"] = w[6];
  if (w[7] != "") doc["hum"] = w[7];
  serializeJson(doc, out);
}
// @400eec1c: поля «завтра» у JSON
void weather_tom_json(String &out, const String *w) {
  JsonDocument doc;
  out = "";
  out.reserve(0xc0);
  doc["event"] = w[0];
  if (w[1] != "") doc["tom"] = w[1];
  if (w[2] != "") doc["temp"] = w[2];
  if (w[3] != "") doc["press"] = w[3];
  if (w[4] != "") doc["wind"] = w[4];
  if (w[5] != "") doc["wspeed"] = w[5];
  if (w[6] != "") doc["hum"] = w[6];
  serializeJson(doc, out);
}
// @400e1d70: HTTPS без перевірки сертифіката, тайм-аути 7 с
void https_prepare(HTTPClient &http, WiFiClientSecure &client) {
  client.setCACert(NULL);
  client.setTimeout(7000);
  http.setTimeout(7000);
  http.useHTTP10(true);
}
// напрям вітру: фраза + гліф стрілки (у трьох функціях автора — однаковий код)
static String wind_dir_text(int deg) {
  String s("");
  String p;
  if ((unsigned)(deg - 23) < 322) {
    if ((unsigned)(deg - 23) < 46) { led_phrase(p, g_c34b0, 0x3e); s = p + " \x12"; }
  } else {
    led_phrase(p, g_c34b0, 0x3d); s = p + " \x19";
  }
  if ((unsigned)(deg - 69) < 46) { led_phrase(p, g_c34b0, 0x3f); s = p + " \x1b"; }
  if ((unsigned)(deg - 115) < 46) { led_phrase(p, g_c34b0, 0x40); s = p + " \x11"; }
  if ((unsigned)(deg - 161) < 46) { led_phrase(p, g_c34b0, 0x41); s = p + " \x18"; }
  if ((unsigned)(deg - 207) < 46) { led_phrase(p, g_c34b0, 0x42); s = p + " \x16"; }
  if ((unsigned)(deg - 253) < 46) { led_phrase(p, g_c34b0, 0x43); s = p + " \x1a"; }
  if ((unsigned)(deg - 299) < 46) { led_phrase(p, g_c34b0, 0x44); s = p + " \x17"; }
  return s;
}
// одиниці (g_c3159: 0 — °C, 1 — °F, 2 — K): символ і параметр запиту
static void weather_units(String &units, String &sym, const char *c, const char *f, const char *k) {
  if (g_c3159 == 0) { sym = "C"; units = c; }
  else if (g_c3159 == 1) { sym = "F"; units = f; }
  else if (g_c3159 == 2) { sym = "K"; units = k; }
}
// гліф стану неба для коду Weatherbit
static void wb_icon(String &icon, unsigned code) {
  const char *g;
  switch (code) {
    case 200: case 201: case 202: case 230: case 231: case 232: case 233: g = " \x07 "; break;
    case 300: case 301: case 302: case 500: case 501: case 521: g = " \x06 "; break;
    case 502: case 511: case 520: case 522: g = " \x05 "; break;
    case 600: case 601: case 602: case 610: case 611: case 612: case 621: case 622: case 623: g = " \x08 "; break;
    case 700: case 711: case 721: case 731: case 741: case 751: g = " \x09 "; break;
    case 800: g = " \x01 "; break;
    case 801: case 802: g = " \x02 "; break;
    case 803: g = " \x03 "; break;
    case 804: case 900: g = " \x04 "; break;
    default: return;
  }
  icon = g;
}
// початок рядка «Погода зараз: [місто] опис гліф»
static String weather_head(const String &desc, const String &icon, const String &apiCity) {
  String p1, p2;
  led_phrase(p1, g_c34b0, 0x45);
  if (g_c3156 == 0) return p1 + " " + desc + icon;
  led_phrase(p2, g_c34b0, 0x46);
  if (g_c315c == "") return p1 + p2 + apiCity + " " + desc + icon;
  return p1 + p2 + g_c315c + " " + desc + icon;
}

// @4010c30c: OpenWeatherMap «зараз» (раз на годину; помилка — 10 хв; cod ≠ 200 — 1 хв)
void weather_owm(float lat, float lon, const String &key) {
  if (g_bec0c != g_c3156) g_c1a7c = 0;
  if (millis() - g_c1a78 < g_c1a7c) return;
  g_c1a78 = millis();
  String units(""), sym("");
  weather_units(units, sym, "metric", "imperial", "");
  g_bec0c = g_c3156;
  String lang("");
  switch (g_c2eef) {
    case 0: lang = "ru"; break;
    case 1: lang = "en"; break;
    case 2: lang = "en"; break;
    case 3: lang = "ua"; break;
    case 4: lang = "ro"; break;
    case 5: lang = "bg"; break;
    case 6: lang = "pl"; break;
  }
  String icon("");
  WiFiClient client;
  HTTPClient http;
  client.setTimeout(7000);
  http.setTimeout(7000);
  http.useHTTP10(true);
  String url = String("http://api.openweathermap.org/data/2.5/weather?lat=") + String(lat, 2) + String("&lon=") + String(lon, 2) + String("&APPID=") + key + String("&lang=") + String(lang) + String("&units=") + units;
  if (!http.begin(client, url)) {
    g_c1a7c = 600000;
    return;
  }
  http.addHeader(String("Content-Type"), String("application/json"), false, true);
  int code = http.GET();
  if (code != 200) {
    http.end();
    if (code < 1) g_c3318 = 1;
    g_c1a7c = 600000;
    return;
  }
  JsonDocument filter;
  filter["weather"][0]["description"] = true;
  filter["weather"][0]["icon"] = true;
  filter["main"]["temp"] = true;
  filter["main"]["humidity"] = true;
  filter["main"]["pressure"] = true;
  filter["wind"]["speed"] = true;
  filter["wind"]["deg"] = true;
  filter["cod"] = true;
  filter["name"] = true;
  JsonDocument doc;
  if (deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter))) {
    http.end();
    g_c1a7c = 600000;
    return;
  }
  http.end();
  String desc = doc["weather"][0]["description"].as<String>();
  float temp = doc["main"]["temp"].as<float>();
  uint8_t hum = doc["main"]["humidity"].as<uint8_t>();
  int pressHpa = doc["main"]["pressure"].as<int>();
  float windSpd = doc["wind"]["speed"].as<float>();
  int windDeg = doc["wind"]["deg"].as<int>();
  String iconCode = doc["weather"][0]["icon"].as<String>();
  g_c1a7c = (doc["cod"].as<int>() == 200) ? 3600000 : 60000;
  unsigned press = pressHpa / 1.33322;
  dbg_ws_print(String("OpenWeatherMap: desc=") + desc + "  temp=" + String(temp, 2) + "  hum=" + String(hum, 10) + "  press=" + String((int)press, 10) + "  windSpd=" + String(windSpd, 2) + "  windDeg=" + String(windDeg, 10));
  String wind = wind_dir_text(windDeg);
  iconCode = iconCode.substring(0, iconCode.length() - 1);
  switch (iconCode.toInt()) {
    case 1: icon = " \x01 "; break;
    case 2: icon = " \x02 "; break;
    case 3: icon = " \x03 "; break;
    case 4: icon = " \x04 "; break;
    case 9: icon = " \x05 "; break;
    case 10: icon = " \x06 "; break;
    case 11: icon = " \x07 "; break;
    case 13: icon = " \x08 "; break;
    case 50: icon = " \x09 "; break;
  }
  String city;
  if (g_c3156 == 0) city = String("");
  else if (g_c315c == "") city = doc["name"].as<String>();
  else city = g_c315c;
  String descCopy = desc;
  g_c1a6c = weather_head(desc, icon, doc["name"].as<String>());
  String tempStr("");
  if (temp <= 0.0) g_c1a6c += String(" ");
  else {
    g_c1a6c += String("+");
    tempStr = String("+");
  }
  g_c1a6c += String(temp, 1) + String("\x1c") + sym;
  String p;
  led_phrase(p, g_c34b0, 0x47);
  g_c1a6c += p + String(hum, 10) + "% ";
  led_phrase(p, g_c34b0, 0x48);
  g_c1a6c += p + String(round((double)press), 0);
  tempStr += String(temp, 1) + String("°") + sym;
  String humStr = String(hum, 10) + String("% ");
  String pressStr = String(round((double)press), 0);
  if (g_c3159 == 1) {
    g_c1a6c += String("in");
    pressStr += String("in");
  } else {
    led_phrase(p, g_c34b0, 0x4b); g_c1a6c += p;
    led_phrase(p, g_c34b0, 0x4b); pressStr += p;
  }
  led_phrase(p, g_c34b0, 0x49);
  g_c1a6c += p + wind + " " + String(windSpd, 1);
  String windShort = wind.substring(0, wind.length() - 2);
  String spdStr = String(windSpd, 1);
  if (g_c3159 == 1) {
    g_c1a6c += String("mph");
    spdStr += String("mph");
  } else {
    led_phrase(p, g_c34b0, 0x4a); g_c1a6c += p;
    led_phrase(p, g_c34b0, 0x4a); spdStr += p;
  }
  weather_set8(g_c3224, String("ok"), city, descCopy, tempStr, pressStr, windShort, spdStr, humStr);
  String j;
  weather_now_json(j, g_c3224);
  dbg_ws_print(j);
  weather_now_json(j, g_c3224);
  ws_send(String("{\"weather_now\": ") + j + String("}"));
}

// @401054a8: Яндекс (час/погода з yandex.com/time/sync.json; раз на 15 хв; помилка — 5 с)
void weather_yandex(const String &geo) {
  if (millis() - g_c1a30 < g_c1a2c) return;
  if (heap_max_block() < 40000) {
    dbg_ws_print(String("Yandex: skip TLS — low contiguous memory (need 4096 buffer)"));
    return;
  }
  g_c1a30 = millis();
  dbg_ws_print(String("Starting Yandex"));
  HTTPClient http;
  String url = String("https://yandex.com/time/sync.json?geo=") + geo;
  WiFiClientSecure client;
  https_prepare(http, client);
  if (!http.begin(client, url)) {
    g_c1a2c = 5000;
    g_c1a34 = String("Yandex: Server not responding");
    return;
  }
  http.addHeader(String("Host"), String("yandex.com"), false, true);
  http.addHeader(String("Accept"), String("*/*"), false, true);
  http.addHeader(String("Accept-Encoding"), String("identity"), false, true);
  http.addHeader(String("Connection"), String("keep-alive"), false, true);
  dbg_ws_print(String("Yandex client IP connected"));
  int code = http.GET();
  if (code < 0) {
    http.end();
    g_c3318 = 1;
    g_c1a2c = 5000;
    g_c1a34 = String("Yandex: Server not responding");
    dbg_ws_print(String("Yandex: server is not responding"));
    return;
  }
  JsonDocument filter;
  filter["clocks"][geo]["weather"]["temp"] = true;
  filter["clocks"][geo]["weather"]["icon"] = true;
  filter["clocks"][geo]["name"] = true;
  JsonDocument doc;
  if (deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter))) {
    http.end();
    g_c1a2c = 5000;
    g_c1a34 = String("Yandex: Server not responding");
    return;
  }
  http.end();
  float temp = doc["clocks"][geo]["weather"]["temp"].as<float>();
  String name = doc["clocks"][geo]["name"].as<String>();
  String iconStr = doc["clocks"][geo]["weather"]["icon"].as<String>();
  char buf[20];
  strcpy(buf, iconStr.c_str());
  String desc(""), icon("");
  char *p = buf;
  while (char *tok = strtok(p, "-")) {
    if (String(tok).equals("skc")) { desc.concat("Ясно "); icon = " \x01 "; }
    else if (String(tok).equals("ovc")) { desc.concat("Пасмурно "); icon = " \x02 "; }
    else if (String(tok).equals("bkn")) { desc.concat("Облачно "); icon = " \x03 "; }
    else if (String(tok).equals("ra")) { desc.concat("Дождь "); icon = " \x05 "; }
    else if (String(tok).equals("ts")) { desc.concat("Гроза "); icon = " \x07 "; }
    else if (String(tok).equals("sn")) { desc.concat("Снег "); icon = " \x08 "; }
    else if (String(tok).equals("bl")) { desc.concat("Метель "); icon = " \x08 "; }
    else if (String(tok).equals("fg")) { desc.concat("Туман "); icon = " \x09 "; }
    dbg_ws_print(desc);
    p = nullptr;
  }
  String city;
  if (g_c3156 == 0) city = String("");
  else city = (g_c315c != "") ? g_c315c : name;
  g_c1a34 = weather_head(desc, icon, name);
  String tempStr("");
  if (temp <= 0.0) g_c1a34 += String(" ");
  else {
    g_c1a34 += String("+");
    tempStr = String("+");
  }
  g_c1a34 += String(temp, 1) + String("\x1c") + String("C");
  tempStr += String(temp, 1) + String("°") + String("C");
  weather_set8(g_c3224, String("ok"), city, desc, tempStr, String(""), String(""), String(""), String(""));
  String j;
  weather_now_json(j, g_c3224);
  dbg_ws_print(String(g_c1a34) + "  | " + j);
  weather_now_json(j, g_c3224);
  ws_send(String("{\"weather_now\": ") + j + String("}"));
  g_c1a2c = 900000;
}

// @40111150: Weatherbit «зараз» (раз на годину; помилка — 10 хв)
void weather_weatherbit_now(float lat, float lon, const String &key) {
  if (g_bec0c != g_c3156) g_c1a64 = 0;
  if (millis() - g_c1a68 < g_c1a64) return;
  g_c1a68 = millis();
  String units(""), sym("");
  weather_units(units, sym, "M", "I", "S");
  g_bec0c = g_c3156;
  String lang("");
  switch (g_c2eef) {
    case 0: lang = "ru"; break;
    case 1: lang = "en"; break;
    case 2: lang = "be"; break;
    case 3: lang = "uk"; break;
    case 4: lang = "ro"; break;
    case 5: lang = "bg"; break;
    case 6: lang = "pl"; break;
  }
  String icon("");
  WiFiClient client;
  HTTPClient http;
  client.setTimeout(7000);
  http.setTimeout(7000);
  http.useHTTP10(true);
  String url = String("http://api.weatherbit.io/v2.0/current?lat=") + String(lat, 2) + String("&lon=") + String(lon, 2) + String("&key=") + key + String("&lang=") + String(lang) + String("&units=") + String(units);
  if (!http.begin(client, url)) {
    g_c1a64 = 600000;
    return;
  }
  http.addHeader(String("Content-Type"), String("text/plain"), false, true);
  int code = http.GET();
  if (code != 200) {
    dbg_ws_print(String("weatherbit: httpCode=") + String(code, 10) + " (403 = ключ невалиден/не активирован; 429 = лимит)");
    http.end();
    if (code < 1) g_c3318 = 1;
    g_c1a64 = 600000;
    return;
  }
  JsonDocument filter;
  filter["data"][0]["weather"]["description"] = true;
  filter["data"][0]["weather"]["code"] = true;
  filter["data"][0]["city_name"] = true;
  filter["data"][0]["temp"] = true;
  filter["data"][0]["rh"] = true;
  filter["data"][0]["pres"] = true;
  filter["data"][0]["wind_spd"] = true;
  filter["data"][0]["wind_dir"] = true;
  JsonDocument doc;
  if (deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter))) {
    dbg_ws_print(String("weatherbit: JSON parse failed"));
    http.end();
    g_c1a64 = 600000;
    return;
  }
  http.end();
  String desc = doc["data"][0]["weather"]["description"].as<String>();
  String apiCity = doc["data"][0]["city_name"].as<String>();
  float temp = doc["data"][0]["temp"].as<float>();
  uint8_t hum = doc["data"][0]["rh"].as<uint8_t>();
  float pres = doc["data"][0]["pres"].as<float>();
  float windSpd = doc["data"][0]["wind_spd"].as<float>();
  int windDeg = doc["data"][0]["wind_dir"].as<int>();
  unsigned wcode = doc["data"][0]["weather"]["code"].as<unsigned>();
  pres = pres / 1.33322;
  String wind = wind_dir_text(windDeg);
  wb_icon(icon, wcode);
  String city;
  if (g_c3156 == 0) city = String("");
  else city = (g_c315c != "") ? g_c315c : apiCity;
  g_c1a54 = weather_head(desc, icon, apiCity);
  String tempStr("");
  if (temp <= 0.0) g_c1a54 += String(" ");
  else {
    g_c1a54 += String("+");
    tempStr = String("+");
  }
  g_c1a54 += String(temp, 1) + String("\x1c") + sym;
  String p;
  led_phrase(p, g_c34b0, 0x47);
  g_c1a54 += p + String(hum, 10) + String("% ");
  led_phrase(p, g_c34b0, 0x48);
  g_c1a54 += p + String(round(pres), 0);
  tempStr += String(temp, 1) + String("°") + sym;
  String humStr = String(hum, 10) + String("% ");
  String pressStr = String(round(pres), 0);
  if (g_c3159 == 1) {
    g_c1a54 += String("in");
    pressStr += String("in");
  } else {
    led_phrase(p, g_c34b0, 0x4b); g_c1a54 += p;
    led_phrase(p, g_c34b0, 0x4b); pressStr += p;
  }
  led_phrase(p, g_c34b0, 0x49);
  g_c1a54 += p + wind + " " + String(windSpd, 1);
  String windShort = wind.substring(0, wind.length() - 2);
  String spdStr = String(windSpd, 1);
  if (g_c3159 == 1) {
    g_c1a54 += String("mph");
    spdStr += String("mph");
  } else {
    led_phrase(p, g_c34b0, 0x4a); g_c1a54 += p;
    led_phrase(p, g_c34b0, 0x4a); spdStr += p;
  }
  weather_set8(g_c31c4, String("ok"), city, desc, tempStr, pressStr, windShort, spdStr, humStr);
  String j;
  weather_now_json(j, g_c31c4);
  dbg_ws_print(j);
  weather_now_json(j, g_c31c4);
  ws_send(String("{\"weather_now_wb\": ") + j + String("}"));
  g_c1a64 = 3600000;
}

// @40113910: Weatherbit «завтра» (прогноз на 2 дні, береться другий; раз на годину; помилка — 10 хв)
void weather_weatherbit_tomorrow(float lat, float lon, const String &key) {
  if (g_bec0c != g_c3156) g_c1a40 = 0;
  if (millis() - g_c1a44 < g_c1a40) return;
  g_c1a44 = millis();
  String units(""), sym("");
  weather_units(units, sym, "M", "I", "S");
  g_bec0c = g_c3156;
  String lang("");
  switch (g_c2eef) {
    case 0: lang = "ru"; break;
    case 1: lang = "en"; break;
    case 2: lang = "be"; break;
    case 3: lang = "uk"; break;
    case 4: lang = "ro"; break;
    case 5: lang = "bg"; break;
    case 6: lang = "pl"; break;
  }
  String icon("");
  WiFiClient client;
  HTTPClient http;
  client.setTimeout(7000);
  http.setTimeout(7000);
  http.useHTTP10(true);
  String url = String("http://api.weatherbit.io/v2.0/forecast/daily?lat=") + String(lat, 2) + String("&lon=") + String(lon, 2) + String("&key=") + key + String("&lang=") + String(lang) + String("&units=") + String(units) + String("&days=2");
  if (!http.begin(client, url)) {
    g_c1a40 = 600000;
    return;
  }
  http.addHeader(String("Content-Type"), String("text/plain"), false, true);
  int code = http.GET();
  if (code != 200) {
    http.end();
    if (code < 1) g_c3318 = 1;
    g_c1a40 = 600000;
    return;
  }
  JsonDocument filter;
  filter["data"][0]["weather"]["description"] = true;
  filter["data"][0]["weather"]["code"] = true;
  filter["data"][0]["rh"] = true;
  filter["data"][0]["pres"] = true;
  filter["data"][0]["wind_spd"] = true;
  filter["data"][0]["wind_dir"] = true;
  filter["data"][0]["min_temp"] = true;
  filter["data"][0]["max_temp"] = true;
  JsonDocument doc;
  if (deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter))) {
    http.end();
    g_c1a40 = 600000;
    return;
  }
  http.end();
  String desc = doc["data"][1]["weather"]["description"].as<String>();
  uint8_t hum = doc["data"][1]["rh"].as<uint8_t>();
  float pres = doc["data"][1]["pres"].as<float>();
  float windSpd = doc["data"][1]["wind_spd"].as<float>();
  int windDeg = doc["data"][1]["wind_dir"].as<int>();
  unsigned wcode = doc["data"][1]["weather"]["code"].as<unsigned>();
  float tmin = doc["data"][1]["min_temp"].as<float>();
  float tmax = doc["data"][1]["max_temp"].as<float>();
  pres = pres / 1.33322;
  String wind = wind_dir_text(windDeg);
  wb_icon(icon, wcode);
  String p, p2;
  led_phrase(p, g_c34b0, 0x4c);
  led_phrase(p2, g_c34b0, 0x4d);
  g_c1a48 = p + desc + icon + " " + p2;
  String tempStr;
  led_phrase(tempStr, g_c34b0, 0x4d);
  if (tmin == 0.0 || tmax <= 0.0) {
    if (0.0 < tmax) {
      g_c1a48 += String("+");
      tempStr += String("+");
    }
    g_c1a48 += String(tmax, 1);
    led_phrase(p, g_c34b0, 0x7c);
    g_c1a48 += String("\x1c") + sym + p;
    tempStr += String(tmax, 1);
    led_phrase(p, g_c34b0, 0x7c);
    tempStr += String("°") + sym + p;
    g_c1a48 += String(tmin, 1) + String("\x1c") + sym + " ";
    tempStr += String(tmin, 1) + String("°") + sym + " ";
  } else {
    if (0.0 < tmin) {
      g_c1a48 += String("+");
      tempStr += String("+");
    }
    g_c1a48 += String(tmin, 1);
    led_phrase(p, g_c34b0, 0x7c);
    g_c1a48 += String("\x1c") + sym + p;
    tempStr += String(tmin, 1);
    led_phrase(p, g_c34b0, 0x7c);
    tempStr += String("°") + sym + p;
    g_c1a48 += String("+");
    tempStr += String("+");
    g_c1a48 += String(tmax, 1) + String("\x1c") + sym + " ";
    tempStr += String(tmax, 1) + String("°") + sym + " ";
  }
  led_phrase(p, g_c34b0, 0x47);
  g_c1a48 += p + String(hum, 10) + String("% ");
  led_phrase(p, g_c34b0, 0x48);
  g_c1a48 += p + String(round(pres), 0);
  String humStr = String(hum, 10) + String("% ");
  String pressStr = String(round(pres), 0);
  if (g_c3159 == 1) {
    g_c1a48 += String("in");
    pressStr += String("in");
  } else {
    led_phrase(p, g_c34b0, 0x4b); g_c1a48 += p;
    led_phrase(p, g_c34b0, 0x4b); pressStr += p;
  }
  led_phrase(p, g_c34b0, 0x49);
  g_c1a48 += p + wind + " " + String(windSpd, 1);
  String windShort = wind.substring(0, wind.length() - 2);
  String spdStr = String(windSpd, 1);
  if (g_c3159 == 1) {
    g_c1a48 += String("mph");
    spdStr += String("mph");
  } else {
    led_phrase(p, g_c34b0, 0x4a); g_c1a48 += p;
    led_phrase(p, g_c34b0, 0x4a); spdStr += p;
  }
  weather_set7(g_c3170, String("ok"), desc, tempStr, pressStr, windShort, spdStr, humStr);
  String j;
  weather_tom_json(j, g_c3170);
  dbg_ws_print(j);
  weather_tom_json(j, g_c3170);
  ws_send(String("{\"weather_tom\": ") + j + String("}"));
  g_c1a40 = 3600000;
}
