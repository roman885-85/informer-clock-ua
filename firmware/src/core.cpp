// Ядро: setup/loop, журнал подій, конфігурація (файл ↔ рядок JSON), кнопка. Перенесено з 2.7.1 один в один.
// EN: Core: setup()/loop(), event log, config file <-> JSON string, button (from 2.7.1; loop restructured, see fix.h).
#include "fw.h"

// ---------- setup @40120794 ----------
void setup() {
#ifdef COPY_DIAG
  Serial.begin(115200);   // лише діагностична збірка (env:diag)
#define DIAG(x) Serial.println(x)
#else
#define DIAG(x)
#endif
  loop_guard_start();                            // нове: сторож головного циклу (guard.cpp)
  ws.onEvent(ws_on_event);                       // обробник подій WebSocket @400d3288
  server.addHandler(&ws);
  Wire.begin(21, 22, 0);
  g_c3468 = g_c346c.begin(&Wire);                // RTC DS3231 знайдено
  SPIFFS.begin(true, "/spiffs", 10);
  evlog_boot_reason();
  fx_crash_text(g_fx_crash_last);                // нове: причина аварії, записана обробником ядра (crash.cpp)
  if (g_fx_crash_last.length()) evlog_write(String("[BOOT] crash: ") + g_fx_crash_last);
  rtc_crash.magic = 0;
  if (rtc_guard == LOOP_GUARD_MAGIC)             // нове: перезапуск зробив сторож циклу
    evlog_write(String("[BOOT] loop guard: loop() stuck > ") + String(LOOP_GUARD_MS / 1000) + String(" s — restarted"));
  rtc_guard = 0;
  uint32_t st = 0xffffffff;
  if (rtc_magic == 0xc10c0fe5) st = rtc_stage;
  if (st - 1 < 0xfffffffe) {
    String m = String("[BOOT] loop was stalled in: ");
    m += (st < 7) ? STAGE_NAMES[st] : "n/a";
    evlog_write(m);
  }
  rtc_magic = 0xc10c0fe5; rtc_stage = 0;
  config_load_file();
  {
    JsonDocument doc;
    deserializeJson(doc, g_c2ef0);
    g_c34ba = doc["matrixa"].as<String>().toInt();
    g_c34b8 = doc["num_matr8"].as<String>().toInt();
    uint8_t z = 1;
    if (g_c34ba == 8) { z = 2; if (g_c34b8 == 8) z = 4; }
    P.begin(z);
    P.begin(g_c34ba != 8 ? 1 : 4);
    // нове: «Запуск» на табло, поки ініціалізуються плеєр і стрічка (status.cpp); мова й шрифт — з конфігурації
    g_c2eef = doc["lang"].as<String>().toInt();
    g_c349b = doc["fs"].as<String>().toInt();
    if (g_c34ba == 8) display_zones_init();
    fx_status_begin(FXS_BOOT, true);
    pinMode(2, OUTPUT);
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, PUT, DELETE, OPTIONS");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "x-test-header, Origin, X-Requested-With, Content-Type, Accept, X-Custom-Header");
    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Credentials", "true");
    dfSerial.begin(9600, SERIAL_8N1, 16, 17, false, 20000);
    uint32_t rr = esp_reset_reason();
    myDFPlayer.begin(dfSerial, true, (rr & 0xfffffff7) == 1);
    dbg_ws_print(String(myDFPlayer.readType(), HEX));
    DIAG("D0");
    myDFPlayer.setTimeOut(500);
    DIAG("D0a");
    myDFPlayer.EQ(0);
    DIAG("D0b");
    myDFPlayer.outputDevice(2);
    DIAG("D0c");
    if ((rr & 0xfffffff7) == 1) { myDFPlayer.volume(0); myDFPlayer.enableDAC(); }
    DIAG("D1");
    led_defaults();                                   // @400e0464
    FastLED.addLeds<WS2812B, 27, GRB>(g_c2f5a, 30).setCorrection(TypicalLEDStrip);
    DIAG("D2");
    FastLED.setMaxPowerInMilliWatts(2500);
    FastLED.setDither(0);
    for (int i = 0; i < 3; i++) {                     // «Bouncing balls»: 3 м'ячі
      g_c20ac[i] = millis();
      g_c20dc[i] = 1.0f;
      g_c20b8[i] = 0;
      g_c20d0[i] = g_bec1c;
      g_c20c4[i] = 0;
      g_c20a0[i] = (float)(0.9 - (double)((float)i / 1.0f) / 9.0);
    }
    g_c34ac = doc["led_fx"].as<String>().toInt();
    g_c34aa = doc["led_fx_tw"].as<String>().toInt();
    DIAG("D3");
    led_off();                                        // @400e4224
    DIAG("D4");
    fx_status_end();
    config_apply_all();
    fx_time_boot();                                   // нове: час після перезапуску (timefix.cpp)
    DIAG("D5");
    display_zones_init();                             // @400d8824
    DIAG("D6");
    wifi_start_wrapper();
    DIAG("D7");
    lang_sync();
    networks_load2();
    lang_installed_load();                            // @400fd7e0
    DIAG("D8");
    hist_init();                                      // @400fd9a0
  }
}

