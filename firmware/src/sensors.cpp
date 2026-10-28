// Датчики: DS18B20, DHT11/22, BME280/BMP280/BMP180, AM2320/HDC1080/AHT10, SGP30/CCS811/MH-Z19B/SCD40/SCD41.
// Кожен датчик читається за своїм таймером; значення йдуть у веб-інтерфейс {"ключ":"…"}.
// Перенесено з 2.7.1 (адреси оригіналу в коментарях).
#include "fw.h"

// надіслати JSON у веб-інтерфейс (автор — через printfAll)
static void ws_send_doc(JsonDocument &doc) {
  String s("");
  serializeJson(doc, s);
  ws.printfAll(String(s).c_str());
}
// @400ec79c: DS18B20 (раз на 20 с; поправка g_c32d8)
void sensor_ds18b20() {
  if (millis() - g_c189c > 19999) {
    if (g_c1898 == 0) {
      dbg_ws_sep(1);
      g_c2280.begin();
      g_c1898 = 1;
      g_c2280.setResolution(g_c2275, 9);
    }
    g_c189c = millis();
    JsonDocument doc;
    g_c2280.requestTemperatures();
    g_c3300 = g_c2280.getTempCByIndex(0);
    g_c3300 = g_c32d8 + g_c3300;
    doc["tds"] = String(g_c3300, 2);
    ws_send_doc(doc);
    dbg_ws_sep(2);
  }
}
// @400ec914: BME280 / BMP280 / BMP180 (раз на 27 с; поправка g_c32d4; тиск у мм рт. ст.)
void sensor_bosch() {
  if (millis() - g_c1784 < 27000) return;
  if (g_c1780 == 0) {
    if (g_c2ee5 == 1) g_c1bd8.begin(0x76, 0x58);
    else if (g_c2ee5 == 0) g_c1c18.begin(0x76, &Wire);
    else if (g_c2ee5 == 2) g_c1bbc.begin(0x77, &Wire);
    g_c1780 = 1;
  }
  dbg_ws_sep(1);
  g_c1784 = millis();
  JsonDocument doc;
  if (g_c2ee5 == 1) {
    g_c3314 = g_c1bd8.readTemperature();
    g_c3314 = g_c32d4 + g_c3314;
    g_c330c = g_c1bd8.readPressure() / 133.3;
  } else if (g_c2ee5 == 0) {
    g_c3314 = g_c1c18.readTemperature();
    g_c3314 = g_c32d4 + g_c3314;
    g_c3310 = g_c1c18.readHumidity();
    g_c330c = g_c1c18.readPressure() / 133.3;
    doc["bme280_h"] = String(g_c3310, 2);
  } else if (g_c2ee5 == 2) {
    g_c3314 = g_c1bbc.readTemperature();
    g_c3314 = g_c32d4 + g_c3314;
    g_c330c = g_c1bbc.readPressure() / 133.3;
  }
  doc["bme280_t"] = String(g_c3314, 2);
  doc["bme280_pr"] = String(g_c330c, 2);
  ws_send_doc(doc);
  dbg_ws_print(String("Bosch      : reading sensor  t=") + String(g_c3314, 2) + String("  h=") + String(g_c3310, 2) + String("  p=") + String(g_c330c, 2));
}
// @400ed5dc: DHT11/DHT22 на GPIO14 (перший раз одразу, далі раз на 30 с; поправка g_c32c8)
void sensor_dht() {
  if (millis() - g_c1750 < g_c174c) return;
  g_c1750 = millis();
  JsonDocument doc;
  if (g_c1749 == 0) {
    if (g_c32e6 == 11) g_c19e8.setup(14, DHTesp::DHT11);
    else if (g_c32e6 == 22) g_c19e8.setup(14, DHTesp::DHT22);
    g_c1749 = 1;
  }
  g_c3304 = g_c19e8.getHumidity();
  g_c3308 = g_c19e8.getTemperature();
  g_c3308 = g_c32c8 + g_c3308;
  doc["dht_hum"] = String(g_c3304, 2);
  doc["dht_t"] = String(g_c3308, 2);
  ws_send_doc(doc);
  dbg_ws_sep(1);
  dbg_ws_print(String("DHT      : reading sensor") + String("  t=") + String(g_c3308, 2) + String("  hum=") + String(g_c3304, 2));
  g_c174c = 30000;
}
// @400ed228: AM2320 / HDC1080 / AHT10 (раз на 26 с; поправка g_c32dc)
void sensor_other() {
  if (millis() - g_c1770 > 25999) {
    if (g_c176d != g_c32e3) {
      g_c176d = g_c32e3;
      g_c176c = 0;
      g_c32fc = 0.0;
      g_c32f8 = 0;
    }
    if (g_c176c == 0) {
      if (g_c32e3 == 1) g_c1b00.begin(0x40, &Wire);
      else if (g_c32e3 == 0) g_c1b0c.begin();
      else if (g_c32e3 == 2) g_c1af8.begin();
      g_c176c = 1;
    }
    g_c1770 = millis();
    JsonDocument doc;
    if (g_c32e3 == 1) {
      g_c32fc = g_c1b00.readTemperature();
      g_c32fc = g_c32dc + g_c32fc;
      g_c32f8 = g_c1b00.readHumidity();
    } else if (g_c32e3 == 0) {
      g_c32fc = g_c1b0c.readTemperature();
      g_c32fc = g_c32dc + g_c32fc;
      g_c32f8 = g_c1b0c.readHumidity();
    } else if (g_c32e3 == 2) {
      g_c32fc = g_c1af8.readTemperature(AHT10_FORCE_READ_DATA);
      g_c32fc = g_c32dc + g_c32fc;
      g_c32f8 = g_c1af8.readHumidity(AHT10_USE_READ_DATA);
    }
    doc["SeS_t"] = String(g_c32fc, 2);
    doc["SeS_h"] = String(g_c32f8, 2);
    ws_send_doc(doc);
    dbg_ws_sep(1);
    dbg_ws_print(String("Other sensors      : reading sensor,") + String("  t=") + String(g_c32fc, 2) + String("  h=") + String(g_c32f8, 2));
  }
}
// @400ecdb0: SGP30 / CCS811 / MH-Z19B / SCD40 / SCD41 (перший раз одразу, далі раз на 25 с)
void sensor_air() {
  if (millis() - g_c177c < g_c1778) return;
  if (g_c1775 != g_c32e0) {
    g_c1775 = g_c32e0;
    g_c1774 = 0;
    g_c1778 = 0;
    g_c32f4 = 0;
    g_c32f0 = 0;
  }
  if (g_c1774 == 0) {
    switch (g_c32e0) {
      case 0: g_c1ba8.begin(&Wire, true); break;
      case 1: g_c1b50.begin(Wire); break;
      case 2: g_c1b6c.begin(15, 4, 1); g_c1b6c.setAutoCalibration(false); break;
      case 3:
      case 4: g_c1b30.begin(Wire, true, true, false); break;
    }
    g_c1774 = 1;
  }
  g_c177c = millis();
  dbg_ws_sep(1);
  JsonDocument doc;
  switch (g_c32e0) {
    case 0:
      g_c1ba4 = g_c1ba8.IAQmeasure();
      g_c32f4 = g_c1ba8.TVOC;
      g_c32f0 = g_c1ba8.eCO2;
      doc["TVOCs"] = String(g_c32f4, 10);
      break;
    case 1:
      if (g_c1b50.dataAvailable()) {
        g_c1b50.readAlgorithmResults();
        g_c32f4 = g_c1b50.getTVOC();
        g_c32f0 = g_c1b50.getCO2();
        doc["TVOCs"] = String(g_c32f4, 10);
      }
      break;
    case 2:
      g_c32f0 = g_c1b6c.getPPM();
      break;
    case 3:
    case 4:
      if (g_c1b30.readMeasurement()) g_c32f0 = g_c1b30.getCO2();
      break;
  }
  doc["eCO2s"] = String(g_c32f0, 10);
  ws_send_doc(doc);
  dbg_ws_print(String("Air quality sensors: reading sensor,") + String("  TVOCs=") + String(g_c32f4, 10) + String("  eCO2s=") + String(g_c32f0, 10));
  dbg_ws_sep(1);
  g_c1778 = 25000;
}
// @400eea30
void sensors_read_all() {
  if (g_c32e8 != 0) sensor_ds18b20();
  if (g_c32e7 != 0) sensor_dht();
  if (g_c32e5 != 0) sensor_bosch();
  if (g_c32e2 != 0) sensor_other();
  if (g_c32e1 != 0) sensor_air();
  night_mode_tick(10000);
}
