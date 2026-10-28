// Перекодування тексту для табло (власна однобайтна кодова сторінка автора) і назад. Перенесено з 2.7.1.
#include "fw.h"

// @400d4138: UTF-8 → коди шрифту табло (кирилиця на місцях cp1251, і/ї/є/Є/І/Ї — окремі коди, ° = 0x1c)
void utf8_to_led(const String &src, char *dst, int cap) {
  int n = src.length();
  unsigned int o = 0;
  int i = 0;
  while (i < n) {
    unsigned int c = (uint8_t)src[i];
    int ni = i + 1;
    unsigned int c2;
    switch ((c + 0x3e) & 0xff) {
      case 0:      // 0xC2
        c = (uint8_t)src[i + 1]; ni = i + 2;
        if (c == 0xb0) c = 0x1c;
        else if ((c & 0xef) == 0xab) c = 0x22;
        else if (c == 0xa0) c = 0x20;
        break;
      case 1:      // 0xC3
        c2 = (uint8_t)src[i + 1]; ni = i + 2; c = c2;
        if (c2 == 0x93 || c2 == 0x84 || c2 == 0xa4) c = 0xa0;
        else if (c2 == 0xb3) c = 0x94;
        break;
      case 2:      // 0xC4
        c2 = (uint8_t)src[i + 1]; ni = i + 2;
        if (c2 == 0x98) c = 0x88; else if (c2 == 0x99) c = 0x89; else c = c2;
        break;
      case 3:      // 0xC5
        c2 = (uint8_t)src[i + 1]; ni = i + 2; c = 0xa0;
        if (c2 == 0xa0) c = 0x53;
        else if (c2 < 0xa1) {
          if (c2 == 0x83) c = 0xa5;
          else if (c2 < 0x84) c = (c2 == 0x81) ? 0x91 : (c2 == 0x82 ? 0x92 : c2);
          else if (c2 == 0x9a) c = 0x98;
          else if (c2 == 0x9b) c = 0x99;
          else c = (c2 == 0x84) ? 0xa4 : c2;
        } else if (c2 == 0xbb) c = 0xa2;
        else if (c2 < 0xbc) {
          if (c2 == 0xb9) c = 0xa0;
          else if (0xb9 < c2) c = 0xa1;
          else c = (c2 == 0xa1) ? 0x73 : c2;
        } else if (c2 == 0xbd) c = 0x5a;
        else if (0xbc < c2) c = (c2 == 0xbe) ? 0x7a : c2;
        else c = 0xa3;
        break;
      case 8:      // 0xCA
        c = ((uint8_t)src[i + 1] != 0xbc) ? 0x20 : 0x27; ni = i + 2;
        break;
      case 10:     // 0xCC: комбінований діакритик змінює попередню літеру
        c2 = (uint8_t)src[i + 1]; i = i + 2;
        if (o != 0) {
          char *p = dst + (o - 1);
          int8_t pc = *p;
          if (c2 == 0x86) {
            if (pc == -0x18) pc = -0x17; else if (pc == -0x38) pc = -0x37; else continue;
          } else if (c2 == 0x88) {
            if (pc == -0x70) pc = -0x69; else if (pc == -0x71) pc = -0x79; else if (pc == -0x1b) pc = -0x48; else if (pc == -0x3b) pc = -0x58; else continue;
          } else continue;
          *p = pc;
        }
        continue;
      case 0xe:    // 0xD0
        c = (uint8_t)src[i + 1]; ni = i + 2;
        if (c == 0x86) c = 0x8f;
        else if (c == 0x81) c = 0xa8;
        else if (((c + 0x70) & 0xff) < 0x30) c = (c + 0x30) & 0xff;
        break;
      case 0xf:    // 0xD1
        c = (uint8_t)src[i + 1]; ni = i + 2;
        if (c == 0x96) c = 0x90;
        else if (c == 0x91) c = 0xb8;
        else if (((c - 0x80) & 0xff) < 0x10) c = (c + 0x70) & 0xff;
        break;
      case 0x10:   // 0xD2
        c2 = (uint8_t)src[i + 1]; ni = i + 2;
        switch (c2) {
          case 0x90: c = 0xc3; break; case 0x91: c = 0xe3; break; case 0xa3: c = 0x86; break; case 0x9a: c = 0x83; break;
          case 0x92: c = 0x81; break; case 0x93: c = 0x82; break; case 0x9b: c = 0x84; break; case 0xa2: c = 0x85; break;
          case 0xb0: c = 0x89; break; case 0xae: c = 0x8b; break; case 0xaf: c = 0x8c; break; case 0xba: c = 0x8d; break;
          case 0xbb: c = 0x8e; break; case 0xb1: c = 0x8a; break; default: c = c2; break;
        }
        break;
      case 0x11:   // 0xD3
        c2 = (uint8_t)src[i + 1]; ni = i + 2;
        if (c2 == 0x99) c = 0x80; else if (c2 == 0x98) c = 0x7f; else if (c2 == 0xa8) c = 0x87; else if (c2 == 0xa9) c = 0x88; else c = c2;
        break;
      case 0x20: { // 0xE2: три байти
        unsigned int a = (uint8_t)src[i + 1], b = (uint8_t)src[i + 2]; ni = i + 3;
        if (a == 0x84 && b == 0x96) c = 0x23;
        else if (a != 0x80) c = 0x20;
        else if (((b + 0x68) & 0xff) <= 1) c = 0x27;
        else if (((b + 100) & 0xff) <= 1) c = 0x22;
        else if (((b + 0x6d) & 0xff) <= 1) c = 0x2d;
        else c = 0x20;
        break;
      }
      default: break;
    }
    i = ni;
    if (o < (unsigned)(cap - 1)) { dst[o] = (char)c; o++; }
  }
  dst[o] = 0;
}

// @400e13a4: коди табло → UTF-8 (для веб-інтерфейсу)
String led_to_utf8(const String &s) {
  String r("");
  int n = s.length();
  for (int i = 0; i < n; i++) {
    unsigned int c = (uint8_t)s[i];
    if (((c - 0x20) & 0xff) < 0x60) { r += (char)c; continue; }
    if (((c + 0x40) & 0xff) < 0x30) { r += (char)0xd0; r += (char)((c - 0x30) & 0xff); continue; }
    if (0xef < c) { r += (char)0xd1; r += (char)((c - 0x70) & 0xff); continue; }
    if (c == 0xa8) { r += (char)0xd0; r += (char)0x81; continue; }
    if (c == 0xb8) { r += (char)0xd1; r += (char)0x91; continue; }
    if (c == 0x1c) { r += (char)0xc2; r += (char)0xb0; continue; }
  }
  return r;
}

// @401d8144: копія рядка з встановленим бітом 7 у кожному символі
void strcpy80(char *dst, const char *src) {
  for (; *src; src++, dst++) *dst = *src | 0x80;
  *dst = 0;
}