// ---------- loop @40121d5c ----------
void loop() {
  ws.cleanupClients(2);
  // Виправлено: у автора — щоциклу, навіть коли DNS captive-порталу не запущено (звичайний режим клієнта): тоді
  // WiFiUDP::parsePacket() на неіснуючому сокеті щоразу виділяє й звільняє 1460 Б і пише в Serial «parsePacket():
  // could not receive data: 9» — ~180 рядків/с, цикл упирається в швидкість порту (115200 бод), анімація табло
  // оновлюється ривками, пам'ять дрібниться.
  if (g_fx_dns_on) dnsServer.processNextRequest();
  if (g_c3090 == 1) mqtt_loop();
  ha_discovery_step();
  f_400e4200();
  uint8_t gol = g_c2f1b;
  if (g_c3000) {
    g_c3000 = 0;
    // виправлено: в автора — випадковий трек години (1–23); тепер — повне оголошення поточного часу (sound.cpp)
    if (g_c2fc9 == 1) { (void)gol; g_fx_voice_test = true; g_c2f21 = 99; }
    else if (g_c2fc9 == 2) g_c2f21 = 99;
  }
  if (g_c2ffd) { g_c2ffd = 0; manual_time_set(); }
  if (g_c2ffe && (g_c2ffe = 0, g_c2d29) && g_c3319 == 0) { g_c22b4 = g_c3319; g_c22b5 = 0; sys_sync_ntp(); }
  if (g_c2fff) {
    g_c2fff = 0;
    g_c2d28 = 1;
    config_save_atomic();
    delay(200);
    df_silence();
    ESP.restart();
  }
  if (g_c3380) {
    g_c3384 = millis();
    g_c34a9 = 0; g_c34a5 = 0; g_c34a8 = 0; g_c34a7 = 0; g_c34a6 = 0; g_c34af = 0;
    g_c34b5++;
    config_set("Cwi", "2");
    config_apply_all();
    WiFi.mode(WIFI_AP_STA);
    wifi_start();
    g_c3380 = 0;
    g_c335b = 0;
    if (g_c2d29 == 0 && g_c33d4.length() != 0 && g_c33d4 != g_c33ec) {
      dbg_ws_print(String("Wifi        : connect to '") + g_c33ec + "' failed — restoring last good '" + g_c33d4 + "'");
      evlog_write(String("[WIFI] manual connect failed, restored ") + g_c33d4);
      g_c33ec = g_c33d4;
      g_c33e0 = g_c33c8;
      config_set("ssid", g_c33ec);
      config_set("pass", g_c33e0);
      g_c2d28 = 1;
      config_save_atomic();
      wifi_start();
    }
  }
  if (g_c335b == 0) {                                  // g_c335b = 1 ставить config_apply_all; 0 — треба застосувати
    config_apply_all();
    if (g_c3090 == 1 && g_c30c5) { g_c244c = 0; mqtt_ha_publish_state(); }
  }
  // Виправлено: в автора весь блок нижче чекав кінця анімації зони 0 (P.getZoneStatus(0)). Поки біжить
  // новина (2–4,5 хв), стояли яскравість, нічний режим, ранкове ввімкнення табло, стан для веб-інтерфейсу,
  // а за анімації, що не закінчується, — і відновлення мережі (годинник «не видно в мережі» до перезапуску).
  // Швидкі задачі (без мережі й без зміни зон табло) — щоциклу:
  rtc_magic = 0xc10c0fe5; rtc_stage = 6;
  display_brightness_tick(1000);
  fx_bright_tick();
  fx_night_tick();
  fx_wdbg_tick();
  if (millis() - g_fx_dst_t > 60000) { g_fx_dst_t = millis(); fx_time_offset_apply(); }   // переходи літнього часу
  night_mode_tick(10000);
  ws_state_tick_5s(5000);
  night_lowmem_reboot_tick(60000);
  // Мережа, MQTT, датчики, історія (блокують на секунди — смикали б прокрутку) — як в автора, після анімації,
  // але не рідше ніж раз на NET_DEFER_MAX_MS.
  if (g_fx_quiet || P.getZoneStatus(0) || millis() - g_fx_gate > NET_DEFER_MAX_MS) {
    g_fx_gate = millis();
    rtc_magic = 0xc10c0fe5; rtc_stage = 1;
    net_check_periodic(90000);
    wifi_reconnect_cycle(5000);
    dbg_ram_print(7000);
    dbg_time_print(120000);
    content_cleanup_30s(30000);
    if (millis() - g_c22dc > 3999) { g_c22dc = millis(); hist_init(); }
    if (millis() - g_c22e0 > 29999) { g_c22e0 = millis(); hist_sample(); }
  }
  if (g_c3334.length() != 0) {
    String s = g_c3334; g_c3334 = "";
    networks_forget(s);
  }
  if (g_c3328.length() != 0) {
    String ssid = g_c3328; g_c3328 = "";
    String pass; networks_password(pass, ssid);
    config_set("ssid", ssid);
    config_set("pass", pass);
    g_c2d28 = 1;
    g_c3374 = "connecting";
    g_c3380 = 1;
  }
  if (g_c331c.length() != 0) {
    String s = g_c331c; g_c331c = "";
    networks_set_auto(s, g_c331a);
  }
  rtc_magic = 0xc10c0fe5; rtc_stage = 3;
  wifi_scan_harvest_json();
  if (g_c2e89) {
    g_c2e89 = 0;
    SPIFFS.remove("/event_log.txt");
    SPIFFS.remove("/event_log.old");
    g_c2e88 = 0;
    g_c2e78 = String("");
  }
  if (g_c2e8b && g_c2e88 == 0) {
    g_c2e8b = g_c2e88;
    File f = SPIFFS.open(g_c2e8a == 0 ? "/event_log.txt" : "/event_log.old", "r");
    if (!f) g_c2e78 = "(журнал пуст)";
    else {
      g_c2e78 = f.readString();
      f.close();
      if (g_c2e78.length() == 0) g_c2e78 = "(журнал пуст)";
    }
    g_c2e88 = 1;
    g_c2e84 = millis();
  }
  if (g_c2e88 && millis() - g_c2e84 > 20000) { g_c2e88 = 0; g_c2e78 = String(""); }
  led_lang_upload_process();
  if (g_c34c4) {
    g_c34c4 = 0;
    if ((WiFi.getMode() & WIFI_STA) && WiFi.scanComplete() != -1) {
      if (g_c34c5 || g_c1624 == 0 || millis() - g_c1624 > 8000) {
        rtc_magic = 0xc10c0fe5; rtc_stage = 2;
        WiFi.scanDelete();
        WiFi.scanNetworks(true, false, false, 300);
        g_c1624 = millis();
      }
      g_c34c5 = 0;
    }
  }
  if (g_c3424 && (g_c3424 = 0, g_c2d29) && g_c3319 == 0) {
    dbg_ws_print(String("NTP        : forced sync requested from UI"));
    g_c22b4 = 0; g_c22b5 = 0;
    rtc_magic = 0xc10c0fe5; rtc_stage = 4;
    sys_sync_ntp();
  }
  alarm_tick(1000);                                   // @400e4bec
  led_tick();                                         // @400f3ecc
  button_tick();                                      // @4011bb90
  if (g_c2e99) { g_c2e99 = 0; config_upload_apply(); }
  if (g_c2eec == 1 || g_c2eec == 2) ota_update();
  if (g_c2fc9 == 1) voice_hourly();
  else if (g_c2fc9 == 2) buzzer_hourly();
  rtc_magic = 0xc10c0fe5; rtc_stage = 5;
  if (g_fx_quiet) sensors_read_all();                 // нове: табло погашене — вміст стоїть, датчики читаємо (night.cpp)
  else if (g_c34ba == 4) {
    if (g_c3470 == 1) content_cycle_4m();
    else if (g_c3470 == 0) content_cycle_default();
    else if (g_c3470 == 2) content_cycle_4m_mixed(g_c3428 * 1000);
  } else if (g_c34ba == 8) {
    if (g_c3471 == 1) content_cycle_8m_mixed(g_c3428 * 1000);
    else if (g_c3471 == 0) content_cycle_default();
    else if (g_c3471 == 2) content_cycle_8m();
  } else content_cycle_default();
  rtc_magic = 0xc10c0fe5; rtc_stage = 0;
  loop_guard_feed();
}

