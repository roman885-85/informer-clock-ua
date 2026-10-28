// MQTT (AsyncMqttClient): підключення, команди, стан, датчики, автовиявлення Home Assistant.
// Перенесено з 2.7.1 (адреси оригіналу в коментарях).
#include "fw.h"

#define mqttClient g_c2628

// Таблиці сутностей Home Assistant @3f4068a0 (числа/перемикачі) і @3f4065c8 (списки)
struct HaNum { const char *key, *name; bool isSwitch; int min, max; const char *icon, *devId, *devName; };
struct HaSel { const char *key, *name, *devId, *devName, *icon; uint8_t count; const char *const *options; const int *values; };
static const char *const OPT_SND[] = { "Off", "Voice", "Buzzer" };
static const int VAL_SND[] = { 5, 1, 2 };
static const char *const OPT_VOICE[] = { "Maxim (M, RU)", "Sem (M, RU)", "Tatjana (F, RU)", "Google (F, UA)", "Salli (F, US)", "Ethel (F, EE)" };
static const int VAL_VOICE[] = { 1, 2, 3, 4, 5, 6 };
static const char *const OPT_BUZ[] = { "Beep 1", "Beep 2", "Beep 3" };
static const int VAL_BUZ[] = { 1, 2, 3 };
static const char *const OPT_AS[] = { "Off", "Buzzer 1", "Buzzer 2", "Buzzer 3", "MP3: Are you ready Kids?", "MP3: Soul", "MP3: Space", "MP3: Active" };
static const int VAL_AS[] = { 0, 1, 2, 3, 4, 5, 6, 7 };
static const char *const OPT_AL[] = { "Off", "Breathe", "Sunrise", "Fade", "Strobe" };
static const int VAL_AL[] = { 0, 1, 2, 3, 4 };
static const char *const OPT_AD[] = { "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday", "Everyday", "Workdays", "Weekends" };
static const int VAL_AD[] = { 2, 3, 4, 5, 6, 7, 1, 8, 9, 10 };
static const char *const OPT_FX[] = { "Random", "Twinkles", "Static color", "Rainbow", "Rainbow 2", "Fade", "Breathe", "Balls", "ColorBounce",
  "ColorBounce Fade", "Color diods", "Fire", "Water", "Confetti", "Bpm", "Rainbow Waves", "Ocean Waves", "Sinelon", "Juggle", "Plasma",
  "Resetvars", "Rippless", "Jelly", "Color Water", "Fountain", "Matrix Ray", "Cry", "Beatwave" };
static const int VAL_FX[] = { 27, 26, 25, 0, 7, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24 };
const HaNum HA_NUM[7] = {
  { "speed", "Scroll speed", false, 10, 60, "mdi:play-speed", "disp", "Display" },
  { "lt_snd", "Live text sound", true, 0, 0, "mdi:bullhorn", "disp", "Display" },
  { "lt_rep", "Live text repeats", false, 1, 10, "mdi:repeat", "disp", "Display" },
  { "led_speed", "LED speed", false, 1, 254, "mdi:speedometer", "led", "LED" },
  { "bud", "Alarm", true, 0, 0, "mdi:alarm", "snd", "Sound & Alarm" },
  { "DFvol", "Voice volume", false, 0, 30, "mdi:volume-high", "snd", "Sound & Alarm" },
  { "n_vc", "Night mode", true, 0, 0, "mdi:weather-night", "snd", "Sound & Alarm" } };
const HaSel HA_SEL[7] = {
  { "onoffv", "Sound mode", "snd", "Sound & Alarm", "mdi:volume-high", 3, OPT_SND, VAL_SND },
  { "DFgol", "Voice", "snd", "Sound & Alarm", "mdi:account-voice", 6, OPT_VOICE, VAL_VOICE },
  { "z", "Buzzer pattern", "snd", "Sound & Alarm", "mdi:bullhorn", 3, OPT_BUZ, VAL_BUZ },
  { "as1", "Alarm sound", "snd", "Sound & Alarm", "mdi:music-note", 8, OPT_AS, VAL_AS },
  { "aL1", "Alarm LED", "snd", "Sound & Alarm", "mdi:led-on", 5, OPT_AL, VAL_AL },
  { "ad1", "Alarm day", "snd", "Sound & Alarm", "mdi:calendar-week", 10, OPT_AD, VAL_AD },
  { "led_fx", "LED effect", "led", "LED", "mdi:auto-fix", 28, OPT_FX, VAL_FX } };
static const char *const HA_SENSOR_KEYS[7] = { "temp", "hum", "pressure", "co2", "tvoc", "rssi", "heap" };   // @3f400020
static const char *const MQTT_REASONS[8] = {   // @3f40617c
  "TCP disconnected (не тот хост/порт, брокер недоступен или закрыл сокет)", "unacceptable protocol version", "client id rejected",
  "server unavailable", "malformed credentials", "not authorized (неверный логин/пароль или ACL брокера)", "not enough heap (ESP8266)",
  "TLS bad fingerprint" };

