// Wi-Fi: запуск (STA/AP), збережені мережі /networks.json, перевірка інтернету, перепідключення, події.
// EN: Wi-Fi: STA/AP start, saved networks, internet check, reconnection (from 2.7.1; network fixes marked "Виправлено").
// Перенесено з 2.7.1 (адреси оригіналу в коментарях).
#include "fw.h"

// @400d6c00
void wifi_attempt_inc() { if (g_c3388 < 10) g_c3388 = g_c3388 + 1; }
// @400d6c18
void wifi_persist_cfg() { WiFi.persistent(false); WiFi.setAutoReconnect(true); }
// @400d55bc: причина відключення текстом
void wifi_reason_text(String &out, uint16_t r) {
  const char *p;
  if (r == 0xf) p = "wrong password (handshake timeout)";
  else if (r < 0x10) {
    if (r == 4) p = "AP inactive (assoc timeout)";
    else if (r == 8) p = "AP kicked client";
    else if (r == 2) p = "auth expired";
    else p = "unknown";
  } else if (r == 0xc9) p = "network not found";
  else if (r < 0xca) p = (r == 200) ? "beacon lost (weak signal)" : "unknown";
  else if (r == 0xca) p = "auth failed";
  else if (r == 0xcb) p = "association failed";
  else p = "unknown";
  out = p;
}
// @400d6c30: стан підключення для веб-інтерфейсу за причиною відмови
void wifi_fail_status(int r) {
  if (g_c3374 != String("connecting")) return;
  const char *p = "fail_notfound";
  if (r != 0xc9) {
    if (r == 2 || r == 0xf) p = "fail_pass";
    else { p = "fail"; if (((r - 0xca) & 0xfffffffd) == 0) p = "fail_pass"; }
  }
  g_c3374 = p;
}
// @400fbac0: обробник подій Wi-Fi (зареєстрований на всі події)
void wifi_event(system_event_id_t ev, system_event_info_t info) {
  if (ev == SYSTEM_EVENT_STA_DISCONNECTED) {
    dbg_ws_print(String("Disconnected from station "));
    uint8_t r = info.disconnected.reason;
    g_c2c14 = (uint16_t)r;
    wifi_fail_status(r);
    String rt;
    if (g_c2d29 != 0) {
      wifi_reason_text(rt, r);
      evlog_write(String("[WIFI] Disconnected  reason=") + String(r, 10) + String(" (") + rt + String(")"));
    }
    wifi_reason_text(rt, r);
    dbg_ws_print(String("[WIFI] Disconnected  reason=") + String(r, 10) + String(" (") + rt + String(")"));
    g_c2d29 = 0;
    g_c3319 = 1;
    if (g_c33f8 == 2) {
      // Виправлено: GOT_IP ставить лічильник спроб g_c3388 = 10 (значок «з'єднано» в анімації), і в автора
      // перша ж спроба після обриву йшла гілкою «цикл вичерпано» — WIFI_OFF і точка доступу за 8 с після
      // обриву (журнал годинника 08.10: reason=200 о 09:36:23, «AP restored» о 09:36:31). Новий обрив —
      // лічба з нуля, перша власна спроба — за 5 с (до того перепідключається саме ядро).
      if (g_c3381 == 0) { g_c3388 = 0; g_c15d8 = millis(); }
      g_c3381 = 1;
    }
  } else if (ev == SYSTEM_EVENT_STA_GOT_IP) {
    String p; led_phrase(p, g_c34b0, 0xad);
    dbg_ws_print(p + WiFi.localIP().toString());
    if (g_c2d29 == 0) evlog_write(String("[WIFI] Connected  IP=") + WiFi.localIP().toString());
    dbg_ws_print(String("[WIFI] Connected  IP=") + WiFi.localIP().toString());
    g_c2d29 = 1;
    g_c3374 = F("connected");
    g_c33d4 = g_c33ec;
    g_c33c8 = g_c33e0;
    g_c3318 = 1;
    g_c3388 = 10;
    g_c2c14 = 0;
  } else if (ev == SYSTEM_EVENT_STA_START) {
    dbg_ws_print(String("Station mode started"));
  }
}
// @400d6d10
void wifi_events_register() {
  if (g_c1958 == 0) {
    WiFi.onEvent(wifi_event, SYSTEM_EVENT_MAX);
    g_c1958 = 1;
  }
}
// @400d6bc8: підключитися до мережі
void wifi_begin(const char *ssid, const char *pass) {
  if (WiFi.status() == WL_CONNECTED) { WiFi.disconnect(false); delay(1000); }
  if (WiFi.status() != WL_CONNECTED) { WiFi.begin(ssid, pass, 0, nullptr, true); delay(500); }
}
// @400ff260
uint32_t wifi_try(int8_t pick) {
  if (pick != 0) return networks_pick();
  int n = g_c33ec.length();
  if (n != 0) wifi_begin(g_c33ec.c_str(), g_c33e0.c_str());
  return n != 0;
}
// @400d6cc8: примусова повторна асоціація
void wifi_force_reassoc() {
  g_c2c10 = g_c2c10 + 1;
  WiFi.disconnect(false);
  delay(0x32);
  wifi_mode_t m;
  if (WiFi.softAPgetStationNum() == 0) { WiFi.mode(WIFI_OFF); delay(0x96); m = WIFI_STA; }
  else m = WIFI_AP_STA;
  WiFi.mode(m);
  wifi_persist_cfg();
  delay(0x96);
}
// @400fb9a4: DNS для captive-порталу
void captive_dns_start() {
  dnsServer.stop();
  bool ok = dnsServer.start(0x35, String("*"), WiFi.softAPIP());
  g_fx_dns_on = ok;
  evlog_write(String("[CAPTIVE] dns start ") + (ok ? "ok" : "FAIL") + " apip=" + WiFi.softAPIP().toString());
}

