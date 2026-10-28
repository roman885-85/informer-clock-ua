// Веб-сервер: маршрути й обробники (перенесено з 2.7.1). Відповіді й коди — як в оригіналі.
// EN: Web server: routes and handlers (from 2.7.1).
#include "fw.h"
#include "web_assets.h"

static String pval(AsyncWebServerRequest *r, const char *n) { return r->getParam(n)->value(); }

// ---------- реєстрація маршрутів ----------
// @400d61ac
void routes_register_mass() { server.on("/mass", HTTP_ANY, h_mass); }
// @400d6208
void routes_register_1() {
  server.on("/content", HTTP_ANY, h_content);
  server.on("/wifi-connect", HTTP_ANY, h_wifi_connect);
  server.on("/turn-off-ap", HTTP_ANY, h_turn_off_ap);
  server.on("/log", HTTP_ANY, h_log);
}
// @400d62d4
void routes_register_2() {
  server.on("/reboot", HTTP_ANY, h_reboot);
  server.on("/forget-network", HTTP_ANY, h_forget_network);
  server.on("/connect-saved", HTTP_ANY, h_connect_saved);
  server.on("/net-autoconnect", HTTP_ANY, h_net_autoconnect);
  server.on("/ntpsync", HTTP_ANY, h_ntpsync);
  server.on("/factory", HTTP_ANY, h_factory);
  server.on("/config.json", HTTP_ANY, h_config_json);
  server.on("/upload", HTTP_POST, h_upload_done, h_upload_file);
}
// @400d6468
void routes_register_3() {
  server.onNotFound(h_not_found);
  server.on("/setmantime", HTTP_ANY, h_setmantime);
}
// @400d64e8 / @400d6544
void routes_register_post() { server.on("/post", HTTP_ANY, h_post); }
void routes_register_post2() { server.on("/post2", HTTP_ANY, h_post2); }
// @400d65a0
void routes_register_catchall() { server.on("/*", HTTP_ANY, captive_request); }
// @400d65fc
void routes_register_lang() {
  server.on("/lang-list", HTTP_ANY, h_lang_list);
  server.on("/hist", HTTP_ANY, h_hist);
  server.on("/lang", HTTP_ANY, h_lang);
  server.on("/lang-upload", HTTP_POST, h_lang_upload_done, h_lang_upload_file);
  server.on("/lang-chunk", HTTP_POST, h_lang_chunk_done, nullptr, h_lang_chunk_body);
  server.on("/lang-delete", HTTP_ANY, h_lang_delete);
  server.on("/lang-led-status", HTTP_ANY, h_lang_led_status);
  server.on("/lang-led", HTTP_POST, h_lang_led_done, h_lang_led_file);
  server.on("/images/logo.svg", HTTP_GET, h_logo_svg);
  server.on("/clock.svg", HTTP_GET, h_clock_svg);
  server.on("/favicon.ico", HTTP_ANY, h_favicon);
}
// @400d6858
void routes_register_all() {
  routes_register_lang();
  routes_register_post();
  routes_register_post2();
  routes_register_2();
  routes_register_3();
  routes_register_mass();
  routes_register_1();
  routes_register_catchall();
}

