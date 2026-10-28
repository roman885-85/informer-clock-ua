// MD5 (tzikis/ArduinoMD5: статичні make_hash / make_digest з результатом у malloc).
// Бібліотеки немає в реєстрі PlatformIO; поведінка (MD5 + hex малими літерами) відтворена на MD5 з ПЗУ ESP32.
#pragma once
#include <Arduino.h>

class MD5 {
 public:
  static unsigned char *make_hash(char *arg);
  static char *make_digest(const unsigned char *digest, int len);
};
