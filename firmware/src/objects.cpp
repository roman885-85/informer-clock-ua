// EN: Global objects of the original firmware: display (MD_Parola), web server, WebSocket, DFPlayer, sensors.
#include "fw.h"
MD_Parola P(MD_MAX72XX::FC16_HW, 5, 16);
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;
HardwareSerial dfSerial(1);
DFRobotDFPlayerMini myDFPlayer;
RTC_NOINIT_ATTR uint32_t rtc_stage;
RTC_NOINIT_ATTR uint32_t rtc_magic;
// таблиця @3f40619c
// + етап 6 (нове): швидкі задачі, винесені з-під очікування кінця анімації (core.cpp, loop)
const char *const STAGE_NAMES[7] = { "idle/misc", "wifi/internet check (ping)", "wifi scan start", "wifi scan harvest",
                                     "NTP sync (oldloop)", "display + network features (generalLoop)",
                                     "brightness / night mode / web state" };