// ---------- журнал подій ----------
// @400d5470: мітка часу журналу
String evlog_ts() {
  if (year() < 2024) return String("up+") + String(millis() / 1000, 10) + String("s");
  char b[20];
  snprintf(b, 20, "%04d-%02d-%02d %02d:%02d:%02d", year(), month(), day(), hour(), minute(), second());
  return String(b);
}
// @400fb79c
void evlog_write(const String &msg) {
  File f = SPIFFS.open("/event_log.txt", "r");
  if (f) {
    size_t sz = f.size();
    f.close();
    if (sz > 0xfff) {
      SPIFFS.remove("/event_log.old");
      SPIFFS.rename("/event_log.txt", "/event_log.old");
    }
  }
  File w = SPIFFS.open("/event_log.txt", "a");
  if (w) {
    String ts = evlog_ts();
    w.write('[');
    w.print(ts);
    w.write(']');
    for (int n = 0x13 - (int)ts.length(); n > 0; n--) w.write(' ');
    w.write(' ');
    int e;
    if (msg.length() == 0 || msg[0] != '[' || (e = msg.indexOf(']')) < 1) {
      w.println(msg);
    } else {
      int i = 0;
      do { w.write(msg[i]); i++; } while (i <= e);
      for (int n = 8 - e; n > 0; n--) w.write(' ');
      w.write(' ');
      i = e + 1;
      if (i < (int)msg.length()) {
        if (msg[i] == ' ') i = e + 2;
        for (; i < (int)msg.length(); i++) w.write(msg[i]);
      }
      w.println();
    }
    w.close();
  }
}
// @400fd62c
void evlog_boot_reason() {
  uint32_t r = esp_reset_reason();
  String s("");
  switch (r) {
    case 1: s = "Power-on reset"; break;
    case 2: s = "External reset"; break;
    case 3: s = "Software reset"; break;
    case 4: s = "CRASH (panic/exception)"; break;
    case 5: case 6: case 7: s = "WATCHDOG reset"; break;
    case 8: s = "Wake from deep-sleep"; break;
    case 9: s = "BROWNOUT (power drop)"; break;
    default: s = String("unknown code=") + String((int)r); break;
  }
  File f = SPIFFS.open("/event_log.txt", "a");
  if (f) { f.println(); f.close(); }
  evlog_write(String("[BOOT] ") + s);
}

