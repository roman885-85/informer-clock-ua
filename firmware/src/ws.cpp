// WebSocket /ws: налагоджувальна консоль, стан кожні 5 с, «зайнято» для другого клієнта (перенесено з 2.7.1)
// EN: WebSocket /ws: debug console, state every 5 s, "busy" for a second client (from 2.7.1; Wi-Fi passwords masked;
// outgoing messages go through a queue drained in the AsyncTCP task).
#include "fw.h"

// Виправлено (падіння 28.10.2026 о 22:30, «CORRUPT HEAP» у lwIP mem_free): автор надсилає у веб-інтерфейс просто з
// головного циклу (ws.textAll/printfAll: налагодження, стан, погода, курси, датчики, MQTT) і щоциклу кличе
// ws.cleanupClients(2), а підключення, відключення й підтвердження клієнтів обробляє потік AsyncTCP. Список клієнтів
// і їхні черги бібліотека не захищає: коли клієнт підключається чи відключається (оновили сторінку, другий клієнт
// отримав «зайнято»), головний цикл може дописати в чергу клієнта, якого саме видаляють, — пам'ять псується, і годинник
// падає пізніше, на звільненні чужого блоку. Тепер головний цикл лише кладе рядки в цю чергу (ws_send), а надсилає їх і
// прибирає зайвих клієнтів потік AsyncTCP (ws_async_hook — гачок у lib/ESPAsyncWebServer-esphome, FORK.md).
#define WS_BOX 16384                  // кільце черги, Б
#define WS_MSG_MAX 6144               // найдовше повідомлення (стан з історією ≈ 1 КБ, погода ≈ 0,5 КБ)
static portMUX_TYPE s_box_mux = portMUX_INITIALIZER_UNLOCKED;
static uint8_t s_box[WS_BOX];
static uint32_t s_rd, s_wr;           // лічильники байтів; зайнято = s_wr - s_rd
static uint32_t s_drop;               // відкинуто: черга повна
volatile uint8_t g_fx_ws_n;           // клієнтів WebSocket (оновлює потік AsyncTCP; головному циклу — лише прочитати)

static void box_put(const uint8_t *p, uint32_t n) {   // під s_box_mux
  for (uint32_t done = 0; done < n;) {
    uint32_t at = (s_wr + done) % WS_BOX, k = n - done;
    if (k > WS_BOX - at) k = WS_BOX - at;
    memcpy(s_box + at, p + done, k);
    done += k;
  }
  s_wr += n;
}
static void box_get(uint8_t *p, uint32_t n) {         // під s_box_mux
  for (uint32_t done = 0; done < n;) {
    uint32_t at = (s_rd + done) % WS_BOX, k = n - done;
    if (k > WS_BOX - at) k = WS_BOX - at;
    memcpy(p + done, s_box + at, k);
    done += k;
  }
  s_rd += n;
}
// покласти повідомлення в чергу (з будь-якого потоку); false — нікого немає або черга повна
bool ws_send(const char *p, size_t n) {
  if (n == 0 || n > WS_MSG_MAX || g_fx_ws_n == 0) return false;
  uint8_t len[2] = { (uint8_t)(n & 0xff), (uint8_t)(n >> 8) };
  bool ok = false;
  portENTER_CRITICAL(&s_box_mux);
  if (WS_BOX - (s_wr - s_rd) >= n + 2) { box_put(len, 2); box_put((const uint8_t *)p, n); ok = true; }
  else s_drop++;
  portEXIT_CRITICAL(&s_box_mux);
  return ok;
}
bool ws_send(const String &s) { return ws_send(s.c_str(), s.length()); }
// є місце ще на n байтів (замість ws.availableForWriteAll() у головному циклі)
bool ws_send_room(size_t n) { return g_fx_ws_n != 0 && WS_BOX - (s_wr - s_rd) >= n + 2; }

// потік AsyncTCP: прибрати зайвих клієнтів і надіслати чергу (опитування клієнта ≈ раз на 0,5 с і після підтверджень)
void ws_async_hook(AsyncWebSocket *server) {
  static uint8_t buf[WS_MSG_MAX];
  static bool busy;
  if (busy) return;
  busy = true;
  server->cleanupClients(2);          // у автора — щоциклу з головного циклу
  g_fx_ws_n = server->count();
  // кожне надсилання тут чекає потік lwIP, а той — місця в черзі цього потоку: коли в ній багато подій, спершу
  // нехай їх розбере (30.10.2026: сторож задач під навантаженням); за раз — не більше 8 повідомлень
  if (async_tcp_queue_waiting() > 16) { busy = false; return; }
  for (int k = 0; k < 8 && server->availableForWriteAll(); k++) {
    uint32_t n = 0;
    portENTER_CRITICAL(&s_box_mux);
    if (s_wr - s_rd >= 2) {
      uint8_t len[2];
      box_get(len, 2);
      n = len[0] | ((uint32_t)len[1] << 8);
      box_get(buf, n);
    }
    portEXIT_CRITICAL(&s_box_mux);
    if (n == 0) break;
    server->textAll((const char *)buf, n);
  }
  busy = false;
}

static String two(uint8_t v) { return v < 10 ? String("0") + String(v, 10) : String(v, 10); }

// @400d6878: рядок налагодження {"dbg":"…"} усім клієнтам
// Виправлено: автор друкує в живу консоль вміст /networks.json — разом із паролями Wi-Fi («Saved Networks Config File:»,
// «Config networks loaded:»). Значення "password" тепер приховано (і в консолі веб-інтерфейсу, і в сетевому журналі).
static String mask_passwords(const String &s) {
  int i = s.indexOf("\"password\":\"");
  if (i < 0) return s;
  String o("");
  int from = 0;
  while (i >= 0) {
    int v = i + 12, n = s.length(), e = v;
    while (e < n && (s[e] != '"' || s[e - 1] == '\\')) e++;   // кінець значення — лапка без зворотної риски
    if (e >= n) break;
    o += s.substring(from, v);
    o += "***";
    from = e;
    i = s.indexOf("\"password\":\"", e);
  }
  o += s.substring(from);
  return o;
}
void dbg_ws_print(const String &s0) {
  const String &s = s0.indexOf("\"password\"") < 0 ? s0 : mask_passwords(s0);
  fx_wdbg_line(s.c_str(), s.length());   // нове: сетевий журнал (wdbg.cpp), якщо ввімкнено
#ifdef COPY_DIAG
  Serial.println(s);   // лише діагностична збірка (env:diag), не частина копії
#endif
  if (g_fx_ws_n != 0 && ws_send_room(s.length() + 0x20)) {
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
    ws_send(o);
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
  g_fx_ws_n = server->count();        // нове: головний цикл дивиться сюди, а не в список клієнтів
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
  if (ws_send(o) && withHist) g_bed08 = 0;
}