// ---------- параметри й топіки ----------
// @400d8aa0
void mqtt_params_changed() {
  if (g_c2590 != 0) {
    dbg_ws_print(String("MQTT broker params changed — reapplying + reconnecting"));
    mqttClient.disconnect(false);
    mqttClient.setClientId(g_c25f4.c_str());
    mqttClient.setCredentials(g_c260c.c_str(), g_c2600.c_str());
    mqttClient.setServer(g_c261c.c_str(), g_c2618);
    g_bed1c = 1;
  }
}
// @400d8b60: параметри брокера (адресу очищено від схеми, шляху й порту)
void mqtt_set_params(const String &clientId, String &host, uint16_t port, const String &user, const String &pass) {
  host.trim();
  int i = host.indexOf(String("://"));
  if (-1 < i) host = host.substring(i + 3);
  i = host.indexOf('/');
  if (-1 < i) host = host.substring(0, i);
  i = host.indexOf(':');
  if (-1 < i) host = host.substring(0, i);
  host.trim();
  g_c25f4 = clientId;
  g_c261c = host;
  g_c2618 = port;
  g_c260c = user;
  g_c2600 = pass;
}
// @400d8c9c: MAC без двокрапок, малими
void mqtt_mac(String &out) {
  out = WiFi.macAddress();
  out.replace(String(":"), String(""));
  out.toLowerCase();
}
// @400d8d3c
void ha_client_id(String &out) {
  String m; mqtt_mac(m);
  out = String("clockinformer_") + m;
}
// @400d8de0
void mqtt_base(String &out) {
  String m; mqtt_mac(m);
  out = String("clockinformer/") + m;
}
// @400d8e84
void mqtt_status_topic(String &out) {
  String b; mqtt_base(b);
  out = b + "/status";
}
// @400d8f04: заповіт «offline»
void mqtt_set_will() {
  if (g_c3090 != 0) {
    static String willTopic;
    String t; mqtt_status_topic(t);
    willTopic = t;
    mqttClient.setWill(willTopic.c_str(), 1, true, "offline", 0);
  }
}
// @400d6970 / @400d6a78 / @400d6b20
void on_mqtt_subscribe(uint16_t packetId, uint8_t qos) {
  dbg_ws_print(String("Subscribe packetId: ") + String(packetId, 10) + String("  qos: ") + String(qos, 10));
}
void on_mqtt_unsubscribe(uint16_t packetId) { dbg_ws_print(String("Unsubscribe.  packetId: ") + String(packetId, 10)); }
void on_mqtt_publish(uint16_t packetId) { dbg_ws_print(String("Publish.   packetId: ") + String(packetId, 10)); }
// @400ec5ec
void on_mqtt_connect(bool sessionPresent) {
  g_c30c5 = 1;
  JsonDocument doc;
  String s("");
  doc["mq_stat"] = String("1");
  serializeJson(doc, s);
  ws.printfAll(String(s).c_str());
  dbg_ws_print(String("Connected to MQTT.  session=") + String(sessionPresent, 10));
  g_bed1c = 0;
  mqtt_publish_online();
}
// @400ec438
void on_mqtt_disconnect(AsyncMqttClientDisconnectReason reason) {
  g_c30c5 = 0;
  JsonDocument doc;
  String s("");
  doc["mq_stat"] = String("0");
  serializeJson(doc, s);
  ws.printfAll(String(s).c_str());
  for (int i = 0; i < 12; i++) {
    g_c2464[i] = 0;
    mqttClient.unsubscribe(g_c2500[i].c_str());
  }
  uint8_t r = (uint8_t)reason;
  const char *rs = "unknown";
  if (r < 8) rs = MQTT_REASONS[r];
  dbg_ws_print(String("Disconnected from MQTT: ") + rs);
  if (WiFi.isConnected() && g_c3319 == 0) g_bed1c = 1;
}
// @400f8f7c
void on_mqtt_message(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  String p = String(payload).substring(0, len);
  dbg_ws_print(String("MQTT msg  topic=") + String(topic) + "  payload=" + p);
  if (mqtt_on_message(String(topic), p) == 0) {
    JsonDocument doc;
    String s("");
    for (int i = 0; i < 12; i++) {
      if (g_c2500[i].equals(String(topic))) {
        g_c2470[i] = p;
        break;
      }
    }
    String t;
    mqtt_text(t, 0x1f); doc[String("mqt1")] = t;
    mqtt_text(t, 0x20); doc[String("mqt2")] = t;
    mqtt_text(t, 0x21); doc[String("mqt3")] = t;
    mqtt_text(t, 0x22); doc[String("mqt4")] = t;
    serializeJson(doc, s);
    ws.printfAll(String(s).c_str());
  }
}
// @400d8fb8
void mqtt_setup() {
  mqttClient.onConnect(on_mqtt_connect);
  mqttClient.onDisconnect(on_mqtt_disconnect);
  mqttClient.onSubscribe(on_mqtt_subscribe);
  mqttClient.onUnsubscribe(on_mqtt_unsubscribe);
  mqttClient.onMessage(on_mqtt_message);
  mqttClient.onPublish(on_mqtt_publish);
  mqttClient.setCleanSession(true);
  mqttClient.setClientId(g_c25f4.c_str());
  mqttClient.setCredentials(g_c260c.c_str(), g_c2600.c_str());
  mqttClient.setServer(g_c261c.c_str(), g_c2618);
  mqtt_set_will();
}
// @400d9128
void mqtt_connect() {
  if (g_c2590 == 0) {
    mqtt_setup();
    g_c2590 = 1;
  }
  dbg_ws_print(String("Connecting to MQTT..."));
  mqttClient.connect();
}
// @400d9198
void mqtt_on_wifi() {
  dbg_ws_print(String("Connected to Wi-Fi."));
  mqtt_connect();
}
// @400d91f0: значення з підписаного топіка (перший виклик підписує; до 12 топіків)
void mqtt_value(String &out, const String &topic, uint8_t qos) {
  if (g_c261c == "") {
    out = "";
    return;
  }
  if (g_c3319 != 0) g_c18af = 0;
  int i;
  for (i = 0; i < 12; i++)
    if (g_c2500[i].equals(topic)) goto found;
  for (i = 0; g_c2500[i] != ""; ) {
    i++;
    if (i == 12) goto nofree;
  }
  g_c2500[i] = topic;
found:
  g_c18ae = i;
nofree:
  if (g_c18af == 0 && g_c2d29 != 0) {
    int8_t was = g_c18af;
    mqtt_on_wifi();
    g_c18af = 1;
    g_bed1c = was;
  }
  if (g_c2464[g_c18ae] == 0 && g_c30c5 != 0) {
    mqttClient.subscribe(g_c2500[g_c18ae].c_str(), qos);
    g_c2464[g_c18ae] = 1;
  }
  out = g_c2470[g_c18ae];
}
// @400d92d8 / @400d9300 / @400d9330: що є з датчиків
uint8_t ha_has_temp() { return (g_c32e8 == 0 && g_c32e7 == 0 && g_c32e5 == 0 && g_c32e2 == 0) ? 0 : 1; }
bool ha_has_hum() {
  bool r = true;
  if (g_c32e7 == 0 && (g_c32e5 == 0 || g_c2ee5 != 0)) r = g_c32e2 != 0;
  return r;
}
bool ha_has_tvoc() { return g_c32e1 != 0 && g_c32e0 < 2; }
// @400d9354: температура з першого наявного датчика (BME/BMP → AM2320/HDC/AHT → DHT → DS18B20), інакше NaN
float ha_temp() {
  if (g_c32e5 != 0) return g_c3314;
  if (g_c32e2 != 0) return g_c32fc;
  if (g_c32e7 != 0) return g_c3308;
  if (g_c32e8 != 0) return g_c3300;
  return NAN;
}
// @400d9398
float ha_hum() {
  if (g_c32e5 != 0 && g_c2ee5 == 0) return g_c3310;
  if (g_c32e2 != 0) return g_c32f8;
  if (g_c32e7 != 0) return g_c3304;
  return NAN;
}
// @400d93d4
void ha_temp_sensor_name(String &out) {
  const char *p;
  if (g_c32e5 != 0) {
    if (g_c2ee5 == 0) p = "BME280";
    else p = (g_c2ee5 == 1) ? "BMP280" : "BMP180";
  } else if (g_c32e2 != 0) {
    if (g_c32e3 == 0) p = "AM2320";
    else p = (g_c32e3 == 1) ? "HDC1080" : "AHT10";
  } else if (g_c32e7 != 0) {
    p = (g_c32e6 == 11) ? "DHT11/12" : "DHT22";
  } else if (g_c32e8 != 0) {
    p = "DS18B20";
  } else {
    out = "";
    return;
  }
  out = p;
}
// @400d9458
void ha_air_sensor_name(String &out) {
  const char *p = "MH-Z19B";
  if (g_c32e0 != 2) {
    if (g_c32e0 == 1) p = "CCS811";
    else if (g_c32e0 == 3) p = "SCD40";
    else if (g_c32e0 == 4) p = "SCD41";
    else p = "SGP30";
  }
  out = p;
}
// @400d9490
void ha_name_join(String &out, const String &a, const String &b) {
  if (a.length() == 0) out = b;
  else out = a + " " + b;
}
// @400d9518
void mqtt_topic(String &out, const String &key) {
  String b; mqtt_base(b);
  out = b + "/" + key;
}
// @400d95a4: значення списку за ключем
int ha_select_value(const char *key) {
  String k(key);
  uint8_t *p;
  if (k.equals(String("onoffv"))) p = &g_c2fc9;
  else if (k.equals(String("DFgol"))) p = &g_c2f1b;
  else if (k.equals(String("z"))) p = &g_c2fb4;
  else if (k.equals(String("as1"))) p = &g_c2fc7;
  else if (k.equals(String("aL1"))) p = &g_c2fc5;
  else if (k.equals(String("ad1"))) p = &g_c2fc6;
  else if (k.equals(String("led_fx"))) p = &g_c2f27;
  else return -1;
  return *p;
}
// @400d9758
void mqtt_cmd_topic(String &out, const String &key) {
  String t; mqtt_topic(t, key);
  out = t + "/set";
}
// @400d97dc: стан числа/перемикача за ключем
void ha_number_state(String &out, const char *key) {
  String k(key);
  uint8_t *b;
  uint8_t v;
  if (k.equals(String("led_pix"))) { b = &g_c2f28; goto onoff; }
  if (k.equals(String("led_brg"))) { v = g_c2f24; goto num; }
  if (k.equals(String("bud"))) { b = &g_c2fc8; goto onoff; }
  if (k.equals(String("DFvol"))) { v = g_c2f1c; goto num; }
  if (k.equals(String("n_vc"))) { b = &g_c2f1d; goto onoff; }
  if (k.equals(String("led_speed"))) { v = g_c2f29; goto num; }
  if (k.equals(String("speed"))) { v = g_bf04d; goto num; }
  if (k.equals(String("lt_snd"))) { b = &g_c2f19; goto onoff; }
  if (k.equals(String("lt_rep"))) { v = g_c2f18; goto num; }
  out = "";
  return;
onoff:
  out = (*b == 0) ? "0" : "1";
  return;
num:
  out = String(v, 10);
}
// @400d9a38
void mqtt_subscribe_cmds() {
  if (g_c3090 != 0) {
    String b; mqtt_base(b);
    String t = b + "/+/set";
    mqttClient.subscribe(t.c_str(), 0);
  }
}
// @400d9ae8: команди з топіків …/<ключ>/set (1 — оброблено)
uint32_t mqtt_on_message(const String &topic, const String &payload) {
  if (g_c3090 == 0) return 0;
  String t;
  mqtt_cmd_topic(t, String("brightness"));
  if (topic.equals(t)) {
    int v = payload.toInt();
    g_bed18 = v < 0 ? 0 : (v < 15 ? v : 15);
    return 1;
  }
  mqtt_cmd_topic(t, String("brmode"));
  if (topic.equals(t)) {
    if (payload.equals(String("Manual"))) g_bed14 = 0;
    else if (payload.equals(String("Sensor"))) g_bed14 = 1;
    else if (payload.equals(String("Schedule"))) g_bed14 = 2;
    else g_bed14 = -1;
    return 1;
  }
  mqtt_cmd_topic(t, String("display"));
  if (topic.equals(t)) {
    g_bed10 = payload.equals(String("1")) ? 1 : (payload.equals(String("ON")) != 0);
    return 1;
  }
  mqtt_cmd_topic(t, String("text"));
  if (topic.equals(t)) { g_c2440 = payload; g_c243c = 1; return 1; }
  mqtt_cmd_topic(t, String("text2"));
  if (topic.equals(t)) { g_c2430 = payload; g_c242c = 1; return 1; }
  mqtt_cmd_topic(t, String("livetext"));
  if (topic.equals(t)) { g_c2420 = payload; g_c241c = 1; return 1; }
  mqtt_cmd_topic(t, String("test"));
  if (topic.equals(t)) { g_c3000 = 1; return 1; }
  mqtt_cmd_topic(t, String("synctime"));
  if (topic.equals(t)) { g_c2fff = 1; return 1; }
  mqtt_cmd_topic(t, String("reboot"));
  if (topic.equals(t)) { g_c2ffe = 1; return 1; }
  String t2;
  mqtt_cmd_topic(t2, String("al1"));
  bool timeKey = topic.equals(t2);
  if (!timeKey) {
    mqtt_cmd_topic(t2, String("t_m"));
    timeKey = topic.equals(t2);
    if (!timeKey) {
      mqtt_cmd_topic(t, String("d_m"));
      timeKey = topic.equals(t);
    }
  }
  if (timeKey) {
    const char *k = "al1";
    mqtt_cmd_topic(t2, String("al1"));
    if (!topic.equals(t2)) {
      mqtt_cmd_topic(t, String("t_m"));
      k = topic.equals(t) ? "t_m" : "d_m";
    }
    if (g_c22e8 < 8) {
      g_c23ac[g_c22e8] = k;
      g_c234c[g_c22e8] = payload;
      g_c22ec[g_c22e8] = payload;
      g_c22e8++;
    }
    if ((*k & 0xef) == 100) g_c2ffd = 1;   // t_m або d_m — встановити час
    return 1;
  }
  mqtt_cmd_topic(t, String("led_json"));
  if (topic.equals(t)) { g_c2410 = payload; g_c240c = 1; return 1; }
  for (int i = 0; i < 7; i++) {
    mqtt_cmd_topic(t, String(HA_NUM[i].key));
    if (topic.equals(t)) {
      if (7 < g_c22e8) return 1;
      g_c23ac[g_c22e8] = HA_NUM[i].key;
      g_c234c[g_c22e8] = payload;
      g_c22ec[g_c22e8] = payload;
      g_c22e8++;
      return 1;
    }
  }
  for (int i = 0; i < 7; i++) {
    const char *key = HA_SEL[i].key;
    mqtt_cmd_topic(t, String(key));
    if (topic.equals(t)) {
      for (uint8_t j = 0; j < HA_SEL[i].count; j++) {
        if (payload.equals(HA_SEL[i].options[j])) {
          int v = HA_SEL[i].values[j];
          if (-1 < v && g_c22e8 < 8) {
            g_c23ac[g_c22e8] = key;
            g_c234c[g_c22e8] = String(v, 10);
            g_c22ec[g_c22e8] = payload;
            g_c22e8++;
          }
          return 1;
        }
      }
      return 1;
    }
  }
  return 0;
}
// @400da380: повідомити веб-інтерфейс {"ключ":"значення"}
void ws_notify_kv(const char *key, const String &value) {
  if (ws.availableForWriteAll()) {
    String v = value;
    v.replace(String("\\"), String("\\\\"));
    v.replace(String("\""), String("\\\""));
    String s = String("{\"") + key + "\":\"" + v + "\"}";
    ws.textAll(s.c_str());
  }
}
// @400da50c: прибрати всі сутності Home Assistant (порожні збережені config) і оголосити offline
void mqtt_ha_remove() {
  String c, t;
  for (int i = 0; i < 7; i++) {
    const char *k = HA_SENSOR_KEYS[i];
    ha_client_id(c);
    t = String("homeassistant/sensor/") + c + "/" + k + "/config";
    mqttClient.publish(t.c_str(), 0, true, "", 0, false, 0);
  }
  static const char *const FIXED[][2] = {
    { "homeassistant/binary_sensor/", "/inet/config" }, { "homeassistant/number/", "/brightness/config" },
    { "homeassistant/select/", "/brmode/config" }, { "homeassistant/switch/", "/display/config" },
    { "homeassistant/light/", "/display/config" }, { "homeassistant/image/", "/photo/config" },
    { "homeassistant/text/", "/text/config" }, { "homeassistant/text/", "/text2/config" },
    { "homeassistant/text/", "/livetext/config" }, { "homeassistant/text/", "/alarmTime/config" },
    { "homeassistant/text/", "/manualTime/config" }, { "homeassistant/text/", "/manualDate/config" },
    { "homeassistant/button/", "/synctime/config" }, { "homeassistant/button/", "/reboot/config" } };
  for (int i = 0; i < 14; i++) {
    ha_client_id(c);
    t = String(FIXED[i][0]) + c + FIXED[i][1];
    mqttClient.publish(t.c_str(), 0, true, "", 0, false, 0);
  }
  for (int i = 0; i < 7; i++) {
    String comp(HA_NUM[i].isSwitch ? "switch" : "number");
    ha_client_id(c);
    t = String("homeassistant/") + comp + "/" + c + "/" + HA_NUM[i].key + "/config";
    mqttClient.publish(t.c_str(), 0, true, "", 0, false, 0);
  }
  for (int i = 0; i < 7; i++) {
    ha_client_id(c);
    t = String("homeassistant/select/") + c + "/" + HA_SEL[i].key + "/config";
    mqttClient.publish(t.c_str(), 0, true, "", 0, false, 0);
  }
  static const char *const FIXED2[][2] = {
    { "homeassistant/button/", "/test/config" }, { "homeassistant/switch/", "/z/config" },
    { "homeassistant/switch/", "/led_pix/config" }, { "homeassistant/number/", "/led_brg/config" },
    { "homeassistant/light/", "/led/config" }, { "homeassistant/light/", "/ledj/config" } };
  for (int i = 0; i < 6; i++) {
    ha_client_id(c);
    t = String(FIXED2[i][0]) + c + FIXED2[i][1];
    mqttClient.publish(t.c_str(), 0, true, "", 0, false, 0);
  }
  mqtt_status_topic(t);
  mqttClient.publish(t.c_str(), 1, true, "offline", 0, false, 0);
}
// @400db13c
void mqtt_publish_online() {
  if (g_c3090 != 0) {
    String t; mqtt_status_topic(t);
    mqttClient.publish(t.c_str(), 1, true, "online", 0, false, 0);
    g_bed0c = 0;
    g_c22e4 = 0;
  }
}

