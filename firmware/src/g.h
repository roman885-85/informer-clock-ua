// Згенеровано автоматично під час реконструкції прошивки — не редагувати вручну
#pragma once
#include "libs.h"

extern int32_t g_bebe8;  // OTA: останній відсоток (ширина з доступів)
extern int8_t g_bebec;  // зсув циферблата (одна/дві цифри години)
extern int32_t g_bebf0;  // ширина з доступів у коді (widths.py)
extern int32_t g_bebf4;  // ширина з доступів у коді (widths.py)
extern int32_t g_bebf8;  // ширина з доступів у коді (widths.py)
extern uint8_t g_bebfc;
extern uint8_t g_bebfd;
extern uint8_t g_bebfe;
extern uint8_t g_bebff;
extern uint8_t g_bec00;
extern uint8_t g_bec04;
extern uint16_t g_bec06;  // ширина з доступів у коді (widths.py)
extern uint32_t g_bec08;  // ширина з доступів у коді (widths.py)
extern uint8_t g_bec0c;
extern uint8_t g_bec0d;
extern uint8_t g_bec0e;
extern uint8_t g_bec0f;
extern uint8_t g_bec10;
extern uint8_t g_bec11;
extern uint8_t g_bec12;
extern uint8_t g_bec13;
extern uint8_t g_bec14;
extern uint8_t g_bec15;
extern uint8_t g_bec16;
extern uint8_t g_bec17;
extern uint8_t g_bec18;
extern uint8_t g_bec19;
extern uint8_t g_bec1a;
extern uint8_t g_bec1b;
extern float g_bec1c;  // м'ячики: початкова швидкість (√20)
extern uint32_t g_bec20;
extern uint32_t g_bec24;  // ширина з доступів у коді (widths.py)
extern uint8_t g_bec28;
extern uint8_t g_bed08;
extern int32_t g_bed0c;  // знаковий байт (вивід); ширина з доступів у коді (widths.py)
extern int32_t g_bed10;  // знаковий байт (вивід); ширина з доступів у коді (widths.py)
extern int32_t g_bed14;  // ширина з доступів у коді (widths.py)
extern int32_t g_bed18;  // ширина з доступів у коді (widths.py)
extern uint8_t g_bed1c;
extern int32_t g_bf010;  // ширина з доступів у коді (widths.py)
extern int32_t g_bf014;  // ширина з доступів у коді (widths.py)
extern int32_t g_bf018;  // знаковий байт (вивід); ширина з доступів у коді (widths.py)
extern int32_t g_bf01c;  // ширина з доступів у коді (widths.py)
extern int32_t g_bf020;  // ширина з доступів у коді (widths.py)
extern uint8_t g_bf040;
extern uint8_t g_bf041;
extern uint8_t g_bf042;  // стрічка: насиченість (байт; не float — адреса не вирівняна на 4)
extern uint8_t g_bf043;
extern float g_bf044;  // ширина з доступів у коді (widths.py)
extern float g_bf048;  // ширина з доступів у коді (widths.py)
extern uint8_t g_bf04c;
extern uint8_t g_bf04d;
extern uint32_t g_c15c8;
extern uint32_t g_c15cc;
extern uint32_t g_c15d0;
extern uint8_t g_c15d4;
extern uint32_t g_c15d8;
extern uint32_t g_c15dc;
extern uint8_t g_c15e0;
extern uint8_t g_c15e1;
extern uint32_t g_c15e4;
extern uint32_t g_c15e8;
extern uint8_t g_c15ec;
extern uint8_t g_c15ed;
extern uint32_t g_c1620;
extern uint32_t g_c1624;
extern char g_c1628[256];  // буфер «живого тексту» (коди табло)
extern uint8_t g_c1728;
extern uint32_t g_c172c;
extern uint32_t g_c1730;
extern uint32_t g_c1734;
extern uint8_t g_c1738;
extern uint8_t g_c1739;
extern uint32_t g_c173c;
extern uint8_t g_c1740;
extern uint32_t g_c1744;
extern uint8_t g_c1748;
extern uint8_t g_c1749;
extern uint32_t g_c174c;
extern uint32_t g_c1750;
extern uint32_t g_c1754;
extern uint32_t g_c1758;
extern uint32_t g_c175c;
extern uint8_t g_c1760;
extern uint32_t g_c1764;
extern uint32_t g_c1768;
extern uint8_t g_c176c;
extern uint8_t g_c176d;
extern uint32_t g_c1770;
extern uint8_t g_c1774;
extern uint8_t g_c1775;
extern uint32_t g_c1778;
extern uint32_t g_c177c;
extern uint8_t g_c1780;
extern uint32_t g_c1784;
extern uint32_t g_c1788;
extern uint32_t g_c178c;
extern uint32_t g_c1790;
extern uint32_t g_c1794;
extern uint8_t g_c1798;
extern uint32_t g_c1894;
extern uint8_t g_c1898;
extern uint32_t g_c189c;
extern uint32_t g_c18a0;
extern uint32_t g_c18a4;
extern uint32_t g_c18a8;
extern uint8_t g_c18ac;
extern uint8_t g_c18ad;
extern uint8_t g_c18ae;
extern uint8_t g_c18af;
extern uint8_t g_c18b0;
extern uint32_t g_c18b4;
extern uint32_t g_c18b8;
extern uint32_t g_c18bc;
extern uint32_t g_c18c0;
extern uint8_t g_c18c4;
extern uint32_t g_c18e0;
extern uint32_t g_c18e4;
extern uint8_t g_c18e8;
extern uint8_t g_c18e9;
extern uint32_t g_c18ec;
extern uint32_t g_c18f0;
extern uint8_t g_c18f4[32];  // цифри годин (2×16 байт) годинника зі зсувом
extern uint8_t g_c1914;
extern uint8_t g_c1915;
extern uint8_t g_c1918[32];  // цифри хвилин (2×16)
extern uint8_t g_c1938[32];  // цифри секунд (2×16)
extern uint8_t g_c1958;
extern uint8_t g_c19b0;
extern uint32_t g_c19b4;
extern uint8_t g_c19b8;
extern uint32_t g_c19bc;
extern uint32_t g_c19c0;
extern uint8_t g_c19c4;
extern uint8_t g_c19c5;
extern uint8_t g_c19c6;
extern uint8_t g_c19c7;
extern uint8_t g_c19c8;
extern uint32_t g_c19cc;
extern uint32_t g_c19d0;
extern uint32_t g_c19d4;
extern uint8_t g_c19d8;
extern uint8_t g_c19d9;
extern uint8_t g_c19da;
extern uint8_t g_c19db;
extern uint8_t g_c19dc;
extern uint32_t g_c19e0;
extern uint32_t g_c19e4;
extern DHTesp g_c19e8;  // DHT11/DHT22 (DHTesp), GPIO14
extern uint8_t g_c1a28;
extern uint32_t g_c1a2c;
extern uint32_t g_c1a30;
extern String g_c1a34;  // рядок (конструктор)
extern uint32_t g_c1a40;
extern uint32_t g_c1a44;
extern String g_c1a48;  // рядок (конструктор)
extern String g_c1a54;  // рядок (конструктор)
extern uint32_t g_c1a64;
extern uint32_t g_c1a68;
extern String g_c1a6c;  // рядок (конструктор)
extern uint32_t g_c1a78;
extern uint32_t g_c1a7c;
extern uint32_t g_c1a80;
extern uint32_t g_c1a84;
extern uint8_t g_c1a88;
extern uint8_t g_c1a89;
extern uint8_t g_c1a8a;
extern uint8_t g_c1a8b;
extern uint8_t g_c1a8c;
extern uint8_t g_c1a8d;
extern uint8_t g_c1a8e;
extern uint32_t g_c1a90;
extern uint8_t g_c1a94;
extern uint8_t g_c1a95;
extern uint8_t g_c1a96;
extern uint8_t g_c1a97;
extern uint8_t g_c1a98;
extern uint8_t g_c1a99;
extern uint8_t g_c1a9a;
extern uint32_t g_c1a9c;
extern uint32_t g_c1aa0;
extern uint32_t g_c1aa4;
extern uint32_t g_c1aa8;
extern uint32_t g_c1aac;
extern uint8_t g_c1ab0;
extern uint32_t g_c1ab4;
extern uint32_t g_c1ab8;
extern uint32_t g_c1abc;
extern uint32_t g_c1ac0;
extern String g_c1ac4;  // рядок (конструктор)
extern uint32_t g_c1ad0;
extern uint32_t g_c1ad4;
extern String g_c1ad8;  // рядок (конструктор)
extern uint32_t g_c1ae4;
extern uint32_t g_c1ae8;
extern String g_c1aec;  // рядок (конструктор)
extern AHT10 g_c1af8;  // AHT10
extern Adafruit_HDC1000 g_c1b00;  // HDC1080 (бібліотека Adafruit HDC1000)
extern Adafruit_AM2320 g_c1b0c;  // AM2320
extern SCD4x g_c1b30;  // SCD40/SCD41
extern CCS811 g_c1b50;  // CCS811 (SparkFun)
extern MHZ19_uart g_c1b6c;  // MH-Z19B (MHZ19_uart, UART1, RX15/TX4)
extern uint8_t g_c1ba4;
extern Adafruit_SGP30 g_c1ba8;  // SGP30
extern Adafruit_BMP085 g_c1bbc;  // BMP180 (Adafruit BMP085)
extern Adafruit_BMP280 g_c1bd8;  // BMP280
extern Adafruit_BME280 g_c1c18;  // BME280
extern uint8_t g_c1c6c;
extern uint8_t g_c1c6d;
extern uint32_t g_c1c70;
extern uint32_t g_c1c74;
extern uint32_t g_c1c78;
extern uint32_t g_c1c7c;
extern String g_c1c80;  // рядок (конструктор)
extern uint8_t g_c1f18;
extern uint8_t g_c1f19;
extern uint32_t g_c209c;
extern float g_c20a0[3];  // м'ячики: затухання
extern uint32_t g_c20ac[3];  // м'ячики: час останнього відскоку
extern int32_t g_c20b8[3];  // м'ячики: позиція (номер світлодіода, round(...))
extern float g_c20c4[3];  // м'ячики: час від відскоку
extern float g_c20d0[3];  // м'ячики: швидкість удару
extern float g_c20dc[3];  // м'ячики: висота
extern uint32_t g_c20e8;
extern uint32_t g_c20f4;
extern uint32_t g_c20f8;
extern uint32_t g_c20fc;
extern uint8_t g_c2100;  // стрічка: відтінок ефекту 0/1 (байт)
extern uint32_t g_c226c;
extern uint32_t g_c2270;
extern uint8_t g_c2274;
extern uint8_t g_c2275[8];  // DS18B20: адреса пристрою (не заповнюється)
extern DallasTemperature g_c2280;  // DS18B20
extern OneWire g_c22a0;  // шина 1-Wire, GPIO26
extern uint8_t g_c22b4;
extern uint8_t g_c22b5;
extern uint32_t g_c22b8;
extern uint8_t g_c22bc;
extern uint8_t g_c22bd;
extern uint8_t g_c22be;
extern uint8_t g_c22bf;
extern uint8_t g_c22c0;
extern String g_c22c4;  // рядок (конструктор)
extern String g_c22d0;  // рядок (конструктор)
extern uint32_t g_c22dc;
extern uint32_t g_c22e0;
extern uint32_t g_c22e4;
extern uint8_t g_c22e8;
extern String g_c22ec[8];  // масив рядків (конструктор)
extern String g_c234c[8];  // масив рядків (конструктор)
extern String g_c23ac[8];  // масив рядків (конструктор)
extern uint8_t g_c240c;
extern String g_c2410;  // рядок (конструктор)
extern uint8_t g_c241c;
extern String g_c2420;  // рядок (конструктор)
extern uint8_t g_c242c;
extern String g_c2430;  // рядок (конструктор)
extern uint8_t g_c243c;
extern String g_c2440;  // рядок (конструктор)
extern uint32_t g_c244c;
extern uint8_t g_c2464[12];  // MQTT: підписано на топік i
extern String g_c2470[12];  // масив рядків (конструктор)
extern String g_c2500[12];  // масив рядків (конструктор)
extern uint8_t g_c2590;
extern String g_c2594[4];  // MQTT-тексти 1–4: префікс (txMQ…txMQ4)
extern String g_c25c4[4];  // MQTT-тексти 1–4: топік (m_text_1…4)
extern String g_c25f4;  // рядок (конструктор)
extern String g_c2600;  // рядок (конструктор)
extern String g_c260c;  // рядок (конструктор)
extern uint32_t g_c2618;
extern String g_c261c;  // рядок (конструктор)
extern AsyncMqttClient g_c2628;  // MQTT-клієнт
extern uint8_t g_c27c8;
extern uint8_t g_c27e0;
extern uint8_t g_c27e1;
extern uint8_t g_c27e2;
extern uint8_t g_c27e3;
extern uint8_t g_c27e4;
extern uint8_t g_c27e5;
extern uint32_t g_c2bfc;
extern uint8_t g_c2c00;
extern uint32_t g_c2c04;
extern uint32_t g_c2c08;
extern uint8_t g_c2c0c;
extern uint8_t g_c2c0d;
extern uint8_t g_c2c0e;
extern uint32_t g_c2c10;
extern uint16_t g_c2c14;  // ширина з доступів у коді (widths.py)
extern uint8_t g_c2c16;
extern uint8_t g_c2c17;
extern String g_c2c18;  // рядок (конструктор)
extern uint8_t g_c2c24;
extern String g_c2c28;  // рядок (конструктор)
extern uint8_t g_c2c34;
extern uint32_t g_c2c38;
extern File g_c2c3c;  // файл завантаження конфігурації
extern uint8_t g_c2d28;
extern uint8_t g_c2d29;
extern uint32_t g_c2d2c;
extern uint8_t g_c2d30;
extern uint8_t g_c2d31;
extern String g_c2d34;  // рядок (конструктор)
extern uint32_t g_c2d40;
extern void * g_c2d44;  // пакет мови (gz) у пам'яті
extern uint8_t g_c2d48;
extern String g_c2d4c;  // рядок (конструктор)
extern uint8_t g_c2d58;
extern String g_c2d5c;  // рядок (конструктор)
extern uint8_t g_c2d68;
extern uint8_t g_c2d69;
extern uint32_t g_c2d6c;
extern uint32_t g_c2d70;
extern void * g_c2d74;  // буфер POST /lang-led
extern uint8_t g_c2d78;
extern uint8_t g_c2d79;
extern uint8_t g_c2d7a;
extern uint8_t g_c2d7b;
extern uint32_t g_c2d7c;
extern void * g_c2d80;  // буфер шматка /lang-chunk
extern uint8_t g_c2d84;
extern String g_c2d88;  // рядок (конструктор)
extern String g_c2d94;  // рядок (конструктор)
extern uint8_t g_c2da0;
extern uint32_t g_c2da4;
extern uint32_t g_c2da8;
extern void * g_c2dac;  // буфер /lang-upload
extern String g_c2db0;  // рядок (конструктор)
extern String g_c2dbc;  // рядок (конструктор)
extern String g_c2dc8;  // рядок (конструктор)
extern String g_c2e34;  // рядок (конструктор)
extern uint32_t g_c2e40;
extern uint16_t * g_c2e44;  // таблиця зсувів фраз пакета .led
extern uint16_t g_c2e48;  // ширина з доступів у коді (widths.py)
extern uint32_t g_c2e4c;
extern uint32_t g_c2e50;
extern uint32_t g_c2e54;
extern uint32_t g_c2e58;
extern uint32_t g_c2e5c;
extern uint32_t g_c2e60;
extern uint32_t g_c2e64;
extern uint32_t g_c2e68;
extern uint32_t g_c2e6c;
extern uint8_t g_c2e70;
extern uint8_t g_c2e71;
extern uint8_t g_c2e72;
extern uint8_t g_c2e73;
extern uint32_t g_c2e74;
extern String g_c2e78;  // рядок (конструктор)
extern uint32_t g_c2e84;
extern uint8_t g_c2e88;
extern uint8_t g_c2e89;
extern uint8_t g_c2e8a;
extern uint8_t g_c2e8b;
extern uint8_t g_c2e8c;
extern uint32_t g_c2e90;
extern uint32_t g_c2e94;
extern uint8_t g_c2e98;
extern uint8_t g_c2e99;
extern String g_c2e9c;  // рядок (конструктор)
extern String g_c2ea8;  // рядок (конструктор)
extern uint8_t g_c2eb4;
extern uint32_t g_c2eb8;
extern String g_c2ebc;  // рядок (конструктор)
extern String g_c2ec8;  // рядок (конструктор)
extern uint32_t g_c2ed4;
extern uint8_t g_c2ed8;
extern uint8_t g_c2ed9;
extern uint32_t g_c2edc;
extern uint8_t g_c2ee0;
extern uint8_t g_c2ee1;
extern uint8_t g_c2ee2;
extern uint8_t g_c2ee3;
extern uint8_t g_c2ee4;
extern uint8_t g_c2ee5;
extern uint8_t g_c2ee6;
extern uint8_t g_c2ee7;
extern uint8_t g_c2ee8;
extern uint8_t g_c2ee9;
extern uint8_t g_c2eea;
extern uint8_t g_c2eeb;
extern uint8_t g_c2eec;
extern uint8_t g_c2eed;
extern uint8_t g_c2eee;
extern uint8_t g_c2eef;
extern String g_c2ef0;  // рядок (конструктор)
extern uint8_t g_c2efc;
extern String g_c2f00;  // рядок (конструктор)
extern String g_c2f0c;  // рядок (конструктор)
extern uint8_t g_c2f18;
extern uint8_t g_c2f19;
extern uint8_t g_c2f1a;
extern uint8_t g_c2f1b;
extern uint8_t g_c2f1c;
extern uint8_t g_c2f1d;
extern uint8_t g_c2f1e;  // стрічка: відтінок статичного кольору (байт)
extern uint8_t g_c2f1f;
extern uint8_t g_c2f20;
extern uint8_t g_c2f21;
extern uint8_t g_c2f22;
extern uint8_t g_c2f23;
extern uint8_t g_c2f24;
extern uint8_t g_c2f25;
extern uint8_t g_c2f26;
extern uint8_t g_c2f27;
extern uint8_t g_c2f28;
extern uint8_t g_c2f29;
extern CRGB g_c2f5a[30];  // світлодіоди стрічки (30 шт., WS2812B на GPIO27)
extern uint8_t g_c2fb4;
extern String g_c2fb8;  // рядок (конструктор)
extern uint8_t g_c2fc4;
extern uint8_t g_c2fc5;
extern uint8_t g_c2fc6;
extern uint8_t g_c2fc7;
extern uint8_t g_c2fc8;
extern uint8_t g_c2fc9;
extern String g_c2fcc;  // рядок (конструктор)
extern String g_c2fd8;  // рядок (конструктор)
extern String g_c2fe4;  // рядок (конструктор)
extern String g_c2ff0;  // рядок (конструктор)
extern uint8_t g_c2ffc;
extern uint8_t g_c2ffd;
extern uint8_t g_c2ffe;
extern uint8_t g_c2fff;
extern uint8_t g_c3000;
extern uint8_t g_c3001;
extern String g_c3004;  // рядок (конструктор)
extern String g_c3010;  // рядок (конструктор)
extern String g_c301c;  // рядок (конструктор)
extern String g_c3028;  // рядок (конструктор)
extern String g_c3034;  // рядок (конструктор)
extern String g_c3040;  // рядок (конструктор)
extern String g_c304c;  // рядок (конструктор)
extern String g_c3058;  // рядок (конструктор)
extern String g_c3064;  // рядок (конструктор)
extern String g_c3070;  // рядок (конструктор)
extern uint8_t g_c307c;
extern uint8_t g_c307d;
extern uint8_t g_c307e;
extern uint8_t g_c307f;
extern uint8_t g_c3080;
extern String g_c3084;  // рядок (конструктор)
extern uint32_t g_c3090;
extern String g_c3094;  // рядок (конструктор)
extern String g_c30a0;  // рядок (конструктор)
extern String g_c30ac;  // рядок (конструктор)
extern String g_c30b8;  // рядок (конструктор)
extern uint8_t g_c30c4;
extern uint8_t g_c30c5;
extern uint8_t g_c30c6;
extern String g_c30c8;  // рядок (конструктор)
extern String g_c30d4;  // рядок (конструктор)
extern String g_c30e0;  // рядок (конструктор)
extern String g_c30ec;  // рядок (конструктор)
extern uint8_t g_c30f8;
extern String g_c30fc;  // рядок (конструктор)
extern String g_c3108;  // рядок (конструктор)
extern String g_c3114;  // рядок (конструктор)
extern String g_c3120;  // рядок (конструктор)
extern String g_c312c;  // рядок (конструктор)
extern uint8_t g_c3138;
extern uint8_t g_c3139;
extern String g_c313c;  // рядок (конструктор)
extern String g_c3148;  // рядок (конструктор)
extern uint16_t g_c3154;  // ширина з доступів у коді (widths.py)
extern uint8_t g_c3156;
extern uint8_t g_c3157;
extern uint8_t g_c3158;
extern uint8_t g_c3159;
extern String g_c315c;  // рядок (конструктор)
extern float g_c3168;  // float (вивід)
extern float g_c316c;  // float (вивід)
extern String g_c3170[7];  // погода завтра (Weatherbit): 7 полів
extern String g_c31c4[8];  // погода зараз (Weatherbit): 8 полів
extern String g_c3224[8];  // погода зараз (OWM/Yandex): 8 полів
extern uint16_t g_c3284;  // ширина з доступів у коді (widths.py)
extern String g_c3288;  // рядок (конструктор)
extern String g_c3294;  // рядок (конструктор)
extern String g_c32a0;  // рядок (конструктор)
extern String g_c32ac;  // рядок (конструктор)
extern String g_c32b8;  // рядок (конструктор)
extern float g_c32c4;  // float (вивід)
extern float g_c32c8;  // float (вивід)
extern float g_c32cc;  // float (вивід)
extern float g_c32d0;  // float (вивід)
extern float g_c32d4;  // float (вивід)
extern float g_c32d8;  // float (вивід)
extern float g_c32dc;  // float (вивід)
extern uint8_t g_c32e0;
extern uint8_t g_c32e1;
extern uint8_t g_c32e2;
extern uint8_t g_c32e3;
extern uint8_t g_c32e4;
extern uint8_t g_c32e5;
extern uint8_t g_c32e6;
extern uint8_t g_c32e7;
extern uint8_t g_c32e8;
extern uint8_t g_c32e9;
extern uint8_t g_c32ea;
extern uint8_t g_c32eb;
extern uint8_t g_c32ec;
extern uint8_t g_c32ed;
extern uint32_t g_c32f0;  // eCO2, ppm (ціле: String(x,10), (float)x)
extern uint32_t g_c32f4;  // TVOC, ppb (ціле)
extern float g_c32f8;  // float (вивід)
extern float g_c32fc;  // float (вивід)
extern float g_c3300;  // float (вивід)
extern uint32_t g_c3304;
extern float g_c3308;  // float (вивід)
extern float g_c330c;  // float (вивід)
extern float g_c3310;  // float (вивід)
extern float g_c3314;  // float (вивід)
extern uint8_t g_c3318;
extern uint8_t g_c3319;
extern uint8_t g_c331a;
extern String g_c331c;  // рядок (конструктор)
extern String g_c3328;  // рядок (конструктор)
extern String g_c3334;  // рядок (конструктор)
extern String g_c3340;  // рядок (конструктор)
extern String g_c334c;  // рядок (конструктор)
extern uint8_t g_c3358;
extern uint8_t g_c3359;
extern uint8_t g_c335a;
extern uint8_t g_c335b;
extern String g_c335c;  // рядок (конструктор)
extern String g_c3368;  // рядок (конструктор)
extern String g_c3374;  // рядок (конструктор)
extern uint8_t g_c3380;
extern uint8_t g_c3381;
extern uint32_t g_c3384;
extern uint8_t g_c3388;
extern String g_c338c;  // рядок (конструктор)
extern String g_c3398;  // рядок (конструктор)
extern String g_c33a4;  // рядок (конструктор)
extern String g_c33b0;  // рядок (конструктор)
extern String g_c33bc;  // рядок (конструктор)
extern String g_c33c8;  // рядок (конструктор)
extern String g_c33d4;  // рядок (конструктор)
extern String g_c33e0;  // рядок (конструктор)
extern String g_c33ec;  // рядок (конструктор)
extern uint8_t g_c33f8;
extern String g_c33fc;  // рядок (конструктор)
extern String g_c3408;  // рядок (конструктор)
extern String g_c3414;  // рядок (конструктор)
extern uint32_t g_c3420;
extern uint8_t g_c3424;
extern uint32_t g_c3428;
extern uint8_t g_c342c;
extern float g_c3430;  // float (вивід)
extern String g_c3434;  // рядок (конструктор)
extern String g_c3440;  // рядок (конструктор)
extern String g_c344c;  // рядок (конструктор)
extern uint32_t g_c3458;
extern uint8_t g_c345c;
extern uint8_t g_c345d;
extern uint8_t g_c345e;
extern uint8_t g_c345f;
extern uint8_t g_c3460;
extern uint8_t g_c3461;
extern uint32_t g_c3464;
extern uint8_t g_c3468;
extern RTC_DS3231 g_c346c;  // годинник реального часу DS3231
extern uint8_t g_c3470;
extern uint8_t g_c3471;
extern uint32_t g_c3488;
extern uint32_t g_c348c;
extern uint32_t g_c3490;
extern uint8_t g_c3494;
extern uint8_t g_c3495;
extern uint8_t g_c3496;
extern uint8_t g_c3497;
extern uint8_t g_c3498;
extern uint8_t g_c3499;
extern uint8_t g_c349a;
extern uint8_t g_c349b;
extern uint8_t g_c349c;
extern uint8_t g_c349d;
extern uint16_t g_c349e;  // ширина з доступів у коді (widths.py)
extern uint8_t g_c34a0;
extern uint8_t g_c34a1;
extern uint8_t g_c34a2;
extern uint8_t g_c34a3;
extern uint8_t g_c34a4;
extern uint8_t g_c34a5;
extern uint8_t g_c34a6;
extern uint8_t g_c34a7;
extern uint8_t g_c34a8;
extern uint8_t g_c34a9;
extern uint8_t g_c34aa;
extern uint8_t g_c34ab;
extern uint8_t g_c34ac;
extern uint8_t g_c34ad;
extern uint8_t g_c34ae;
extern uint8_t g_c34af;
extern uint8_t g_c34b0;
extern uint8_t g_c34b1;
extern uint8_t g_c34b2;
extern uint8_t g_c34b3;
extern uint8_t g_c34b4;
extern uint8_t g_c34b5;
extern uint8_t g_c34b6;
extern uint8_t g_c34b7;
extern uint8_t g_c34b8;
extern uint8_t g_c34b9;
extern uint8_t g_c34ba;
extern MD_MAX72XX * g_c34bc;  // MD_MAX72XX об'єкта P (0x3ffc3520)
extern uint32_t g_c34c0;
extern uint8_t g_c34c4;
extern uint8_t g_c34c5;
extern uint32_t g_c34c8;
extern String g_c34cc;  // рядок (конструктор)

// адреси всередині масивів — елементи масивів
#define g_c25a0 (g_c2594[1])
#define g_c25ac (g_c2594[2])
#define g_c25b8 (g_c2594[3])
#define g_c25d0 (g_c25c4[1])
#define g_c25dc (g_c25c4[2])
#define g_c25e8 (g_c25c4[3])
#define g_c317c (g_c3170[1])
#define g_c3188 (g_c3170[2])
#define g_c3194 (g_c3170[3])
#define g_c31a0 (g_c3170[4])
#define g_c31ac (g_c3170[5])
#define g_c31b8 (g_c3170[6])
