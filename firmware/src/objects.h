// Глобальні об'єкти прошивки (з глобального конструктора автора @401008b8). Адреси — в оригіналі.
// EN: Global objects of the original firmware: declarations.
#pragma once
#include "libs.h"
extern MD_Parola P;                     // 0x3ffc3518: MD_Parola(FC16_HW, CS=5, 16 модулів), апаратний SPI
extern AsyncWebServer server;           // 0x3ffc2cd4: порт 80
extern AsyncWebSocket ws;               // 0x3ffc2c54: "/ws"
extern DNSServer dnsServer;             // 0x3ffc2dd4
extern HardwareSerial dfSerial;         // 0x3ffc3500: UART1 для DFPlayer
extern DFRobotDFPlayerMini myDFPlayer;  // 0x3ffc34d8
extern uint32_t rtc_stage, rtc_magic;   // RTC_SLOW 0x50000200 / 0x50000204: етап циклу для діагностики сторожа
extern const char *const STAGE_NAMES[7];
extern char g_c347d[11];               // рядок часу
extern char g_c3472[11];               // копія рядка часу з бітом 7
extern char g_c27f9[0x400];            // текст поточного пункту (коди табло)
extern char g_c27ef[10];               // короткий текст зони 3
extern const uint8_t SEC_GLYPHS[10];   // гліфи цифр секунд @3f4061b4