// ---------- Home Assistant: стан ----------
// @400eb580: стан стрічки (JSON-схема світла)
void mqtt_publish_led_state() {
  if (g_c3090 != 0 && g_c30c5 != 0) {
    JsonDocument doc;
    doc["state"] = (g_c2f28 == 0) ? "OFF" : "ON";
    doc["brightness"] = g_c2f24;
    doc["color_mode"] = "hs";
    JsonObject color = doc["color"].to<JsonObject>();
    color["h"] = lround(g_c2f1e / 255.0f * 360.0f);
    color["s"] = lround(g_bf042 / 255.0f * 100.0f);
    String s("");
    serializeJson(doc, s);
    String t; mqtt_topic(t, String("led_json"));
    mqttClient.publish(t.c_str(), 0, true, s.c_str(), 0, false, 0);
  }
}
// @400eb7f0: усі стани (не частіше ніж раз на 30 с)
void mqtt_ha_publish_state() {
  if (g_c3090 == 0 || g_c30c5 == 0) return;
  if (g_c244c != 0 && millis() - g_c244c <= 29999) return;
  g_c244c = millis();
  mqtt_publish_led_state();
  yield();
  String t, v;
  if (ha_has_temp() && !isnan(ha_temp())) {
    mqtt_topic(t, String("temp"));
    v = String(ha_temp(), 1);
    mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  }
  if (ha_has_hum() && !isnan(ha_hum())) {
    mqtt_topic(t, String("hum"));
    v = String(ha_hum(), 0);
    mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  }
  if (g_c32e5 != 0) {
    mqtt_topic(t, String("pressure"));
    v = String(g_c330c, 0);
    mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  }
  if (g_c32e1 != 0) {
    mqtt_topic(t, String("co2"));
    v = String(g_c32f0, 10);
    mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  }
  if (ha_has_tvoc()) {
    mqtt_topic(t, String("tvoc"));
    v = String(g_c32f4, 10);
    mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  }
  yield();
  mqtt_topic(t, String("rssi"));
  v = String((int)WiFi.RSSI(), 10);
  mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  mqtt_topic(t, String("heap"));
  v = String(ESP.getFreeHeap(), 10);
  mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  mqtt_topic(t, String("inet"));
  mqttClient.publish(t.c_str(), 0, true, g_c3319 == 0 ? "1" : "0", 0, false, 0);
  mqtt_topic(t, String("photo"));
  v = String("http://") + WiFi.localIP().toString() + "/clock.svg";
  mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  yield();
  mqtt_topic(t, String("brightness"));
  v = String(g_c2ee8, 10);
  mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  mqtt_topic(t, String("brmode"));
  mqttClient.publish(t.c_str(), 0, true, g_c2ee9 == 0 ? "Manual" : (g_c2ee9 == 1 ? "Sensor" : "Schedule"), 0, false, 0);
  mqtt_topic(t, String("display"));
  mqttClient.publish(t.c_str(), 0, true, g_c3080 == 0 ? "1" : "0", 0, false, 0);
  yield();
  mqtt_topic(t, String("text"));
  mqttClient.publish(t.c_str(), 0, true, g_c3034.c_str(), 0, false, 0);
  mqtt_topic(t, String("text2"));
  mqttClient.publish(t.c_str(), 0, true, g_c3028.c_str(), 0, false, 0);
  mqtt_topic(t, String("al1"));
  mqttClient.publish(t.c_str(), 0, true, g_c2fb8.c_str(), 0, false, 0);
  mqtt_topic(t, String("t_m"));
  mqttClient.publish(t.c_str(), 0, true, g_c344c.c_str(), 0, false, 0);
  mqtt_topic(t, String("d_m"));
  mqttClient.publish(t.c_str(), 0, true, g_c3440.c_str(), 0, false, 0);
  yield();
  for (int i = 0; i < 7; i++) {
    const char *k = HA_NUM[i].key;
    mqtt_topic(t, String(k));
    ha_number_state(v, k);
    mqttClient.publish(t.c_str(), 0, true, v.c_str(), 0, false, 0);
  }
  yield();
  for (int i = 0; i < 7; i++) {
    const char *k = HA_SEL[i].key;
    mqtt_topic(t, String(k));
    int val = ha_select_value(k);
    const char *opt = "";
    for (uint8_t j = 0; j < HA_SEL[i].count; j++) {
      if (val == HA_SEL[i].values[j]) {
        opt = HA_SEL[i].options[j];
        break;
      }
    }
    mqttClient.publish(t.c_str(), 0, true, opt, 0, false, 0);
    yield();
  }
}

