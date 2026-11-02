// Новини (GNews, NewsData), анекдоти/афоризми/статуси (rzhunemogu.ru), YouTube (лічильники каналу).
// EN: News (GNews, NewsData), jokes/quotes, YouTube channel counters (from 2.7.1).
// Перенесено з 2.7.1 (адреси в коментарях).
#include "fw.h"

// @400e19d8: HTTPS без перевірки сертифіката, тайм-аути 15 с
void https_prepare15(HTTPClient &http, WiFiClientSecure &client) {
  client.setCACert(NULL);
  client.setTimeout(15000);
  http.setTimeout(15000);
  http.useHTTP10(true);
}
// @400e1a0c: порожньо для "", "null", "NULL"
void news_clean(String &out, const String &s) {
  if (!s.equals("") && !s.equals("null") && !s.equals("NULL")) {
    out = s;
    return;
  }
  out = String("");
}
// @400f6ad8: «Новини: заголовок [/ Опис: опис] [ - вміст]»
void news_compose(String &out, const String &title, const String &desc, const String &content) {
  String t, d, c;
  news_clean(t, title);
  news_clean(d, desc);
  news_clean(c, content);
  String p;
  if (!t.equals("")) {
    led_phrase(p, g_c34b0, 0x50);
    String s = p + String(": ") + t + String(" ");
    if (g_c30c6 == 1) {
      if (d != "") {
        led_phrase(p, g_c34b0, 0x5a);
        s += String("/ ") + p + String(": ") + d;
      }
      if (c != "") s += String(" - ") + c;
    }
    out = s;
  } else {
    led_phrase(p, g_c34b0, 0x50);
    out = p + String(": No data");
  }
}
// нове (1.1.3): GNews на безкоштовному тарифі віддає лише початок тексту статті (≈260 знаків, обірваний посеред слова)
// і дописує «... [2072 chars]» — табло прокручувало цей хвіст. Прибираємо його й закінчуємо текст на останньому повному
// реченні з другої половини тексту, а якщо такого немає — на останньому цілому слові з «...».
static void news_cut_tail(String &c) {
  c.trim();                          // переведення рядка в кінці text_typography_fix робить пробілом
  int n = c.length();
  int b = c.lastIndexOf('[');
  if (b < 0 || !c.endsWith(" chars]") || b + 1 >= n - 7) return;
  for (int i = b + 1; i < n - 7; i++)
    if ((c[i] < '0' || c[i] > '9') && c[i] != '+') return;
  c.remove(b);
  c.trim();
  if (c.endsWith("...")) c.remove(c.length() - 3);
  c.trim();
  n = c.length();
  if (n == 0) return;
  for (int i = n - 1; i >= n / 2; i--) {
    char ch = c[i];
    if (ch != '.' && ch != '!' && ch != '?') continue;
    int e = i + 1;
    if (e < n && (c[e] == '\'' || c[e] == '"' || c[e] == ')')) e++;   // лапка чи дужка після кінця речення
    if (e < n && c[e] != ' ') continue;
    if (ch == '.') {                 // не ініціал («Д. Трамп») і не «т. ч.»: перед крапкою більше однієї літери
      int s = c.lastIndexOf(' ', i) + 1, k = 0;
      for (int j = s; j < i; j++)
        if ((c[j] & 0xc0) != 0x80) k++;
      if (k < 2) continue;
    }
    c.remove(e);
    return;
  }
  int s = c.lastIndexOf(' ');
  if (s > 0) c.remove(s);
  while (c.length() && strchr(",;:-", c[c.length() - 1])) c.remove(c.length() - 1);
  c.trim();
  c += "...";
}
static void news_err(const char *msg) {
  String p;
  led_phrase(p, g_c34b0, 0x50);
  g_c30fc = p + String(msg);
}
// спільна частина двох джерел новин
static void news_fetch(const String &url, const char *arr, const char *third, uint32_t okMs, uint32_t errMs, bool logCode) {
  HTTPClient http;
  WiFiClientSecure client;
  https_prepare15(http, client);
  if (!http.begin(client, String(url))) {
    g_c1a80 = errMs;
    news_err(": Server not responding");
    return;
  }
  http.addHeader(String("Content-Type"), String("application/json"), false, true);
  int code = http.GET();
  if (code != 200) {
    http.end();
    if (code < 1) g_c3318 = 1;
    if (logCode) {
      String p;
      led_phrase(p, g_c34b0, 0x50);
      dbg_ws_print(p + String(": Server not responding - code: ") + String(code, 10));
    }
    g_c1a80 = errMs;
    news_err(": Server not responding");
    return;
  }
  JsonDocument filter;
  filter[arr][0]["title"] = true;
  filter[arr][0]["description"] = true;
  filter[arr][0][third] = true;
  JsonDocument doc;
  if (!deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter))) {
    http.end();
    JsonArray a = doc[arr].as<JsonArray>();
    if (!a.isNull() && a.size() != 0) {
      JsonObject o = a[0].as<JsonObject>();
      String t, d, c;
      if (!o["title"].isNull()) text_typography_fix(t, o["title"].as<String>());
      else t = String("");
      if (!o["description"].isNull()) text_typography_fix(d, o["description"].as<String>());
      else d = String("");
      if (!o[third].isNull()) text_typography_fix(c, o[third].as<String>());
      else c = String("");
      news_cut_tail(c);
      String s;
      news_compose(s, t, d, c);
      g_c30fc = s;
      dbg_ws_print(g_c30fc);
      g_c1a80 = okMs;
      return;
    }
    g_c1a80 = errMs;
    news_err(": No data");
  } else {
    http.end();
    g_c1a80 = errMs;
    news_err(": No data");
  }
}
// @40103b90: GNews (раз на 15 хв; помилка — 10 с)
void news_gnews() {
  if (millis() - g_c1a84 < g_c1a80) return;
  if (heap_max_block() < 30000) {
    g_c30fc = String("");
    dbg_ws_print(String("News: low memory — freed previous result, waiting for free block"));
    return;
  }
  g_c1a84 = millis();
  String url = String("https://gnews.io/api/v4/top-headlines?country=") + g_c30c8 + String("&apikey=") + g_c30e0 + String("&pageSize=1&max=1&category=") + g_c30d4;
  // нове (1.1.3): для України — новини українською (без lang GNews давав і російськомовні статті; NewsData в автора
  // й так з language=uk). Для інших країн — як в автора, мову не задаємо.
  if (g_c30c8.equals("ua")) url += String("&lang=uk");
  news_fetch(url, "articles", "content", 900000, 10000, true);
}
// @40104868: NewsData (раз на 10 хв)
void news_newsdata() {
  if (millis() - g_c1a84 < g_c1a80) return;
  if (heap_max_block() < 30000) {
    g_c30fc = String("");
    dbg_ws_print(String("News: low memory — freed previous result, waiting for free block"));
    return;
  }
  g_c1a84 = millis();
  String url = String("https://newsdata.io/api/1/latest?country=") + g_c30c8 + String("&apikey=") + g_c30e0 + String("&size=1&language=uk") + String("&category=") + g_c30d4;
  news_fetch(url, "results", "source_id", 600000, 600000, false);
}
// @40106db8: анекдот / афоризм / статус (g_c2eea: 2/3/4; раз на 15 хв)
void jokes_fetch() {
  if (millis() - g_c1ac0 < g_c1abc) return;
  g_c1ac0 = millis();
  WiFiClient client;
  HTTPClient http;
  String url(""), prefix("");
  if (g_c2eea == 3) { url = "http://rzhunemogu.ru/RandJSON.aspx?CType=4"; prefix = "Афоризм: "; }
  else if (g_c2eea == 4) { url = "http://rzhunemogu.ru/RandJSON.aspx?CType=8"; prefix = "Статус: "; }
  else if (g_c2eea == 2) { url = "http://rzhunemogu.ru/RandJSON.aspx?CType=1"; prefix = "Анекдот: "; }
  if (!http.begin(client, String(url))) {
    g_c1abc = 900000;
    return;
  }
  http.addHeader(String("Content-Type"), String("text/plain"), false, true);
  int code = http.GET();
  if (http.getSize() > 6000) {
    http.end();
    g_c1abc = 600000;
    return;
  }
  String s = http.getString();
  http.end();
  if (code == 200) {
    s.replace(String("’"), String("'"));
    s.replace(String("“"), String("'"));
    s.replace(String("”"), String("'"));
    s.replace(String("`"), String("'"));
    s.replace(String("‘"), String("'"));
    s.replace(String("„"), String("'"));
    s.replace(String("\\"), String(""));
    s.replace(String("•"), String("-"));
    s.replace(String("»"), String("'"));
    s.replace(String("«"), String("'"));
    s.replace(String("…"), String("..."));
    s.replace(String("–"), String("-"));
    s.replace(String("—"), String("-"));
    JsonDocument doc;
    s = led_to_utf8(s);
    deserializeJson(doc, s);
    g_c1ac4 = String(prefix) + doc["content"].as<String>();
    dbg_ws_print(g_c1ac4);
    g_c1abc = 900000;
    return;
  }
  if (code < 1) g_c3318 = 1;
  g_c1abc = 5000;
  String p;
  led_phrase(p, g_c34b0, 0x84);
  g_c1ac4 = p + ": Server not responding";
}
// @4010120c: YouTube — підписники й перегляди каналу g_c2f00 (ключ g_c2f0c)
void youtube_fetch() {
  if (g_c3319 != 0 || millis() - g_c1c74 < g_c1c70) return;
  g_c1c74 = millis();
  WiFiClientSecure client;
  YoutubeApi api(String(g_c2f0c), client);
  dbg_ws_print(String("YouTube: starting"));
  if (!api.getChannelStatistics(String(g_c2f00))) {
    dbg_ws_print(String("YouTube: not started — something is wrong, check the API key or module"));
  } else {
    g_c1c7c = api.channelStats.subscriberCount;
    g_c1c78 = api.channelStats.viewCount;
    String p;
    led_phrase(p, g_c34b0, 0x5b);
    g_c1c80 = String("Youtube: ") + p + String(g_c1c7c, 10);
    led_phrase(p, g_c34b0, 0x5b);
    g_c1c80 = String("Youtube: ") + p + String(g_c1c7c, 10);
    led_phrase(p, g_c34b0, 0x5c);
    g_c1c80 += String("  ") + p + String(g_c1c78, 10);
  }
  client.stop();
  dbg_ws_print(g_c1c80);
}
