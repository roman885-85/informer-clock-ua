// Періодичні службові такти: налагоджувальні рядки часу й пам'яті, MQTT/narodmon/будильник (раз на 7 с),
// EN: Periodic service ticks: debug lines (time, memory), MQTT/narodmon/alarm, low-memory night reboot (from 2.7.1).
// нічний перезапуск при браку пам'яті, завантаження конфігурації файлом. Перенесено з 2.7.1 (адреси в коментарях).
#include "fw.h"

// @40108d18
void narodmon_read_cfg() {
  String dev(g_c2fcc);
  narodmon_read(g_c2fd8, dev);
}
// @400fdcd8: завантажений файл починається (після пробілів) з '{' або '['
bool config_upload_is_json() {
  File f;
  f = SPIFFS.open(String("/config_upload.json"), "r");
  if (!f) return false;
  uint8_t c;
  do {
    if (!f.available()) {
      f.close();
      return false;
    }
    c = f.read();
  } while ((uint8_t)(c - 9) < 0x18 && ((1u << (c - 9)) & 0x800013u) != 0);
  f.close();
  return (c & 0xdf) == 0x5b;
}
// @401001a4: замінити /config.json завантаженим файлом
bool config_upload_replace() {
  SPIFFS.remove(String("/config.json"));
  bool ok = SPIFFS.rename(String("/config_upload.json"), String("/config.json"));
  if (ok) config_load_file();
  // інакше — "Failed to replace config file with uploaded JSON" (налагоджувальний рядок, не виводиться)
  return ok;
}
// @400fbf74: о 4-й ночі, якщо найбільший вільний блок < 30000 Б (і робота > 1 год, рік ≥ 2024, режим STA) — перезапуск
void night_lowmem_reboot_tick(uint32_t ms) {
  if (millis() - g_c15cc < ms) return;
  g_c15cc = millis();
  if (millis() > 3599999 && g_c33f8 == 2 && year() > 2023 && hour() == 4 && heap_max_block() < 30000) {
    evlog_write(String("[SYS ] Night reboot — low memory  maxBlock=") + String(heap_max_block(), 10) + String(" B"));
    dbg_ws_print(String("Night low-memory reboot. maxFreeBlock=") + String(heap_max_block(), 10));
    delay(100);
    df_silence();
    ESP.restart();
  }
}
// @400f6714: час системи / NTP / RTC у налагодження
void dbg_time_print(uint32_t ms) {
  if (millis() - g_c15e8 < ms) return;
  g_c15e8 = millis();
  g_c34b5 = 0;
  rtc_detect_sync();
  dbg_ws_sep(1);
  String t, d;
  time_hm_text(t, 1);
  date_text(d, 1);
  dbg_ws_print(String("SYSTEM Time : ") + t + String(":") + String(second(), 10) + String("   Date: ") + d);
  if (g_c2d29 != 0 && g_c2eb4 != 0) {
    time_hm_text(t, 2);
    date_text(d, 2);
    dbg_ws_print(String("NTP Time    : ") + t + String(":") + String(g_c3460, 10) + String("   Date: ") + d);
  }
  if (g_c3468 != 0) {
    DateTime n = g_c346c.now();
    time_hm_text(t, 3);
    date_text(d, 3);
    dbg_ws_print(String("RTC Time    : ") + t + String(":") + String(n.second(), 10) + String("   Date: ") + d);
  }
  manual_time_print();
  dbg_ws_sep(2);
}
// @4010bdd4: раз на ms — пам'ять, фоторезистор, зони, MQTT (параметри, датчики, стан, Home Assistant), narodmon, будильник
void dbg_ram_print(uint32_t ms) {
  if (millis() - g_c1620 < ms) return;
  g_c1620 = millis();
  dbg_ws_sep(2);
  dbg_ws_print(String("RAM         : ") + String(ESP.getFreeHeap(), 10));
  dbg_ws_sep(2);
  fx_photo_print();                                   // виправлено: замір і рівень — щоразу в bright.cpp, тут лише рядок
  f_400e4c90();
  if (g_c30c4 == 1 || g_c3090 == 1) {
    if (g_c3319 != 0) goto alarm;
    if (g_c30b8 != "" && g_c30ac != "" && g_c30a0 != "" && g_c3094 != "") {
      static String lastHost, lastPort, lastUser, lastPass;
      if (g_c15ed == 0) {
        lastHost = g_c30b8;
        lastPort = g_c30ac;
        lastUser = g_c30a0;
        lastPass = g_c3094;
        g_c15ed = 1;
      } else if (g_c30b8 != lastHost || g_c30ac != lastPort || g_c30a0 != lastUser || g_c3094 != lastPass) {
        lastHost = g_c30b8;
        lastPort = g_c30ac;
        lastUser = g_c30a0;
        lastPass = g_c3094;
        String id = String("SemUI-") + WiFi.macAddress();
        String host(g_c30b8), user(g_c30a0), pass(g_c3094);
        mqtt_set_params(id, host, g_c30ac.toInt(), user, pass);
        mqtt_params_changed();
      }
      if (g_c30c5 == 0) {
        String id = String("SemUI-") + WiFi.macAddress();
        String host(g_c30b8), user(g_c30a0), pass(g_c3094);
        mqtt_set_params(id, host, g_c30ac.toInt(), user, pass);
        f_400e4210();
      }
      if (g_c30c4 == 1) mqtt_publish_sensors();
      if (g_c3090 == 1) mqtt_ha_publish_state();
      if (g_c15ec == 1 && g_c3090 == 0 && g_c30c5 != 0) mqtt_ha_remove();
      if (g_c15ec == 0 && g_c3090 == 1 && g_c30c5 != 0) mqtt_publish_online();
      g_c15ec = g_c3090;
    }
  }
  if (g_c3319 == 0) { FxStatusScope fxs(FXS_DATA); narodmon_tick(); }   // нове: стан на табло під час запуску
alarm:
  if (g_c2fc8 == 1 && g_c2fb8 != "") f_400e1480();
  g_c2f20 = 1;
  g_c34b3 = 0x16;
  g_c2f1f = 1;
}