// ---------- Home Assistant: автовиявлення ----------
// @400eeea0: опис пристрою (або підпристрою subId/subName)
void ha_device(JsonObject dev, const char *subId, const char *subName) {
  JsonArray ids = dev["ids"].to<JsonArray>();
  JsonVariant id = ids.add<JsonVariant>();
  String nm = g_c3084.length() == 0 ? String("Clock Informer") : g_c3084;
  String c;
  if (subId == nullptr || *subId == 0) {
    ha_client_id(c);
    id.set(c);
    dev["name"] = nm;
  } else {
    ha_client_id(c);
    id.set(c + "_" + subId);
    dev["name"] = nm + " " + subName;
    ha_client_id(c);
    dev["via_device"] = c;
  }
  dev["mf"] = "Sem Prygorodov";
  dev["mdl"] = "Clock Informer";
  dev["sw"] = String("GoldArtOS ") + "2.7.1";
  dev["hw"] = String("ESP32");
  dev["cu"] = String("http://") + WiFi.localIP().toString() + "/";
  if (subId == nullptr || *subId == 0) {
    mqtt_mac(c);
    dev["sn"] = c;
  }
}
// @400ef36c: «dev» і «o» (походження)
static void ha_dev_origin(JsonDocument &doc, const char *subId = nullptr, const char *subName = nullptr) {
  ha_device(doc["dev"].to<JsonObject>(), subId, subName);
  JsonObject o = doc["o"].to<JsonObject>();
  o["name"] = "GoldArtOS";
  o["sw"] = "2.7.1";
  o["url"] = "https://goldart.ee";
}
// опублікувати config сутності
static void ha_publish_config(JsonDocument &doc, const String &topic) {
  String s("");
  serializeJson(doc, s);
  mqttClient.publish(topic.c_str(), 0, true, s.c_str(), 0, false, 0);
}
static String ha_topic(const char *comp, const char *key) {
  String c; ha_client_id(c);
  return String("homeassistant/") + comp + "/" + c + "/" + key + "/config";
}
// @400ef400: датчик
void ha_sensor(const char *comp, const String &key, const String &name, const char *devClass, const char *unit, bool diag) {
  JsonDocument doc;
  doc["name"] = name;
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_" + key;
  String t; mqtt_topic(t, key);
  doc["stat_t"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  if (devClass != nullptr) doc["dev_cla"] = devClass;
  if (unit != nullptr) doc["unit_of_meas"] = unit;
  doc["stat_cla"] = "measurement";
  if (diag) doc["ent_cat"] = "diagnostic";
  ha_dev_origin(doc);
  ha_publish_config(doc, ha_topic(comp, key.c_str()));
}
// @400ef7a8
void ha_inet() {
  JsonDocument doc;
  doc["name"] = "Internet";
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_inet";
  String t; mqtt_topic(t, String("inet"));
  doc["stat_t"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  doc["dev_cla"] = "connectivity";
  doc["pl_on"] = "1";
  doc["pl_off"] = "0";
  doc["ent_cat"] = "diagnostic";
  ha_dev_origin(doc);
  ha_client_id(c);
  ha_publish_config(doc, String("homeassistant/binary_sensor/") + c + "/inet/config");
}
// @400efb5c
void ha_display_light() {
  JsonDocument doc;
  doc["name"] = "Display";
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_displight";
  String t; mqtt_topic(t, String("display"));
  doc["stat_t"] = t;
  mqtt_cmd_topic(t, String("display"));
  doc["cmd_t"] = t;
  doc["pl_on"] = "1";
  doc["pl_off"] = "0";
  mqtt_topic(t, String("brightness"));
  doc["bri_stat_t"] = t;
  mqtt_cmd_topic(t, String("brightness"));
  doc["bri_cmd_t"] = t;
  doc["bri_scl"] = 15;
  doc["ic"] = "mdi:brightness-6";
  ha_dev_origin(doc, "disp", "Display");
  ha_client_id(c);
  ha_publish_config(doc, String("homeassistant/light/") + c + "/display/config");
}
// @400f0074: текстове поле
void ha_text(const char *key, const String &name, const char *icon, const char *devId, const char *devName, const char *pattern, int max) {
  JsonDocument doc;
  doc["name"] = name;
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_" + key;
  String t; mqtt_cmd_topic(t, String(key));
  doc["cmd_t"] = t;
  mqtt_topic(t, String(key));
  doc["stat_t"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  doc["max"] = max;
  if (pattern != nullptr) doc["pattern"] = pattern;
  doc["ic"] = icon;
  ha_dev_origin(doc, devId, devName);
  ha_publish_config(doc, ha_topic("text", key));
}
// @400f046c / @400f0788 / @400f1734: кнопки
static void ha_button(const char *name, const char *key, const char *icon, const char *devClass, const char *devId, const char *devName) {
  JsonDocument doc;
  doc["name"] = name;
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_" + key;
  String t; mqtt_cmd_topic(t, String(key));
  doc["cmd_t"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  doc["ic"] = icon;
  if (devClass != nullptr) doc["dev_cla"] = devClass;
  ha_dev_origin(doc, devId, devName);
  ha_client_id(c);
  ha_publish_config(doc, String("homeassistant/button/") + c + "/" + key + "/config");
}
void ha_btn_synctime() { ha_button("Sync time (NTP)", "synctime", "mdi:clock-check", nullptr, nullptr, nullptr); }
void ha_btn_reboot() { ha_button("Reboot", "reboot", "mdi:restart", "restart", nullptr, nullptr); }
void ha_btn_test() { ha_button("Test sound", "test", "mdi:play", nullptr, "snd", "Sound & Alarm"); }
// @400f0ad8: зображення циферблата
void ha_image() {
  JsonDocument doc;
  doc["name"] = "Clock";
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_photo";
  String t; mqtt_topic(t, String("photo"));
  doc["url_topic"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  ha_dev_origin(doc);
  ha_client_id(c);
  ha_publish_config(doc, String("homeassistant/image/") + c + "/photo/config");
}
// @400f0dc4: число або перемикач з таблиці
static void ha_number(const HaNum *e) {
  JsonDocument doc;
  doc["name"] = e->name;
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_" + e->key;
  String t; mqtt_cmd_topic(t, String(e->key));
  doc["cmd_t"] = t;
  mqtt_topic(t, String(e->key));
  doc["stat_t"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  doc["ic"] = e->icon;
  if (!e->isSwitch) {
    doc["min"] = e->min;
    doc["max"] = e->max;
    doc["step"] = 1;
    doc["mode"] = "slider";
  } else {
    doc["pl_on"] = "1";
    doc["pl_off"] = "0";
  }
  ha_dev_origin(doc, e->devId, e->devName);
  ha_publish_config(doc, ha_topic(e->isSwitch ? "switch" : "number", e->key));
}
// @400f12b0: список з таблиці
static void ha_select(const HaSel *e) {
  JsonDocument doc;
  doc["name"] = e->name;
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_" + e->key;
  String t; mqtt_cmd_topic(t, String(e->key));
  doc["cmd_t"] = t;
  mqtt_topic(t, String(e->key));
  doc["stat_t"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  JsonArray opts = doc["options"].to<JsonArray>();
  for (uint8_t i = 0; i < e->count; i++) opts.add(e->options[i]);
  doc["ic"] = e->icon;
  ha_dev_origin(doc, e->devId, e->devName);
  ha_publish_config(doc, ha_topic("select", e->key));
}
// @400f1a54
void ha_brmode() {
  JsonDocument doc;
  doc["name"] = "Brightness mode";
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_brmode";
  String t; mqtt_cmd_topic(t, String("brmode"));
  doc["cmd_t"] = t;
  mqtt_topic(t, String("brmode"));
  doc["stat_t"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  JsonArray opts = doc["options"].to<JsonArray>();
  opts.add("Manual");
  opts.add("Sensor");
  opts.add("Schedule");
  doc["ic"] = "mdi:brightness-auto";
  ha_dev_origin(doc, "disp", "Display");
  ha_client_id(c);
  ha_publish_config(doc, String("homeassistant/select/") + c + "/brmode/config");
}
// @400f1e5c: стрічка як світло (JSON-схема); старий запис …/led/config прибирається
void ha_led_light() {
  JsonDocument doc;
  doc["name"] = "LED strip";
  String c; ha_client_id(c);
  doc["uniq_id"] = c + "_ledlight";
  doc["schema"] = "json";
  String t; mqtt_topic(t, String("led_json"));
  doc["stat_t"] = t;
  mqtt_cmd_topic(t, String("led_json"));
  doc["cmd_t"] = t;
  mqtt_status_topic(t);
  doc["avty_t"] = t;
  doc["brightness"] = true;
  doc["brightness_scale"] = 255;
  JsonArray modes = doc["supported_color_modes"].to<JsonArray>();
  modes.add("hs");
  doc["ic"] = "mdi:led-strip-variant";
  ha_dev_origin(doc, "led", "LED");
  String s("");
  serializeJson(doc, s);
  ha_client_id(c);
  t = String("homeassistant/light/") + c + "/led/config";
  mqttClient.publish(t.c_str(), 0, true, "", 0, false, 0);
  ha_client_id(c);
  t = String("homeassistant/light/") + c + "/ledj/config";
  mqttClient.publish(t.c_str(), 0, true, s.c_str(), 0, false, 0);
}
// @400f23a8: один крок автовиявлення (1 — усе опубліковано)
uint32_t ha_discovery_item(int i) {
  String a, b, n;
  switch (i) {
    case 0:
      if (!ha_has_temp()) return 0;
      a = "temp"; ha_temp_sensor_name(b); ha_name_join(n, b, String("Temperature"));
      ha_sensor("sensor", a, n, "temperature", "°C", false);
      break;
    case 1:
      if (!ha_has_hum()) return 0;
      a = "hum"; ha_temp_sensor_name(b); ha_name_join(n, b, String("Humidity"));
      ha_sensor("sensor", a, n, "humidity", "%", false);
      break;
    case 2:
      if (g_c32e5 == 0) return 0;
      a = "pressure"; ha_temp_sensor_name(b); ha_name_join(n, b, String("Pressure"));
      ha_sensor("sensor", a, n, "atmospheric_pressure", "mmHg", false);
      break;
    case 3:
      if (g_c32e1 == 0) return 0;
      a = "co2"; ha_air_sensor_name(b); ha_name_join(n, b, String("CO2"));
      ha_sensor("sensor", a, n, "carbon_dioxide", "ppm", false);
      break;
    case 4:
      if (!ha_has_tvoc()) return 0;
      a = "tvoc"; ha_air_sensor_name(b); ha_name_join(n, b, String("TVOC"));
      ha_sensor("sensor", a, n, "volatile_organic_compounds_parts", "ppb", false);
      break;
    case 5: ha_sensor("sensor", String("rssi"), String("Wi-Fi signal"), "signal_strength", "dBm", true); break;
    case 6: ha_sensor("sensor", String("heap"), String("Free memory"), "data_size", "B", true); break;
    case 7: ha_inet(); break;
    case 8: ha_display_light(); break;
    case 9: ha_brmode(); break;
    case 10: {
      String c; ha_client_id(c);
      String t = String("homeassistant/switch/") + c + "/display/config";
      mqttClient.publish(t.c_str(), 0, true, "", 0, false, 0);
      ha_client_id(c);
      t = String("homeassistant/number/") + c + "/brightness/config";
      mqttClient.publish(t.c_str(), 0, true, "", 0, false, 0);
      break;
    }
    case 11: ha_text("text", String("Loop text 1"), "mdi:message-text", "disp", "Display", nullptr, 120); break;
    case 12: ha_text("text2", String("Loop text 2"), "mdi:message-text-outline", "disp", "Display", nullptr, 120); break;
    case 13: ha_text("livetext", String("Live text"), "mdi:bullhorn", "disp", "Display", nullptr, 120); break;
    case 14: ha_text("al1", String("Alarm time"), "mdi:alarm", "snd", "Sound & Alarm", "^([01][0-9]|2[0-3]):[0-5][0-9]$", 5); break;
    case 15: ha_text("t_m", String("Set time"), "mdi:clock-edit", nullptr, nullptr, "^([01][0-9]|2[0-3]):[0-5][0-9]$", 5); break;
    case 16: ha_text("d_m", String("Set date"), "mdi:calendar-edit", nullptr, nullptr, "^[0-9]{4}-[0-1][0-9]-[0-3][0-9]$", 10); break;
    case 17: ha_btn_synctime(); break;
    case 18: ha_btn_reboot(); break;
    case 19: ha_image(); break;
    default:
      if ((unsigned)(i - 20) < 7) ha_number(&HA_NUM[i - 20]);
      else if ((unsigned)(i - 27) < 7) ha_select(&HA_SEL[i - 27]);
      else if (i - 27 == 7) ha_btn_test();
      else if (i - 27 == 8) ha_led_light();
      else return 1;
  }
  return 0;
}
// @400f28e8: автовиявлення по одній сутності раз на 120 мс
void ha_discovery_step() {
  if (g_bed0c < 0 || g_c3090 == 0 || g_c30c5 == 0) return;
  if (g_c22e4 != 0 && millis() - g_c22e4 < 120) return;
  g_c22e4 = millis();
  if (ha_discovery_item(g_bed0c) == 0) {
    g_bed0c++;
    return;
  }
  g_bed0c = -1;
  mqtt_subscribe_cmds();
  g_c244c = 0;
  mqtt_ha_publish_state();
}

// ---------- цикл: застосування команд, що надійшли ----------
// @400f3ef0
void mqtt_loop() {
  bool changed = false;
  String t;
  if (-1 < g_bed18) {   // яскравість табло
    g_c2ee8 = g_bed18;
    P.setIntensity(g_bed18 & 0xff);
    g_c34b4 = g_bed18;
    config_set(String("brg"), String(g_bed18, 10));
    g_c2d28 = 1;
    mqtt_topic(t, String("brightness"));
    mqttClient.publish(t.c_str(), 0, true, String(g_bed18, 10).c_str(), 0, false, 0);
    ws_notify_kv("brg", String(g_bed18, 10));
    g_bed18 = -1;
  }
  if (-1 < g_bed14) {   // режим яскравості
    g_c2ee9 = g_bed14;
    g_c34b4 = 0x18;
    g_c34b3 = 0x18;
    config_set(String("bright"), String(g_bed14, 10));
    g_c2d28 = 1;
    mqtt_topic(t, String("brmode"));
    uint8_t m = g_bed14;
    mqttClient.publish(t.c_str(), 0, true, m == 0 ? "Manual" : (m == 1 ? "Sensor" : "Schedule"), 0, false, 0);
    ws_notify_kv("bright", String(g_bed14, 10));
    g_bed14 = -1;
  }
  if (-1 < g_bed10) {   // табло вкл/викл
    g_c3080 = g_bed10 == 0;
    P.displayShutdown(g_c3080);
    mqtt_topic(t, String("display"));
    mqttClient.publish(t.c_str(), 0, true, g_c3080 == 0 ? "1" : "0", 0, false, 0);
    g_bed10 = -1;
  }
  bool t1 = g_c243c != 0;
  if (t1) {
    config_set(String("text"), g_c2440);
    mqtt_topic(t, String("text"));
    mqttClient.publish(t.c_str(), 0, true, g_c2440.c_str(), 0, false, 0);
    ws_notify_kv("text", g_c2440);
    g_c2440 = String("");
    g_c243c = 0;
  }
  changed = g_c242c != 0;
  if (changed) {
    config_set(String("text2"), g_c2430);
    mqtt_topic(t, String("text2"));
    mqttClient.publish(t.c_str(), 0, true, g_c2430.c_str(), 0, false, 0);
    ws_notify_kv("text2", g_c2430);
    g_c2430 = String("");
    g_c242c = 0;
  }
  changed = changed || t1;
  if (g_c241c != 0) {   // живий текст
    g_c3004 = g_c2420;
    g_c3001 = 1;
    mqtt_topic(t, String("livetext"));
    mqttClient.publish(t.c_str(), 0, false, g_c2420.c_str(), 0, false, 0);
    g_c2420 = String("");
    g_c241c = 0;
  }
  if (g_c240c != 0) {   // стрічка: JSON від Home Assistant
    g_c240c = 0;
    JsonDocument doc;
    if (deserializeJson(doc, g_c2410) == DeserializationError::Ok) {
      if (!doc["state"].isNull()) {
        bool on = doc["state"].as<String>().equals(String("ON"));
        config_set(String("led_pix"), String(on ? "1" : "0"));
      }
      if (!doc["brightness"].isNull()) {
        int b = doc["brightness"].as<int>();
        b = b < 0 ? 0 : (b < 255 ? b : 255);
        config_set(String("led_brg"), String(b, 10));
      }
      changed = true;
      if (!doc["color"].isNull()) {
        float h = doc["color"]["h"].as<float>();
        float s = doc["color"]["s"].as<float>();
        int hh = lround(h / 360.0 * 255.0);
        int ss = lround(s / 100.0 * 255.0);
        hh = hh < 0 ? 0 : (hh < 255 ? hh : 255);
        ss = ss < 0 ? 0 : (ss < 255 ? ss : 255);
        config_set(String("stat_ledo"), String(hh, 10));
        config_set(String("stat_leds"), String(ss, 10));
        config_set(String("led_fx"), String(25, 10));
        changed = true;
      }
    }
    g_c2410 = String("");
  }
  for (uint8_t i = 0; ; i = i + 1) {   // відкладені ключі (числа, списки, будильник, ручний час)
    if (g_c22e8 <= i) {
      g_c22e8 = 0;
      if (changed) {
        g_c2d28 = 1;
        g_c335b = 0;
      }
      return;
    }
    String &k = g_c23ac[i];
    String &v = g_c234c[i];
    config_set(k, v);
    String *dst = nullptr;
    if (k.equals(String("t_m"))) dst = &g_c344c;
    else if (k.equals(String("d_m"))) dst = &g_c3440;
    else if (k.equals(String("al1"))) dst = &g_c2fb8;
    if (dst != nullptr) *dst = v;
    mqtt_topic(t, k);
    mqttClient.publish(t.c_str(), 0, true, g_c22ec[i].c_str(), 0, false, 0);
    ws_notify_kv(k.c_str(), v);
    k = String("");
    v = String("");
    g_c22ec[i] = String("");
    changed = true;
  }
}

// ---------- значення датчиків у власні топіки ----------
// @400f6ef8 (раз на g_c19e0 мс; після першого разу — 32 с)
void mqtt_publish_sensors() {
  if (g_c30c5 != 0) {
    if (millis() - g_c19e4 < g_c19e0) return;
    g_c19e4 = millis();
    String v, t;
    if (g_c32e8 != 0 && g_c3070 != "") {   // DS18B20
      mqttClient.publish(g_c3070.c_str(), 0, true, String(g_c3300, 1).c_str(), 0, false, 0);
      dbg_ws_print(String("MQTT: SENDING... DS18B20: ") + String(g_c3300, 1) + "°C");
    }
    if (g_c32e7 != 0 && g_c3064 != "") {   // DHT
      mqttClient.publish(g_c3064.c_str(), 0, true, String(g_c3308, 1).c_str(), 0, false, 0);
      mqttClient.publish((String(g_c3064) + "1").c_str(), 0, true, String(g_c3304, 0).c_str(), 0, false, 0);
      dbg_ws_print(String("MQTT: SENDING... DHT t=") + String(g_c3308, 1) + "°C  hum=" + String(g_c3304, 0) + "%  topic=" + String(g_c3064));
    }
    if (g_c32e5 != 0 && g_c3058 != "") {   // BME280 / BMP280 / BMP180
      String p1, p2, p3;
      if (g_c2ee5 == 1 || g_c2ee5 == 2) {
        mqttClient.publish(g_c3058.c_str(), 0, true, String(g_c3314, 1).c_str(), 0, false, 0);
        mqttClient.publish((String(g_c3058) + "1").c_str(), 0, true, String(g_c330c, 0).c_str(), 0, false, 0);
        led_phrase(p2, g_c34b0, 0x48);
        led_phrase(p3, g_c34b0, 0x4b);
        dbg_ws_print(String(g_c2ee5 == 1 ? "MQTT: SENDING... BMP280: " : "MQTT: SENDING... BMP180: ") + String(g_c3314, 1) + "°C " + p2 + String(g_c330c, 0) + p3);
      } else if (g_c2ee5 == 0) {
        mqttClient.publish(g_c3058.c_str(), 0, true, String(g_c3314, 1).c_str(), 0, false, 0);
        mqttClient.publish((String(g_c3058) + "1").c_str(), 0, true, String(round(g_c3310), 0).c_str(), 0, false, 0);
        mqttClient.publish((String(g_c3058) + "2").c_str(), 0, true, String(g_c330c, 0).c_str(), 0, false, 0);
        led_phrase(p1, g_c34b0, 0x47);
        led_phrase(p2, g_c34b0, 0x48);
        led_phrase(p3, g_c34b0, 0x4b);
        dbg_ws_print(String("MQTT: SENDING... BME280: ") + String(g_c3314, 1) + "°C " + p1 + String(round(g_c3310), 0) + "% " + p2 + String(g_c330c, 0) + p3);
      }
    }
    if (g_c32e2 != 0 && g_c304c != "") {   // AM2320 / HDC1080 / AHT10
      mqttClient.publish(g_c304c.c_str(), 0, true, String(g_c32fc, 1).c_str(), 0, false, 0);
      mqttClient.publish((String(g_c304c) + "1").c_str(), 0, true, String(g_c32f8, 0).c_str(), 0, false, 0);
      const char *nm = nullptr;
      if (g_c32e3 == 1) nm = "MQTT: SENDING... HDC1080: ";
      else if (g_c32e3 == 0) nm = "MQTT: SENDING... AM2320: ";
      else if (g_c32e3 == 2) nm = "MQTT: SENDING... AHT10: ";
      if (nm != nullptr) {
        String p1; led_phrase(p1, g_c34b0, 0x47);
        dbg_ws_print(String(nm) + String(g_c32fc, 1) + "°C " + p1 + String(g_c32f8, 0) + "%");
      }
    }
    if (g_c32e1 != 0 && g_c3040 != "") {   // CO2 / TVOC
      if (g_c32e0 == 1 || g_c32e0 == 0) {
        mqttClient.publish(g_c3040.c_str(), 0, true, String(g_c32f0, 10).c_str(), 0, false, 0);
        mqttClient.publish((String(g_c3040) + "1").c_str(), 0, true, String(g_c32f4, 10).c_str(), 0, false, 0);
      } else if (g_c32e0 < 5) {
        mqttClient.publish(g_c3040.c_str(), 0, true, String(g_c32f0, 10).c_str(), 0, false, 0);
      }
      if (g_c32e0 == 1 || g_c32e0 == 0) {
        dbg_ws_print(String(g_c32e0 == 1 ? "MQTT: SENDING... CCS811: " : "MQTT: SENDING... SGP30: ") + String(g_c32f0, 10) + "ppm " + String(g_c32f4, 10) + "ppb");
      } else {
        if (4 < g_c32e0) return;
        dbg_ws_print(String("MQTT: SENDING... CO2: ") + String(g_c32f0, 10) + "ppm ");
      }
    }
  }
  g_c19e0 = 32000;
}

// ---------- тексти з MQTT для табло ----------
// @400f8358: значення топіка (і топіка+«1», +«2») з префіксом і одиницями за типом датчика
void mqtt_format(String &out, const String &topic, const String &prefix, uint8_t type) {
  String v1(""), v2(""), v3("");
  if (topic != "") {
    String p1, p2, p3;
    if (type == 0) {
      mqtt_value(v1, topic, 0);
      if (v1 != "") { out = String(prefix) + v1 + "°C "; return; }
    } else if (type == 10) {
      mqtt_value(v1, topic, 0);
      if (v1 != "") { out = String(prefix) + v1; return; }
    } else if ((uint8_t)(type - 11) < 2 || type == 9) {
      mqtt_value(v1, topic, 0);
      if (v1 != "") { out = String(prefix) + v1 + "ppm "; return; }
    } else if (type == 3) {
      mqtt_value(v1, topic, 0);
      mqtt_value(v2, String(topic) + "1", 0);
      mqtt_value(v3, String(topic) + "2", 0);
      if (v1 != "" && v2 != "" && v3 != "") {
        led_phrase(p1, g_c34b0, 0x47);
        led_phrase(p2, g_c34b0, 0x48);
        led_phrase(p3, g_c34b0, 0x4b);
        out = String(prefix) + v1 + "°C " + p1 + v2 + "% " + p2 + v3 + p3;
        return;
      }
    } else if ((uint8_t)(type - 7) < 2) {
      mqtt_value(v1, topic, 0);
      mqtt_value(v2, String(topic) + "1", 0);
      if (v1 != "" && v2 != "") { out = String(prefix) + v1 + "ppm " + v2 + "ppb"; return; }
    } else {
      mqtt_value(v1, topic, 0);
      mqtt_value(v2, String(topic) + "1", 0);
      if (v1 != "" && v2 != "") {
        led_phrase(p1, g_c34b0, 0x47);
        out = String(prefix) + v1 + "°C " + p1 + v2 + "% ";
        return;
      }
    }
  }
  out = String("");
}
// @400f8f08: текст MQTT-слота (' ' — 2-й, '!' — 3-й, '"' — 4-й, інакше 1-й)
void mqtt_text(String &out, char which) {
  if (g_c30c5 == 0) {
    out = String("");
    return;
  }
  int i;
  uint8_t type;
  if (which == '!') { i = 2; type = g_c307d; }
  else if (which == '"') { i = 3; type = g_c307c; }
  else if (which == ' ') { i = 1; type = g_c307e; }
  else { i = 0; type = g_c307f; }
  dbg_ws_sep(1);
  mqtt_format(out, g_c25c4[i], g_c2594[i], type);
}