// ---------- обробники ----------
// @400d3dd0
void h_wifi_connect(AsyncWebServerRequest *r) {
  g_c3374 = "connecting";
  g_c3380 = 1;
  r->send(200, String("text/plain"), String("ok"));
}
// @400d3ae4
void h_turn_off_ap(AsyncWebServerRequest *r) {
  WiFi.mode(WIFI_STA);
  r->send(200, String("text/plain"), String("ok"));
}
// @400d3c4c
void h_log(AsyncWebServerRequest *r) {
  if (!r->hasParam("clear")) {
    bool old = r->hasParam("old");
    if (g_c2e88 != 0 && g_c2e8a == (uint8_t)old) {
      r->send(200, String("text/plain; charset=utf-8"), g_c2e78);
      g_c2e88 = 0;
      g_c2e78 = String("");
      return;
    }
    g_c2e8a = (uint8_t)old;
    g_c2e8b = 1;
    r->send(0xca, String("text/plain; charset=utf-8"), String("__LOG_PREPARING__"));
  } else {
    g_c2e89 = 1;
    r->send(200, String("text/plain; charset=utf-8"), String("Журнал очищен"));
  }
}
// @400fd578
void h_reboot(AsyncWebServerRequest *r) {
  r->send(200, String("text/plain"), String("OK"));
  g_c2d28 = 1;
  config_save_atomic();
  df_silence();
  WiFi.disconnect(false);
  delay(2000);
  ESP.restart();
}
// @400d4bd0
void h_factory(AsyncWebServerRequest *r) {
  r->send(200, String("text/plain"), String("OK"));
  SPIFFS.format();
  WiFi.disconnect(false);
  delay(1000);
  df_silence();
  ESP.restart();
}
// @400d37b0
void h_ntpsync(AsyncWebServerRequest *r) {
  uint32_t now = millis();
  if (g_c3420 == 0 || now - g_c3420 > 14999) {
    if (g_c2d29 == 0 || g_c3319 != 0) r->send(200, String("application/json"), String("{\"status\":\"offline\"}"));
    else {
      g_c3424 = 1;
      g_c3420 = now;
      r->send(200, String("application/json"), String("{\"status\":\"ok\"}"));
    }
  } else {
    String s = String("{\"status\":\"cooldown\",\"retry\":") + String((uint32_t)((g_c3420 + 15999) - now) / 1000, 10) + "}";
    r->send(0x1ad, String("application/json"), s);
  }
}
// @400d369c
void h_connect_saved(AsyncWebServerRequest *r) {
  if (!r->hasParam("ssid")) { r->send(400, String("application/json"), String("{\"status\":\"no-ssid\"}")); return; }
  g_c3328 = pval(r, "ssid");
  r->send(200, String("application/json"), String("{\"status\":\"queued\"}"));
}
// @400d3584
void h_forget_network(AsyncWebServerRequest *r) {
  if (!r->hasParam("ssid")) { r->send(400, String("application/json"), String("{\"status\":\"no-ssid\"}")); return; }
  g_c3334 = pval(r, "ssid");
  r->send(200, String("application/json"), String("{\"status\":\"queued\"}"));
}
// @400d3938
void h_net_autoconnect(AsyncWebServerRequest *r) {
  if (!r->hasParam("ssid")) { r->send(400, String("application/json"), String("{\"status\":\"no-ssid\"}")); return; }
  g_c331c = pval(r, "ssid");
  uint8_t on = 0;
  if (r->hasParam("on")) { g_c331a = pval(r, "on") == "1"; on = g_c331a; }
  g_c331a = on;
  r->send(200, String("application/json"), String("{\"status\":\"queued\"}"));
}
// @400f4fb8
void h_config_json(AsyncWebServerRequest *r) {
  if (!r->client()->localIP().toString().equals("192.168.4.1")) {
    JsonDocument d;
    String pass;
    if (config_doc_load(d)) {
      JsonVariant v = d["pass"];
      pass = v.is<const char *>() ? String(v.as<const char *>()) : String("");
    }
    AsyncWebParameter *k = r->getParam("key");
    if (k == nullptr || k->value() != pass) {
      r->send(0x193, String("text/plain"), String("forbidden: wrong Wi-Fi password"));
      return;
    }
  }
  AsyncWebServerResponse *resp = r->beginResponse(200, String("application/json"), g_c2ef0);
  resp->addHeader("Content-Disposition", "attachment; filename=\"config.json\"");
  r->send(resp);
}
// @400d3bcc
void h_upload_done(AsyncWebServerRequest *r) {
  r->send(200, String("text/plain"), String("Config file uploaded, now rebooting... Please refresh the page"));
}
// @400fdddc
void h_upload_file(AsyncWebServerRequest *r, const String &filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (index == 0) {
    g_c2c3c = SPIFFS.open(String("/config_upload.json"), "w");
    g_c2c38 = 0;
    // "Upload: START /config_upload.json"
  }
  if (len != 0 && g_c2c3c) { g_c2c3c.write(data, len); g_c2c38 += len; }
  if (final) {
    if (g_c2c3c) g_c2c3c.close();
    // "Upload: END size=" + String(g_c2c38, 10)
    evlog_write(String("[CFG] upload received, size=") + String(g_c2c38, 10));
    g_c2e99 = 1;
  }
}
// @400f546c
void h_setmantime(AsyncWebServerRequest *r) {
  int n = r->params();
  if (n == 0) {
    r->send(400, String("application/json"), String("{ \"server\": \"error\" }"));
    // "WEB SERVER: empty request"
    return;
  }
  JsonDocument d;
  if (!config_doc_load(d)) { r->send(500, String("application/json"), String("{ \"server\": \"config-error\" }")); return; }
  for (int i = 0; i < n; i++) {
    AsyncWebParameter *p = r->getParam(i);
    d[p->name()] = p->value();
  }
  config_doc_store(d);
  r->send(200, String("application/json"), String("{ \"server\": \"ok\" }"));
  g_c2d28 = 1;
  manual_time_set();
  g_c2f21 = hour();
  int ov = d["onoffv"].as<String>().toInt();
  if (ov == 1) {
    if (!(d["n_vc"].as<String>() == "1") || g_c2e70 != 0) myDFPlayer.playFolder(0xf, 10);
  } else if (ov == 2) {
    if (!(d["n_vc"].as<String>() == "1") || g_c2e70 != 0) g_c2f21 = 99;
  }
  g_c335b = 0;
}
// @400db4d8
void h_hist(AsyncWebServerRequest *r) {
  String s; hist_json(s);
  r->send(200, String("application/json"), s);
}
// @400d51ac
void h_mass(AsyncWebServerRequest *r) {
  uint8_t s = r->arg("s").toInt();
  String v; mass_json(v, s);
  r->send(200, String("application/json"), v);
}
// @400d510c
void mass_json(String &out, int s) {
  if (s == 3) {
    if (g_c34c8 == 0 || millis() - g_c34c8 > 59999 || g_c34c5 != 0) g_c34c4 = 1;
    out = g_c34cc;
  } else out = "";
}
// @400d3b70
void h_lang_list(AsyncWebServerRequest *r) { r->send(200, String("application/json"), g_c2dc8); }
// @400d32c4
void h_lang_led_status(AsyncWebServerRequest *r) {
  String s = String("{\"lang\":") + (int)g_bf040 + ",\"rev\":\"" + String(g_c2e4c, 0x10) + "\"}";
  r->send(200, String("application/json"), s);
}
// @400d3e64
void h_lang_led_done(AsyncWebServerRequest *r) {
  if (g_c2d69 == 0) r->send(200, String("text/plain"), String("ok"));
  else r->send(0x1fb, String("text/plain"), String("low-memory"));
}
// @400d2ce8: тіло POST /lang-led — у буфер пам'яті (до 32 КБ)
void h_lang_led_file(AsyncWebServerRequest *r, const String &fn, size_t index, uint8_t *data, size_t len, bool final) {
  if (index == 0) {
    uint32_t id = 0xffffffff;
    if (r->hasParam("id")) { g_bf014 = pval(r, "id").toInt(); id = g_bf014; }
    g_bf014 = id;
    if (g_c2d74 != nullptr) free(g_c2d74);
    g_c2d74 = nullptr;
    g_c2d69 = 0; g_c2d70 = 0; g_c2d6c = 0;
  }
  if (g_c2d69 == 0) {
    uint32_t need = len + g_c2d70;
    if (need < 0x8001) {
      if (len != 0) {
        if (g_c2d6c < need) {
          void *p = realloc(g_c2d74, need + 0x200);
          if (p == nullptr) g_c2d69 = 1;
          else { g_c2d6c = need + 0x200; g_c2d74 = p; }
        }
        if (g_c2d69 == 0) { memcpy((uint8_t *)g_c2d74 + g_c2d70, data, len); g_c2d70 += len; }
      }
    } else g_c2d69 = 1;
  }
  if (final) g_c2d68 = 1;
}
// @400d58d8: код мови — 1…8 символів [a-z0-9_-]
bool lang_code_ok(const String &c) {
  if ((uint32_t)(c.length() - 1) < 8) {
    for (unsigned int i = 0; i < c.length(); i++) {
      int ch = c[i];
      if (0x19 < ((ch - 0x61) & 0xff) && 9 < ((ch - 0x30) & 0xff) && ch != 0x5f && ch != 0x2d) return false;
    }
    return true;
  }
  return false;
}
// @400d2be8: видача пакета мови шматками з буфера пам'яті
size_t lang_gz_filler(uint8_t *buf, size_t maxLen, size_t index) {
  if (g_c2d44 == nullptr || g_c2d40 <= index) { g_c2d30 = 1; return 0; }
  uint32_t left = g_c2d40 - index;
  size_t n = (left < maxLen) ? left : maxLen;
  memcpy(buf, (uint8_t *)g_c2d44 + index, n);
  if (g_c2d40 <= index + n) g_c2d30 = 1;
  return n;
}
// @400d5924
void h_lang(AsyncWebServerRequest *r) {
  String code = r->hasParam("code") ? pval(r, "code") : String("");
  if (!lang_code_ok(code)) { r->send(400, String("text/plain"), String("bad code")); return; }
  if (g_c2d34.equals(code) && g_c2d44 != nullptr && g_c2d31 == 0) {
    g_c2d31 = 1;
    AsyncWebServerResponse *resp = r->beginResponse(String("application/json"), g_c2d40, lang_gz_filler);
    resp->addHeader("Content-Encoding", "gzip");
    r->send(resp);
    return;
  }
  g_c2d4c = code;
  g_c2d48 = 1;
  r->send(0xca, String("text/plain"), String("__LANG_PREPARING__"));
}
// @400d5b9c
void h_lang_delete(AsyncWebServerRequest *r) {
  String code = r->hasParam("code") ? pval(r, "code") : String("");
  if (!lang_code_ok(code)) { r->send(400, String("text/plain"), String("bad code")); return; }
  g_c2d5c = code;
  uint32_t id = 0xffffffff;
  if (r->hasParam("id")) { g_bf010 = pval(r, "id").toInt(); id = g_bf010; }
  g_bf010 = id;
  g_c2d58 = 1;
  r->send(200, String("text/plain"), String("ok"));
}
// @400d3f18
void h_lang_chunk_done(AsyncWebServerRequest *r) {
  if (g_c2d7b == 0) {
    g_c2d79 = 1;
    g_c2d7a = 1;
    r->send(200, String("text/plain"), String("ok"));
  } else {
    g_c2d7b = 0;
    r->send(0x199, String("text/plain"), String("retry"));
  }
}
// @400d6188 → @400d5e28: тіло POST /lang-chunk
void h_lang_chunk_body(AsyncWebServerRequest *r, uint8_t *data, size_t len, size_t index, size_t total) {
  if (index == 0) {
    g_c2d7b = g_c2d79;
    if (g_c2d79 != 0) return;
    g_c2d94 = r->hasParam("code") ? pval(r, "code") : String("");
    uint32_t v = 0xffffffff;
    if (r->hasParam("id")) { g_bf01c = pval(r, "id").toInt(); v = g_bf01c; }
    g_bf01c = v;
    g_c2d88 = r->hasParam("rev") ? pval(r, "rev") : String("");
    v = 0xffffffff;
    if (r->hasParam("seq")) { g_bf018 = pval(r, "seq").toInt(); v = g_bf018; }
    g_bf018 = v;
    uint8_t fin = 0;
    if (r->hasParam("final")) { g_c2d84 = pval(r, "final") == "1"; fin = g_c2d84; }
    g_c2d84 = fin;
    if (g_c2d80 != nullptr) free(g_c2d80);
    size_t sz = total != 0 ? total : 1;
    g_c2d80 = malloc(sz);
    g_c2d7c = 0;
    if (g_c2d80 == nullptr) g_c2d7b = 1;
  }
  if (g_c2d7b == 0 && len != 0 && g_c2d80 != nullptr) {
    memcpy((uint8_t *)g_c2d80 + g_c2d7c, data, len);
    g_c2d7c += len;
  }
}
// @400d3494
void h_lang_upload_done(AsyncWebServerRequest *r) {
  if (g_c2da0 == 0) r->send(200, String("text/plain"), String("ok"));
  else r->send(0x1fb, String("text/plain"), String("low-memory"));
}
// @400d2e2c
void h_lang_upload_file(AsyncWebServerRequest *r, const String &fn, size_t index, uint8_t *data, size_t len, bool final) {
  if (index == 0) {
    g_c2dbc = r->hasParam("code") ? pval(r, "code") : String("");
    uint32_t v = 0xffffffff;
    if (r->hasParam("id")) { g_bf020 = pval(r, "id").toInt(); v = g_bf020; }
    g_bf020 = v;
    g_c2db0 = r->hasParam("rev") ? pval(r, "rev") : String("");
    if (g_c2dac != nullptr) free(g_c2dac);
    g_c2dac = nullptr;
    g_c2da0 = 0; g_c2da8 = 0; g_c2da4 = 0;
  }
  if (g_c2da0 == 0) {
    uint32_t need = len + g_c2da8;
    if (need < 0x8001) {
      if (len != 0) {
        if (g_c2da4 < need) {
          void *p = realloc(g_c2dac, need + 0x400);
          if (p == nullptr) g_c2da0 = 1;
          else { g_c2da4 = need + 0x400; g_c2dac = p; }
        }
        if (g_c2da0 == 0) { memcpy((uint8_t *)g_c2dac + g_c2da8, data, len); g_c2da8 += len; }
      }
    } else g_c2da0 = 1;
  }
  if (final) g_c2d78 = 1;
}
// @400d456c / @400d4634 / @400d46fc: статичні файли з прошивки (gzip)
void h_favicon(AsyncWebServerRequest *r) {
  AsyncWebServerResponse *resp = r->beginResponse_P(200, String("image/x-icon"), FAVICON_GZ, sizeof(FAVICON_GZ));
  resp->addHeader("Content-Encoding", "gzip");
  r->send(resp);
}
void h_clock_svg(AsyncWebServerRequest *r) {
  AsyncWebServerResponse *resp = r->beginResponse_P(200, String("image/svg+xml"), CLOCK_SVG_GZ, sizeof(CLOCK_SVG_GZ));
  resp->addHeader("Content-Encoding", "gzip");
  r->send(resp);
}
void h_logo_svg(AsyncWebServerRequest *r) {
  AsyncWebServerResponse *resp = r->beginResponse_P(200, String("image/svg+xml"), LOGO_SVG_GZ, sizeof(LOGO_SVG_GZ));
  resp->addHeader("Content-Encoding", "gzip");
  r->send(resp);
}
// @400d33dc: OPTIONS → 200, інше → 404
void h_not_found(AsyncWebServerRequest *r) {
  if (r->method() == HTTP_OPTIONS) r->send(200, String(""), String(""));
  else r->send(0x194, String(""), String(""));
}
// @400fc13c: «/*» — сторінка інтерфейсу; на точці доступа з чужим Host — перенаправлення (captive-портал)
void captive_request(AsyncWebServerRequest *r) {
  if (g_c2c34 == 0) {
    g_c2c34 = 1;
    evlog_write(String("[CAPTIVE] req host=") + r->host() + " lip=" + r->client()->localIP().toString());
  }
  if (r->client()->localIP().toString().equals("192.168.4.1") && r->host() != "192.168.4.1") {
    r->send(200, String("text/html"), String("<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=\"0; url=http://192.168.4.1/\"></head><body><a href=\"http://192.168.4.1/\">Open clock settings</a></body></html>"));
    return;
  }
  if (g_c33a4 != "" && g_c3398 != "") {
    if (!r->authenticate(g_c33a4.c_str(), g_c3398.c_str(), nullptr, false)) { r->requestAuthentication(nullptr, true); return; }
  }
  g_c34c0 = millis();
  // Виправлено: мітка була «"h2.7.1"» — за версією, а не за вмістом: після оновлення сторінки з тією ж версією браузер
  // отримував 304 і показував старий інтерфейс із кешу. Тепер — за вмістом (web/webui_patch.py).
  String etag = String(INDEX_HTML_ETAG);
  if (r->hasHeader("If-None-Match") && r->getHeader("If-None-Match")->value().equals(etag)) {
    r->send(0x130, String(""), String(""));
    return;
  }
  AsyncWebServerResponse *resp = r->beginResponse_P(200, String("text/html"), INDEX_HTML_GZ, sizeof(INDEX_HTML_GZ));
  resp->addHeader("Content-Encoding", "gzip");
  resp->addHeader("Cache-Control", "no-cache");
  resp->addHeader("ETag", etag);
  r->send(resp);
}
// @400f51fc: POST /post — записати поля в конфігурацію
void h_post(AsyncWebServerRequest *r) {
  int n = r->params();
  if (n == 0) { r->send(400, String("text/plain"), String("No parameters")); return; }
  JsonDocument d;
  if (!config_doc_load(d)) { r->send(500, String("text/plain"), String("Config error")); return; }
  for (int i = 0; i < n; i++) { AsyncWebParameter *p = r->getParam(i); d[p->name()] = p->value(); }
  config_doc_store(d);
  r->send(200, String("text/plain"), String(""));
  g_c2d28 = 1;
  g_c335b = 0;
}
// @400f4fac → @400f4b3c
void h_post2(AsyncWebServerRequest *r) { h_post_apply(r); }
void h_post_apply(AsyncWebServerRequest *r) {
  int n = r->params();
  if (n == 0) { r->send(400, String("text/plain"), String("No parameters")); return; }
  JsonDocument d;
  if (!config_doc_load(d)) { r->send(500, String("text/plain"), String("Config error")); return; }
  for (int i = 0; i < n; i++) { AsyncWebParameter *p = r->getParam(i); d[p->name()] = p->value(); }
  config_doc_store(d);
  r->send(200, String("text/plain"), String("Saved!"));
  g_c2d28 = 1;
  int ov = d["onoffv"].as<String>().toInt();
  if (ov == 1) {
    int vol;
    if (!(d["n_vc"].as<String>() == "1")) vol = d["DFvol"].as<String>().toInt();
    else {
      if (g_c2e70 == 0) goto done;
      vol = d["DFvol"].as<String>().toInt();
    }
    if ((uint32_t)vol == (uint32_t)g_c2f1a) myDFPlayer.playFolder(0xf, 10);
  } else if (ov == 2) {
    if (!(d["n_vc"].as<String>() == "1") || g_c2e70 != 0) g_c2f21 = 99;
  }
done:
  g_c335b = 0;
}
