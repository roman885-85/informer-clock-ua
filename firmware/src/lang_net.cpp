// Мовні пакети: пакети табло (.led) з сервера автора й через веб, пакети веб-інтерфейсу (/lang/*.json.gz).
// Перенесено з 2.7.1 (адреси оригіналу в коментарях).
#include "fw.h"

// @400d5d90: шлях пакета веб-інтерфейсу
void lang_web_path(String &out, const String &code) {
  out = String("/lang") + "/" + code + ".json.gz";
}
// @400fdc5c: пакет табло — щонайменше 10 байтів і починається з "LED1"
bool led_pack_valid(const String &path) {
  File f = SPIFFS.open(path, "r");
  bool ok = false;
  if (f) {
    uint8_t m[4];
    if (f.size() < 10 || f.read(m, 4) != 4) ok = false;
    else ok = memcmp(m, "LED1", 4) == 0;
    f.close();
  }
  return ok;
}
// @40120e50: завантажити пакет табло з goldart.ee у /lang/<n>.led (через .tmp)
int32_t led_lang_download(uint8_t lang) {
  if (lang == 1) return 0;
  WiFiClient c;
  HTTPClient h;
  c.setTimeout(5000);
  h.setTimeout(5000);
  h.useHTTP10(true);
  String url = String("http://goldart.ee/api/lang/") + String((int)lang) + ".led";
  if (!h.begin(c, url)) { g_c19b4 = -1; return 0; }
  g_c19b4 = h.GET();
  if (g_c19b4 != 200) { h.end(); return 0; }
  SPIFFS.mkdir("/lang");
  String p; led_pack_path(p, lang);
  String tmp = p + ".tmp";
  String dst; led_pack_path(dst, lang);
  File f = SPIFFS.open(tmp, "w");
  if (!f) { h.end(); return 0; }
  h.writeToStream(&f);
  f.close();
  h.end();
  if (!led_pack_valid(tmp)) {
    SPIFFS.remove(tmp);
    g_c19b4 = -2;
    return 0;
  }
  SPIFFS.remove(dst);
  SPIFFS.rename(tmp, dst);
  g_bf040 = 0xff;
  return 1;
}
// @40121230: якщо пакета табло для мови немає — завантажити (не частіше ніж раз на хвилину)
void led_lang_fetch(uint32_t lang) {
  lang &= 0xff;
  if (lang == 1) return;
  String p; led_pack_path(p, lang);
  if (SPIFFS.exists(p)) return;
  if (g_c19b8 != 0 && millis() - g_c19bc < 60000) return;
  g_c19bc = millis();
  g_c19b8 = 1;
  if (led_lang_download(lang) == 0) {
    if ((int32_t)lang != g_bec00) {
      g_bec00 = lang;
      evlog_write(String("[MLED] fetch FAILED lang=") + String((int)lang) + " http=" + String((int)g_c19b4));
    }
  } else {
    dbg_ws_print(String("MATRIX LED: pack fetched from server"));
    evlog_write(String("[MLED] fetched lang=") + String((int)lang));
    g_bec00 = -1;
  }
}
// @400ff508: /lang/installed.json — прибрати запис мови; якщо add — додати {code,id,rev}
void lang_installed_json(const String &code, int32_t id, const String &rev, int8_t add) {
  JsonDocument doc;
  File f = SPIFFS.open("/lang/installed.json", "r");
  if (f) {
    deserializeJson(doc, f);
    f.close();
  }
  if (!doc.is<JsonArray>()) doc.to<JsonArray>();
  JsonArray a = doc.as<JsonArray>();
  for (int i = (int)a.size() - 1; -1 < i; i--) {
    if (a[i]["code"].as<String>().equals(code)) a.remove(i);
  }
  if (add != 0) {
    JsonObject o = a.add<JsonObject>();
    o["code"] = code;
    o["id"] = id;
    o["rev"] = rev;
  }
  f = SPIFFS.open("/lang/installed.json", "w");
  if (f) {
    serializeJson(doc, f);
    f.close();
  }
  lang_installed_load();
}
// @400ff86c: обробка завантажень/видалень мовних пакетів, підготованих веб-обробниками
void led_lang_upload_process() {
  String p;
  File f;
  // пакет веб-інтерфейсу одним файлом
  if (g_c2d78 != 0) {
    g_c2d78 = 0;
    if (g_c2da0 == 0 && lang_code_ok(g_c2dbc)) {
      if (g_c2dac == nullptr) goto c1;
      if (g_c2da8 == 0) goto c2;
      SPIFFS.mkdir("/lang");
      lang_web_path(p, g_c2dbc);
      f = SPIFFS.open(p, "w");
      if (f) {
        f.write((uint8_t *)g_c2dac, g_c2da8);
        f.close();
        lang_installed_json(g_c2dbc, g_bf020, g_c2db0, 1);
      }
    }
    if (g_c2dac != nullptr) {
c2:
      free(g_c2dac);
      g_c2dac = nullptr;
    }
c1:
    g_c2da8 = 0;
    g_c2da4 = 0;
  }
  // пакет веб-інтерфейсу частинами
  if (g_c2d7a != 0) {
    g_c2d7a = 0;
    if (lang_code_ok(g_c2d94) && g_c2d80 != nullptr && -1 < g_bf018) {
      SPIFFS.mkdir("/lang");
      lang_web_path(p, g_c2d94);
      f = SPIFFS.open(p, g_bf018 == 0 ? "w" : "a");
      if (f) {
        f.write((uint8_t *)g_c2d80, g_c2d7c);
        f.close();
        if (g_c2d84 != 0) lang_installed_json(g_c2d94, g_bf01c, g_c2d88, 1);
      }
    }
    if (g_c2d80 != nullptr) {
      free(g_c2d80);
      g_c2d80 = nullptr;
    }
    g_c2d7c = 0;
    g_c2d79 = 0;
  }
  // пакет табло (LED1)
  if (g_c2d68 != 0) {
    g_c2d68 = 0;
    if (g_c2d69 == 0 && -1 < (int32_t)g_bf014 && g_c2d74 != nullptr && 9 < g_c2d70 && memcmp(g_c2d74, "LED1", 4) == 0) {
      SPIFFS.mkdir("/lang");
      led_pack_path(p, g_bf014 & 0xff);
      f = SPIFFS.open(p, "w");
      if (f) {
        f.write((uint8_t *)g_c2d74, g_c2d70);
        f.close();
        g_bf040 = 0xff;
        evlog_write(String("[MLED] uploaded lang=") + String((int)g_bf014) + " size=" + String((int)g_c2d70));
      }
    } else if (-1 < (int32_t)g_bf014) {
      evlog_write(String("[MLED] upload REJECTED lang=") + String((int)g_bf014) + " size=" + String((int)g_c2d70) + " (not a LED1 blob)");
    }
    if (g_c2d74 != nullptr) {
      free(g_c2d74);
      g_c2d74 = nullptr;
    }
    g_c2d70 = 0;
    g_c2d6c = 0;
  }
  // видалення мови
  if (g_c2d58 != 0) {
    g_c2d58 = 0;
    if (lang_code_ok(g_c2d5c)) {
      lang_web_path(p, g_c2d5c);
      SPIFFS.remove(p);
      p = "";
      lang_installed_json(g_c2d5c, -1, p, 0);
    }
    if (-1 < (int32_t)g_bf010) {
      led_pack_path(p, g_bf010 & 0xff);
      SPIFFS.remove(p);
      if (g_bf040 == g_bf010) g_bf040 = 0xff;
    }
    g_bf010 = -1;
  }
  // підготувати пакет веб-інтерфейсу до видачі (у пам'ять)
  if (g_c2d48 != 0 && g_c2d31 == 0 && g_c2d34.length() == 0) {
    g_c2d48 = 0;
    if (lang_code_ok(g_c2d4c)) {
      lang_web_path(p, g_c2d4c);
      f = SPIFFS.open(p, "r");
      if (f) {
        size_t n = f.size();
        if (n - 1 < 0x8000) {
          if (g_c2d44 != nullptr) free(g_c2d44);
          g_c2d44 = malloc(n);
          if (g_c2d44 != nullptr) {
            f.read((uint8_t *)g_c2d44, n);
            g_c2d40 = n;
            g_c2d34 = g_c2d4c;
            g_c2d30 = 0;
            g_c2d2c = millis();
          }
        }
        f.close();
      }
    }
  }
  if (g_c2d44 != nullptr && (g_c2d30 != 0 || (g_c2d34.length() != 0 && 20000 < millis() - g_c2d2c))) {
    free(g_c2d44);
    g_c2d44 = nullptr;
    g_c2d40 = 0;
    p = "";
    g_c2d34 = p;
    g_c2d31 = 0;
    g_c2d30 = 0;
  }
}