// ---------- конфігурація: рядок JSON g_c2ef0 ↔ /config.json ----------
// @400ffe18
void config_load_file() {
  File f = SPIFFS.open(String("/config.json"), "r");
  if (!f || f.size() == 0) {
    if (f) f.close();
    File t = SPIFFS.open(String("/config.tmp"), "r");
    if (!t || t.size() == 0) {
      if (t) t.close();
      // "Failed to open config file"
      config_load_defaults();
      g_c2d28 = 1;
      config_save_atomic();
      return;
    }
    t.close();
    // "config.json empty — restoring from config.tmp"
    SPIFFS.remove(String("/config.json"));
    SPIFFS.rename(String("/config.tmp"), String("/config.json"));
    f = SPIFFS.open(String("/config.json"), "r");
  }
  g_c2ef0 = "";
  g_c2ef0.reserve(f.size() + 1);
  g_c2ef0 = f.readString();
  f.close();
  // g_c2ef0 + "  [config loaded]" — налагоджувальний рядок, не виводиться
}
// @400fc6bc
void config_save_atomic() {
  if (!g_c2d28) return;
  unsigned int n = g_c2ef0.length();
  if (n < 2 || g_c2ef0[0] != '{' || g_c2ef0[n - 1] != '}') return;   // "Skip config save — json_config looks invalid (low heap?)"
  File f = SPIFFS.open(String("/config.tmp"), "w");
  if (!f) return;                                                     // "Error opening config tmp for writing."
  size_t w = f.print(g_c2ef0);
  f.close();
  if (n != w) { SPIFFS.remove(String("/config.tmp")); return; }       // "Config tmp write incomplete — keeping old config"
  SPIFFS.remove(String("/config.json"));
  SPIFFS.rename(String("/config.tmp"), String("/config.json"));
  // "Save Config (atomic)"
  g_c2d28 = 0;
}
// @400eb54c
void config_load_defaults() {
  config_defaults();
  // "Loading default config"
}
// @400f3dbc: записати ключ у рядок конфігурації
void config_set(const String &key, const String &value) {
  JsonDocument doc;
  config_doc_load(doc);          // результат автор не перевіряє
  doc[key] = value;              // @400f2a50
  config_doc_store(doc);
}
// @400d4bb4
void df_silence() {
  myDFPlayer.pause();
  myDFPlayer.volume(0);
  delay(20);
}
// @400fd7e0
void lang_installed_load() {
  File f = SPIFFS.open("/lang/installed.json", "r");
  if (!f) { g_c2dc8 = "[]"; return; }
  String s = f.readString();
  f.close();
  g_c2dc8 = s.length() == 0 ? String("[]") : s;
}
// @40100274: застосувати завантажений файл конфігурації
void config_upload_apply() {
  if (config_upload_is_json() == 0 || config_upload_replace() == 0) evlog_write(String("[CFG] upload ignored — file is not plain JSON"));
  else evlog_write(String("[CFG] uploaded JSON applied — rebooting"));
  WiFi.disconnect(false);
  delay(2000);
  df_silence();
  ESP.restart();
}