// ---------- /networks.json ----------
// @400fdf84
void networks_save(const String &s) {
  File f = SPIFFS.open(String("/networks.json"), "w");
  if (!f) dbg_ws_print(String("Error opening file for writing."));
  else {
    f.print(s);
    dbg_ws_print(String("Saved Networks Config File: ") + s);
    dbg_ws_sep(1);
  }
  f.close();
}
// @400fe0cc
void networks_load(JsonDocument &doc) {
  File f = SPIFFS.open(String("/networks.json"), "r");
  String s = f.readString();
  String e("[]");
  if (s != "") {
    dbg_ws_print(String("Config networks loaded: ") + s);
    dbg_ws_sep(1);
    deserializeJson(doc, s);
  } else {
    dbg_ws_print(String("Failed to open saved networks file, creating initial empty file"));
    networks_save(e);
    deserializeJson(doc, e);
  }
  f.close();
}
// @400e4c14
void dbg_ws_sep(int8_t k) {
  if (k == 1) dbg_ws_print(String("======================================================="));
  else if (k == 2) dbg_ws_print(String("===================="));
}
// @400fe264: мережі з автопідключенням (з індексу 1; 0 — основна з конфігурації), до 9
void networks_auto_list(JsonDocument &doc, String *ss, String *pp) {
  networks_load(doc);
  if (!doc.is<JsonArray>()) return;
  int i = 1;
  for (JsonObject o : doc.as<JsonArray>()) {
    if (9 < i) return;
    if (o["auto"] | true) {
      const char *n = o["network"];
      if (n != nullptr) {
        const char *p = o["password"];
        ss[i] = n;
        pp[i] = p != nullptr ? p : "";
        i++;
      }
    }
  }
}
// @400fe324: пароль збереженої мережі
void networks_password(String &out, const String &ssid) {
  JsonDocument doc;
  networks_load(doc);
  for (JsonObject o : doc.as<JsonArray>()) {
    const char *n = o["network"];
    if (n != nullptr && ssid == n) {
      const char *p = o["password"];
      out = p != nullptr ? p : "";
      return;
    }
  }
  out = "";
}
// @400fe3f0: якщо основна мережа не задана — взяти першу збережену
void networks_seed() {
  if (g_c33ec.length() != 0 && g_c33ec != String("Your WI-FI network")) return;
  JsonDocument doc;
  networks_load(doc);
  for (JsonObject o : doc.as<JsonArray>()) {
    if (o["auto"] | true) {
      const char *n = o["network"];
      if (n != nullptr && *n != 0) {
        const char *p = o["password"];
        g_c33ec = n;
        g_c33e0 = p != nullptr ? p : "";
        config_set("ssid", g_c33ec);
        config_set("pass", g_c33e0);
        g_c2d28 = 1;
        dbg_ws_print(String("Seeded primary SSID from saved list: ") + g_c33ec);
        break;
      }
    }
  }
}
// @400fe648: список для веб-інтерфейсу [{"network","auto"}] → g_c2c28
void networks_load2() {
  File f = SPIFFS.open(String("/networks.json"), "r");
  String s = f ? f.readString() : String("");
  if (f) f.close();
  JsonDocument in;
  if (s.length() != 0) deserializeJson(in, s);
  JsonDocument out;
  JsonArray a = out.to<JsonArray>();
  for (JsonObject o : in.as<JsonArray>()) {
    const char *n = o["network"];
    if (n != nullptr && *n != 0) {
      JsonObject e = a.add<JsonObject>();
      e["network"] = n;
      e["auto"] = o["auto"] | true;
    }
  }
  g_c2c28 = "";
  serializeJson(out, g_c2c28);
  g_c2c24 = 1;
}
// @400fe924: додати мережу або оновити пароль
void networks_update(JsonDocument &doc, const char *ssid, const char *pass) {
  JsonArray a = doc.as<JsonArray>();
  int n = a.size();
  for (JsonObject o : a) {
    const char *nn = o["network"];
    if (nn != nullptr && strcmp(nn, ssid) == 0) {
      const char *pp = o["password"];
      if (pp != nullptr && strcmp(pp, pass) == 0) return;
      o["password"] = pass;
      String s(""); serializeJson(doc, s);
      networks_save(s);
      networks_load2();
      dbg_ws_print(String("Updated stored password for: ") + String(ssid));
      return;
    }
  }
  dbg_ws_print(String("New WiFi network added to list: ") + String(ssid));
  if (9 < n && !a.isNull()) a.remove(0);
  JsonObject e = a.add<JsonObject>();
  e["network"] = ssid;
  e["password"] = pass;
  e["auto"] = true;
  String s(""); serializeJson(doc, s);
  networks_save(s);
  networks_load2();
}
// @400febe4: вибрати мережу за скануванням (основна або збережена)
uint32_t networks_pick() {
  if (WiFi.scanComplete() == -1) return 0;
  int n = WiFi.scanNetworks(false, true, false, 300);
  JsonDocument doc;
  String ss[10], pp[10];
  ss[0] = g_c33ec;
  pp[0] = g_c33e0;
  networks_auto_list(doc, ss, pp);
  uint32_t r = 0;
  if (0 < n) {
    for (uint32_t k = 0; ss[k].length() != 0; ) {
      String s = ss[k];
      dbg_ws_print(String("Saved ssid: *") + s + String("*"));
      for (int8_t i = 0; i < n; i++) {
        wifi_connect_anim();
        P.displayZoneText(0, g_c335c.c_str(), PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
        P.displayZoneText(1, g_c3368.c_str(), PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
        P.displayAnimate();
        String sc = WiFi.SSID(i);
        dbg_ws_print(String("....Scanned ssid: *") + sc + String("*"));
        if (sc.equals(s)) {
          if ((k & 0xff) == 0) {
            dbg_ws_print(String("Found selected network: ") + sc);
            networks_update(doc, ss[0].c_str(), pp[0].c_str());
          } else {
            dbg_ws_print(String("Can't connect to the selected network, but found one from the saved SSID list: ") + sc);
          }
          wifi_begin(s.c_str(), pp[k].c_str());
          r = 1;
          goto done;
        }
      }
      k++;
      if (k == 10) break;
    }
  }
done:
  WiFi.scanDelete();
  return r;
}
// @400ff2a0: забути мережу
void networks_forget(const String &ssid) {
  JsonDocument doc;
  networks_load(doc);
  JsonArray a = doc.as<JsonArray>();
  bool ch = false;
  for (int i = (int)a.size() - 1; -1 < i; i--) {
    const char *n = a[i]["network"];
    if (n != nullptr && ssid == n) { a.remove(i); ch = true; }
  }
  if (ch) { String s(""); serializeJson(doc, s); networks_save(s); networks_load2(); }
}
// @400ff3c8: автопідключення для мережі
void networks_set_auto(const String &ssid, uint8_t on) {
  JsonDocument doc;
  networks_load(doc);
  bool ch = false;
  for (JsonObject o : doc.as<JsonArray>()) {
    const char *n = o["network"];
    if (n != nullptr && ssid == n) { o["auto"] = (bool)on; ch = true; }
  }
  if (ch) { String s(""); serializeJson(doc, s); networks_save(s); networks_load2(); }
}
// @400d4c80: результат асинхронного сканування → g_c34cc
void wifi_scan_harvest_json() {
  int n = WiFi.scanComplete();
  if (n < 0) return;
  String s("[");
  s.reserve(n * 0x50 + 0x40);
  for (int i = 0; i < n; i++) {
    uint8_t k = i;
    int enc = WiFi.encryptionType(k);
    s += String("{ \"name\":\"") + WiFi.SSID(k) + String("\", ");
    s += String("\"channel\":\"") + String(WiFi.channel(k)) + String("\", ");
    s += String("\"signalStrength\":\"") + String(WiFi.RSSI(k)) + String("\", ");
    s += String("\"open\":") + (enc == 0 ? "true" : "false") + String(" }");
    if (i < n - 1) s += String(", ");
  }
  s.concat("]");
  g_c34cc = s;
  g_c34c8 = millis();
  g_c34c5 = 0;
  WiFi.scanDelete();
}
// @400d6d7c (FUN_400d6d7c)
// @400d6d7c: кадри анімації підключення
void wifi_connect_anim()
{
  String t_30, t_3c;  // тимчасові
  if (g_c34ba == '\b') {
    if (0x36 < g_c2c16) {
      g_c2c16 = 0;
    }
    switch(g_c2c16) {
    case 0:
      g_c3368 = "\x01" "B\x16";
      t_3c = "\x02" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 1:
      g_c3368 = "\x03" "B\x17";
      t_3c = "\x04" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 2:
      g_c3368 = "\x05" "B\x18";
      t_3c = "\x06" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 3:
      g_c3368 = "\x07" "B\x19";
      t_3c = "\x08" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 4:
      g_c3368 = "\x09" "B\x1a";
      t_3c = "\nB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 5:
      g_c3368 = "\x0b" "B\x1b";
      t_3c = "\x0c" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 6:
      g_c3368 = "\x0d" "B\x1a";
      t_3c = "\x0e" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 7:
      g_c3368 = "\x0f" "B\x1b";
      t_3c = "\x10" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 8:
      g_c3368 = "\x11" "B\x16";
      t_3c = "\x12" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 9:
      g_c3368 = "IB\x17";
      t_3c = "JB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 10:
      g_c3368 = "KB\x18";
      t_3c = "LB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0xb:
      g_c3368 = "MB\x19";
      t_3c = "NB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0xc:
      g_c3368 = "OB\x1a";
      t_3c = "PB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0xd:
      g_c3368 = "QB\x1b";
      t_3c = "RB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0xe:
      g_c3368 = "SB\x1a";
      t_3c = "TB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0xf:
      g_c3368 = "UB\x1b";
      t_3c = "VB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x10:
      g_c3368 = "WB\x16";
      t_3c = "XB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x11:
      g_c3368 = "YB\x17";
      t_3c = "ZB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x12:
      g_c3368 = "[B\x18";
      t_3c = "\\B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x13:
      g_c3368 = "]B\x19";
      t_3c = "^B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x14:
      g_c3368 = "_B\x1a";
      t_3c = "`B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x15:
      g_c3368 = "aB\x1b";
      t_3c = "bB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x16:
      g_c3368 = "cB\x1a";
      t_3c = "dB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x17:
      g_c3368 = "eB\x1b";
      t_3c = "fB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x18:
      g_c3368 = "gB\x16";
      t_3c = "hB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x19:
      g_c3368 = "iB\x17";
      t_3c = "jB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x1a:
      g_c3368 = "kB\x18";
      t_3c = "lB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x1b:
      g_c3368 = "mB\x19";
      t_3c = "nB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x1c:
      g_c3368 = "oB\x1a";
      t_3c = "pB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x1d:
      g_c3368 = "qB\x1b";
      t_3c = "rB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x1e:
      g_c3368 = "sB\x1a";
      t_3c = "tB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x1f:
      g_c3368 = "uB\x1b";
      t_3c = "vB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x20:
      g_c3368 = "wB\x17";
      t_3c = "xB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x21:
      g_c3368 = "yB\x18";
      t_3c = "zB";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x22:
      g_c3368 = "{B\x19";
      t_3c = "|B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x23:
      g_c3368 = "}B\x1a";
      t_3c = "~B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x24:
      g_c3368 = "\x7f" "B\x1b";
      t_3c = "\x80" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x25:
      g_c3368 = "\x81" "B\x1a";
      t_3c = "\x82" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x26:
      g_c3368 = "\x83" "B\x1b";
      t_3c = "\x84" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x27:
      g_c3368 = "\x85" "B\x16";
      t_3c = "\x86" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x28:
      g_c3368 = "\x87" "B\x17";
      t_3c = "\x88" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x29:
      g_c3368 = "\x89" "B\x18";
      t_3c = "\x8a" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x2a:
      g_c3368 = "\x8b" "B\x19";
      t_3c = "\x8c" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x2b:
      g_c3368 = "\x8d" "B\x1a";
      t_3c = "\x8e" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x2c:
      g_c3368 = "\x8f" "B\x1b";
      t_3c = "\x90" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x2d:
      g_c3368 = "\x91" "B\x1a";
      t_3c = "\x92" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x2e:
      g_c3368 = "\x93" "B\x1b";
      t_3c = "\x94" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x2f:
      g_c3368 = "\x95" "B\x16";
      t_3c = "\x96" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x30:
      g_c3368 = "\x97" "B\x17";
      t_3c = "\x98" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x31:
      g_c3368 = "\x99" "B\x18";
      t_3c = "\x9a" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x32:
      g_c3368 = "\x9b" "B\x19";
      t_3c = "\x9c" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x33:
      g_c3368 = "\x9d" "B\x1a";
      t_3c = "\x9e" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x34:
      g_c3368 = "\x9f" "B\x1b";
      t_3c = "\xa0" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x35:
      g_c3368 = "\xa1" "B\x1a";
      t_3c = "\xa2" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 0x36:
      g_c3368 = "\xa3" "B\x1b";
      t_3c = "\xa4" "B";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    default:
      goto LAB_400d7bd5;
    }
  }
  else {
    if (4 < g_c2c16) {
      g_c2c16 = 0;
    }
    switch(g_c2c16) {
    case 0:
      t_3c = "\x14" "\x1c";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 1:
      t_3c = "\x15" "F";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 2:
      t_3c = "CG";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 3:
      t_3c = "DH";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    case 4:
      t_3c = "E\x1c";
      t_30 = t_3c;
      t_30 += g_c2c18;
      g_c335c = t_30;
      break;
    default:
      goto LAB_400d7bd5;
    }
  }
LAB_400d7bd5:
  delay(0x1e);
  g_c2c16 = g_c2c16 + 1;
  return;
}

// @4011b374: запуск Wi-Fi (AP — якщо «офлайн»; STA — підключення до 40 с, інакше AP)
void wifi_start() {
  g_c3388 = 0;
  g_c3384 = millis();
  g_c3381 = 0;
  g_c2c18 = "+";
  led_off();
  if (g_c33f8 == 1) {
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(g_c33bc.c_str(), g_c33b0.c_str(), 1, 0, 4);
    captive_dns_start();
    dbg_ws_print(String("Starting Wi-Fi AP mode — Offline mode selected in UI"));
    return;
  }
  if (g_c33f8 != 2) return;
  networks_seed();
  wifi_events_register();
  wifi_persist_cfg();
  if (WiFi.softAPgetStationNum() == 0 && WiFi.getMode() != WIFI_AP_STA) WiFi.mode(WIFI_STA);
  uint32_t t0 = millis();
  dbg_ws_print(String("Connecting STA"));
  P.setCharSpacing(0);
  P.setFont(0, (MD_MAX72XX::fontType_t *)font_icons_status);
  P.setFont(1, (MD_MAX72XX::fontType_t *)font_icons_status);
  if (g_c34ba == 4) {
    wifi_connect_anim();
    P.displayZoneText(0, g_c335c.c_str(), PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayAnimate();
    delay(500);
  }
  uint32_t ok = wifi_try(0);
  wifi_attempt_inc();
  while (WiFi.status() != WL_CONNECTED) {
    loop_guard_feed();
    P.displayAnimate();
    if (millis() - g_c3384 > 39999 || g_c33ec == "" || 9 < g_c3388) {
      WiFi.mode(WIFI_AP);
      WiFi.softAP(g_c33bc.c_str(), g_c33b0.c_str(), 1, 0, 4);
      captive_dns_start();
      g_c34af = 0;
      g_c34a9 = 0;
      evlog_write(String("[WIFI] AP mode — client connect timed out  SSID=") + g_c33bc);
      dbg_ws_print(String("Starting Wi-Fi AP mode (2nd event)"));
      display_clear_all();
      int n = 0x14;
      if (g_c34ba == 8) {
        do {
          const char *p;
          if (g_c2c17 == 0) { P.displayZoneText(1, "5BB;", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT); p = "6BB<"; }
          else { P.displayZoneText(1, "5", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT); p = "6"; }
          P.displayZoneText(0, p, PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
          g_c2c17 = g_c2c17 ^ 1;
          P.displayAnimate();
          n--;
          delay(0x96);
        } while (n != 0);
      } else {
        do {
          P.displayZoneText(0, g_c2c17 == 0 ? "9:BBB?" : "9:", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
          g_c2c17 = g_c2c17 ^ 1;
          P.displayAnimate();
          n--;
          delay(0x96);
        } while (n != 0);
      }
      g_c2d29 = 0;
      g_c3319 = 1;
      delay(2000);
      g_c3471 = 0;
      g_c3470 = 0;
      P.displayZoneText(1, "@@@@", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
      P.displayZoneText(0, "        ", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
      P.setCharSpacing(1);
      display_clear_all();
      g_c34a4 = random(0x2d, 0x3a);
      g_c34a3 = random(0x2d, 0x3a);
      sys_font_apply();
      P.displayAnimate();
      g_c34af = 0;
      display_zones_init();
      return;
    }
    uint32_t per = ok != 0 ? 12000 : 4000;
    if (per <= millis() - t0) {
      t0 = millis();
      wifi_attempt_inc();
      dbg_ws_print(String("Wifi        : startup reconnect attempt=") + String(g_c3388, 10));
      uint8_t pick;
      if (ok == 0 || (uint32_t)g_c3388 % 3 == 0) { wifi_force_reassoc(); pick = 1; }
      else pick = 0;
      ok = wifi_try(pick);
    }
    static const char *const ICON[11] = { "+", ",", "-", ".", "/", "0", "1", "2", "3", "4", "@" };
    if (g_c3388 <= 10) g_c2c18 = ICON[g_c3388];
    wifi_connect_anim();
    P.displayZoneText(0, g_c335c.c_str(), PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayZoneText(1, g_c3368.c_str(), PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayAnimate();
    button_check();
    g_c2e94 = millis();
  }
  if (g_c34ba == 8 && WiFi.status() == WL_CONNECTED) {
    P.displayZoneText(1, "\xa5" "B\x1e", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayZoneText(0, "\xa6", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayAnimate();
    g_c2e94 = millis();
    delay(0x9c4);
    g_c34bc->clear(0, 0xf);
  } else if (g_c34ba == 4 && WiFi.status() == WL_CONNECTED) {
    P.displayZoneText(0, "\x14" "@\x1e", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
    P.displayAnimate();
    g_c2e94 = millis();
    delay(0x9c4);
    g_c34bc->clear(0, 0xf);
  }
  P.setCharSpacing(1);
  g_c3471 = 0;
  g_c3470 = 0;
  g_c34a4 = 0x37;
  g_c34a3 = 0x37;
  g_c34af = 0;
  fx_status_begin(FXS_TIME, true);               // нове: «Синхронізація часу» замість порожнього табло
  sys_sync_ntp();
  rtc_detect_sync();
  fx_status_end();
  P.displayZoneText(1, "@@@@", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayZoneText(0, "        ", PA_LEFT, 0, 0, PA_PRINT, PA_NO_EFFECT);
  sys_font_apply();
  if (g_c34ba == 8) {
    uint8_t z, a = g_c34b8, b;
    if (g_c34b8 == 4) { P.setZone(0, 0, 3); b = 7; z = 1; }
    else {
      if (g_c34b8 != 8) goto out;
      if (g_c34b7 == 0) { P.setZone(0, 0, 7); P.setZone(1, 0xc, g_c34b8 * 2 - 1); P.setZone(3, 0, 0); b = 0xb; }
      else { P.setZone(0, 0, 7); P.setZone(1, 8, 0xb); P.setZone(3, 0, 0); b = 0xf; a = 0xc; }
      z = 2;
    }
    P.setZone(z, a, b);
  }
out:
  display_clear_all();
}
// @4011bb58
void wifi_start_wrapper() {
  config_migrate();
  button_pin_init();
  if (g_c33f8 == 2) { WiFi.mode(WIFI_STA); WiFi.disconnect(false); delay(100); }
  wifi_start();
  routes_register_all();
  server.begin();
}
// @400d619c
void button_pin_init() { pinMode(0, INPUT_PULLUP); }

// @401213b4: перевірка інтернету.
// Виправлено: в автора спершу HTTP до msftconnecttest (DNS + з'єднання до 5 с), а запасна TCP-проба до
// 8.8.8.8:53 — без тайм-ауту: без інтернету цикл стояв до ~3 хв, поки lwIP вичерпає повтори SYN (саме тут
// оригінал ловив сторожа: «loop was stalled in: wifi/internet check»). Тепер спершу TCP до 8.8.8.8:53 і
// 1.1.1.1:53 по 1,5 с (без DNS), потім HTTP з тайм-аутом з'єднання 3 с. «Офлайн» — лише після двох невдач
// поспіль: в автора лічильник g_c1748 не давав другої спроби, і перша ж невдала проба вимикала погоду,
// новини й NTP (скарги користувачів «нет интернета» при робочому Wi-Fi); повтор — через 15 с (net_check_periodic).
void net_probe_internet() {
  FxStatusScope fxs(FXS_NET);                    // нове: стан на табло під час запуску
  dbg_ws_print(String("[NET ] Probing internet..."));
  bool ok = net_tcp_probe(IPAddress(8, 8, 8, 8), 0x35, 1500) == 1 || net_tcp_probe(IPAddress(1, 1, 1, 1), 0x35, 1500) == 1;
  dbg_ws_print(String(ok ? "TCP probe (8.8.8.8 / 1.1.1.1:53): OK" : "TCP probe (8.8.8.8 / 1.1.1.1:53): FAIL"));
  if (!ok) {
    dbg_ws_print(String("Trying HTTP probe (msftconnecttest)..."));
    String url("http://www.msftconnecttest.com/connecttest.txt");
    WiFiClient c;
    HTTPClient h;
    h.setConnectTimeout(3000);
    h.setTimeout(2000);
    h.useHTTP10(true);
    if (h.begin(c, url)) {
      if (h.GET() == 200) {
        char b[0x18]; memset(b, 0, 0x18);
        h.getStream().readBytes(b, 0x17);
        ok = strncmp(b, "Microsoft Connect Test", 0x16) == 0;
      }
      h.end();
    }
    dbg_ws_print(String(ok ? "HTTP probe: OK" : "HTTP probe: FAIL"));
  }
  if (!ok) {
    g_bf04c = 0;
    if (g_c1748 != 0xff) g_c1748 = g_c1748 + 1;
    dbg_ws_print(String("Internet probe failed (count=") + String(g_c1748, 10) + String(")"));
    if (g_c3319 == 0 && g_c1748 < 2) dbg_ws_print(String("Internet probe: one failure — re-check before going offline"));
    else { dbg_ws_print(String("No internet - going OFFLINE")); g_c3319 = 1; }
    return;
  }
  g_c1748 = 0;
  g_bf04c = 1;
  g_c2d29 = 1;
  g_c3319 = 0;
  dbg_ws_print(String("[NET ] Probe OK"));
  dbg_ws_print(String("Internet is working"));
  if (g_c33fc == "") sys_sync_ntp();
}
// @40121848: періодичний стан зв'язку й інтернету
void net_check_periodic(uint32_t) {
  if (millis() - g_c15e4 > 4999) {
    g_c15e4 = millis();
    int8_t pend = g_c3381;
    if (WiFi.status() == WL_CONNECTED) g_c2d29 = 1;
    else {
      g_c2d29 = 0;
      g_c3319 = 1;
      if (g_c33f8 == 2 && g_c3381 == 0) { g_c3381 = 1; g_c3388 = pend; }
    }
  }
  int st = WiFi.status();
  uint32_t t = g_c15dc;
  uint32_t per;
  if (st == WL_CONNECTED) {
    if (g_c3319 == 0) {
      if (g_bf04c != 0) goto L8ed;
      g_c15e1 = g_bf04c;
      per = 15000;
    } else {
      per = 30000;
      if (1 < g_c15e1) { per = 60000; if (4 < g_c15e1) per = 120000; }
    }
  } else {
L8ed:
    g_c15e1 = 0;
    per = 600000;
  }
  g_c2e74 = per;
  if (st != WL_CONNECTED) {
    if (g_c15e0 == 0) { dbg_ws_print(String("Wifi        : link lost")); g_c15e0 = 1; }
    return;
  }
  g_c15e0 = 0;
  if (g_c3318 == 0 && millis() - t < per) return;
  g_c3318 = 0;
  g_c15dc = millis();
  dbg_ws_print(String("Wifi        : link OK, probing internet"));
  if (WiFi.softAPgetStationNum() == 0) WiFi.softAPdisconnect(true);
  int8_t was = g_c3319;
  net_probe_internet();
  if (g_c3319 == 0) {
    if (was != 0) { evlog_write(String("[NET ] Internet online")); dbg_ws_print(String("[NET ] ONLINE")); }
  } else if (was == 0) {
    evlog_write(String("[NET ] Internet offline — ping failed"));
    dbg_ws_print(String("[NET ] OFFLINE — ping failed"));
  }
  g_c3381 = 0;
  if (!g_fx_quiet) {                             // нове: уночі (табло погашене) — без перевірки версії й мовного пакета
    fw_version_check();
    if (g_c2d29 != 0 && g_c3319 == 0) led_lang_fetch(g_c34b0);
  }
  if (g_c3319 != 0) {
    if (g_c15e1 != 0xff) g_c15e1 = g_c15e1 + 1;
    dbg_ws_print(String("Wifi        : link up but no internet"));
    if (g_c33f8 == 2 && 5 < g_c15e1) {
      g_c15e1 = 0;
      // Виправлено: в автора — перепідключення Wi-Fi (з WIFI_OFF) кожні ~6 хв без інтернету, навіть коли
      // роутер у порядку й інтернету немає лише в провайдера. Це не допомагає, а рве веб-з'єднання; у журналі
      // годинника після такого — падіння по колу. Тепер — лише коли не відповідає й сам роутер.
      if (net_gateway_alive()) {
        dbg_ws_print(String("Wifi        : router answers — no internet at provider, keeping Wi-Fi link"));
      } else {
        evlog_write(String("[NET ] Forced re-association (no internet >6 min, router silent)"));
        dbg_ws_print(String("[NET ] Forced re-association"));
        dbg_ws_print(String("Wifi        : no internet too long — forcing re-association"));
        wifi_force_reassoc();
        g_c3381 = 1;
        g_c3388 = 0;
      }
    }
  }
}
// @40121b3c: цикл перепідключення
void wifi_reconnect_cycle(uint32_t per) {
  uint32_t t = g_c15d8;
  if (g_c3388 < 6) { if (2 < g_c3388) per = 15000; }
  else per = 30000;
  if (per <= millis() - t && (g_c15d8 = millis(), g_c3381 != 0)) {
    if (WiFi.status() == WL_CONNECTED) {
      g_c2d29 = 1;
      g_c2e73 = 0;
      net_probe_internet();
      if (g_c3319 == 0) sys_sync_ntp();
      g_c3381 = 0;
    } else if (g_c3388 < 10) {
      uint8_t a = g_c3388 + 1;
      if (g_c2e73 == 0) {
        dbg_ws_print(String("Wifi        : reconnecting  attempt=") + String((uint32_t)a, 10));
        g_c2e73 = 1;
      }
      bool f = (uint32_t)a % 3 == 0;
      if (f) wifi_force_reassoc();
      wifi_try(f);
      g_c2e73 = 0;
      g_c3388 = a;
    } else {
      g_c2e73 = 0;
      wifi_force_reassoc();
      wifi_try(1);
      g_c3388 = 0;
      if (g_c2d29 == 0 && WiFi.getMode() == WIFI_STA) {
        WiFi.mode(WIFI_AP_STA);
        WiFi.softAP(g_c33bc.c_str(), g_c33b0.c_str(), 1, 0, 4);
        captive_dns_start();
        evlog_write(String("[WIFI] AP restored — router reconnect cycle exhausted"));
        dbg_ws_print(String("Wifi        : AP restored after full reconnect cycle"));
      }
    }
  }
}
