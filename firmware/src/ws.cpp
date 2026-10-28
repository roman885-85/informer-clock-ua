// WebSocket /ws: налагоджувальна консоль, стан кожні 5 с, «зайнято» для другого клієнта (перенесено з 2.7.1)
#include "fw.h"

static String two(uint8_t v) { return v < 10 ? String("0") + String(v, 10) : String(v, 10); }

// @400d6878: рядок налагодження {"dbg":"…"} усім клієнтам
void dbg_ws_print(const String &s) {
#ifdef COPY_DIAG
  Serial.println(s);   // лише діагностична збірка (env:diag), не частина копії
#endif
  if (ws.count() != 0 && ws.availableForWriteAll()) {
    String o("");
    o.reserve(s.length() + 0xe);
    o += "{\"dbg\":\"";
    for (unsigned int i = 0; i < s.length(); i++) {
      int c = s[i];
      if (c == 0x22 || c == 0x5c) o += (char)0x5c;
      else if (c == 10 || c == 0xd) c = 0x20;
      o += (char)c;
    }
    o += "\"}";
    ws.textAll(o);
  }
}
// @400d3288: подія WebSocket — другому клієнту «зайнято»
void ws_on_event(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    if (1 < server->count()) {
      client->text("{\"ws_busy\":\"1\"}");
      client->close(0, nullptr);
    } else {
      g_bed08 = 1;
    }
  }
}
// @400e616c
void ws_state_tick_5s(uint32_t ms) {
  uint32_t t = g_c15c8;
  if (ms <= millis() - t) { g_c15c8 = millis(); ws_state_json_build(); }
}
// @400e4f60: стан годинника для веб-інтерфейсу
void ws_state_json_build() {
  String rtcs("");
  if (g_c3468 != 0) {
    DateTime rt = g_c346c.now();
    String t; time_hm_text(t, 3);
    uint8_t ss = rt.second(), dd = rt.day(), mm = rt.month(), yo = rt.year() - 2000;
    rtcs = t + ":" + String(ss / 10) + String((uint32_t)ss % 10) + " - " + two(dd) + "." + two(mm) + "." + String(yo + 2000);
  }
  String ntpn;
  if (g_c2eb4 == 0) ntpn = "No data";
  else {
    String t; time_hm_text(t, 2);
    ntpn = t + ":" + String(g_c3460 / 10) + String((uint32_t)g_c3460 % 10) + " - " + two(g_c345d) + "." + two(g_c345c) + "." + String(g_c3458);
  }
  String heap = String("free=") + String(ESP.getFreeHeap(), 10) + String(" maxBlock=") + String(ESP.getMaxAllocHeap(), 10) + String(" total=") + String(ESP.getHeapSize(), 10);
  uint8_t withHist = g_bed08;
  String o("");
  o.reserve(withHist ? 0x44c : 0x168);
  o = "{\"rtc_detect\":\"";
  o += g_c3468 ? "1" : "0";
  o += "\",\"lsynRtcSys\":\"";
  o += g_c3468 ? g_c3408 : String("-");
  o += "\",\"lsynRtcNet\":\"";
  o += g_c3468 ? g_c3414 : String("-");
  o += "\",\"lsynNtpSys\":\"";
  o += g_c33fc;
  o += "\",\"lsynNtpNet\":\"";
  o += ntpn;
  o += "\",\"sys_time\":\"";
  o += String((long)now(), 10);
  o += "\",\"rtc_time\":\"";
  o += g_c3468 ? rtcs : String("-");
  o += "\",\"heap_diag\":\"";
  o += heap;
  o += "\",\"latest_version_fw\":\"";
  o += g_c2ea8;
  o += "\",\"inet\":\"";
  o += g_c3319 == 0 ? "1" : "0";
  o += "\",\"ha_link\":\"";
  uint8_t hl = 0;
  if (g_c30c5 != 0) {
    if (g_c3090 == 1) hl = 2;
    if (g_c30c4 == 1) hl = hl | g_c30c4;
  }
  o += (char)(hl + 0x30);
  o += "\"";
  if (withHist) {
    o += ",\"hist\":";
    String h; hist_json(h);
    o += h;
  }
  o += "}";
  if (ws.availableForWriteAll()) {
    ws.textAll(o);
    if (withHist) g_bed08 = 0;
  }
}