// ---------- кнопка GPIO0 ----------
// @4011bb90
void button_tick() { button_check(); config_save_atomic(); }
// @4011b154
void button_check() {
  if (digitalRead(0) == 1) return;
  uint32_t t0 = millis();
  while (digitalRead(0) == 0) {
    if (millis() - t0 > 4999) {
      if (g_c33f8 == 1) { config_set("Cwi", "2"); config_save_and_restart(); }
      if (g_c33f8 == 2) { config_set("Cwi", "1"); config_save_and_restart(); }
    }
    if (millis() - t0 > 14999) {
      SPIFFS.remove(String("/config.json"));
      WiFi.disconnect(false);
      delay(1000);
      df_silence();
      ESP.restart();
    }
    if (millis() - t0 > 99 && g_c34b5 == 0) {
      if (g_c2d29 == 0) {
        if (g_c33f8 == 1) { config_set("Cwi", "2"); g_c33f8 = 2; }
        g_c3384 = millis();
        g_c34a9 = 0; g_c34a5 = 0; g_c34a8 = 0; g_c34a7 = 0; g_c34a6 = 0; g_c34af = 0;
        g_c34b5++;
        if (g_c2d29 == 0) WiFi.mode(WIFI_STA);
        wifi_start();
      } else {
        button_next_section();
      }
    }
  }
}
// @400fc8ac
void config_save_and_restart() {
  g_c2d28 = 1;
  config_save_atomic();
  WiFi.disconnect(false);
  delay(1000);
  df_silence();
  ESP.restart();
}
// @4011afdc: коротке натискання — перейти до наступного розділу інформаційного циклу
void button_next_section() {
  if (g_c34af == g_c27c8 || g_c34a9 != 3) return;
  if (g_c3471) { clockface_effect_select(); g_c3471 = 0; }
  if (g_c3470) { clockface_effect_select(); g_c3470 = 0; }
  g_c34a4 = 0x37; g_c34a3 = 0x37; g_c34b4 = 0x18; g_c34b3 = 0x16;
  uint8_t s = g_c34af, n;
  if (s < 8) n = 8;
  else if ((uint8_t)(s - 8) < 5) n = 0xd;
  else if ((uint8_t)(s - 0xd) < 0x11) n = 0x1e;
  else if ((uint8_t)(s - 0x1e) < 7) n = 0x25;
  else if ((uint8_t)(s - 0x25) < 3) n = 0x28;
  else if ((uint8_t)(s - 0x28) < 4) n = 0x2c;
  else if ((uint8_t)(s - 0x2c) < 3) n = 0x2f;
  else if ((uint8_t)(s - 0x2f) < 3) n = 0x32;
  else if ((uint8_t)(s - 0x32) < 3) n = 0x35;
  else if ((uint8_t)(s - 0x35) < 7) n = 0x3c;
  else n = s;
  g_c34af = n;
  g_c27c8 = g_c34af;
  String t("");
  content_slot_text(g_c34af, t);
  utf8_to_led(t, (char *)g_c27f9, 0x400);
  P.displayZoneText(0, "", PA_CENTER, 0, 0, PA_PRINT, PA_NO_EFFECT);
  P.displayClear(0);
  P.displayReset(0);
}

// @400f3da4: розібрати рядок конфігурації в документ
bool config_doc_load(JsonDocument &d) { return deserializeJson(d, g_c2ef0) == DeserializationError::Ok; }
// @400e968c: документ → рядок конфігурації
void config_doc_store(JsonDocument &d) { g_c2ef0 = ""; serializeJson(d, g_c2ef0); }
