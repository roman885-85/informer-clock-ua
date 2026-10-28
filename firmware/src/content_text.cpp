// Тексти пунктів інформаційного циклу (слоти 0…60). Перенесено з 2.7.1 (@4011675c) майже дослівно.
#include "fw.h"

// ===== content_slot_text @ 4011675c size=1059
// strings: "2.7.1" "2.7.1"
void content_slot_text(uint8_t param_1, String &param_2)
{
  String t_30, t_3c, t_48, t_54, t_60, t_6c, t_78, t_84, t_90, t_98;  // тимчасові
  uint8_t bVar1;
  uint8_t bVar2;
  uint8_t bVar3;
  uint8_t bVar4;
  uint8_t bVar5;
  uint8_t bVar6;
  uint8_t bVar7;
  uint8_t bVar8;
  uint8_t bVar9;
  uint8_t bVar10;
  uint8_t bVar11;
  uint8_t bVar12;
  uint8_t bVar13;
  uint8_t bVar14;
  char *pcVar15;
  int32_t iVar16;
  uint32_t uVar17;
  uint32_t uVar18;
  uint32_t uVar19;
  uint32_t uVar20;
  uint64_t uVar21;
  bVar13 = g_c3319;
  bVar12 = g_c32e8;
  bVar11 = g_c32e7;
  bVar10 = g_c32e5;
  bVar9 = g_c32e2;
  bVar8 = g_c32e1;
  bVar7 = g_c3157;
  bVar6 = g_c3139;
  bVar5 = g_c3138;
  bVar4 = g_c30f8;
  bVar3 = g_c30c4;
  bVar2 = g_c2ffc;
  bVar1 = g_c2efc;
  if (g_c34a9 != '\x03') {
    g_c3494 = 1;
    g_c3490 = 0;
    if (g_c2d29 == '\0') {
      if (param_1 == '\0') {
        rtc_detect_sync();
        g_c2f21 = hour();
        led_phrase(t_54,g_c34b0,0xae);
        t_48 = t_54;
        t_48 += g_c33bc;
        led_phrase(t_3c,g_c34b0,0xaf);
        t_48 += t_3c;
        t_30 = t_48;
        param_2 = t_30;
      }
      else if (param_1 == '\x03') {
        g_c34a3 = 0x10;
        g_c34a4 = 0x7d;
        g_c34a9 = '\x02';
        g_c3319 = 1;
        g_c34a5 = param_1;
        param_2 = "";
      }
      else {
        led_phrase(t_54,g_c34b0,0xae);
        t_48 = t_54;
        t_48 += g_c33bc;
        led_phrase(t_3c,g_c34b0,0xaf);
        t_48 += t_3c;
        t_30 = t_48;
        param_2 = t_30;
      }
      goto LAB_4011aaa4;
    }
    if (g_c3319 != 0) {
      g_c2ea8 = "2.7.1";
    }
    if (param_1 == '\x02') {
      g_c34a9 = '\x01';
      led_phrase(t_54,g_c34b0,0xad);
      t_3c = t_54;
      t_48 = WiFi.localIP().toString();
      t_3c += t_48;
      t_30 = t_3c;
      param_2 = t_30;
      goto LAB_4011aaa4;
    }
    if (param_1 == '\x03') {
      g_c34a9 = '\x02';
      led_phrase(t_78,g_c34b0,0xa9);
      t_60 = t_78;
      t_6c = "2.7.1";
      t_60 += t_6c;
      led_phrase(t_54,g_c34b0,0xaa);
      t_60 += t_54;
      t_48 = g_c2ea8;
      t_60 += t_48;
      led_phrase(t_3c,g_c34b0,0xab);
      t_60 += t_3c;
      t_30 = t_60;
      param_2 = t_30;
      goto LAB_4011aaa4;
    }
    bVar14 = g_c3494;
    if (param_1 == '\x01') {
      fw_version_check();
      g_c34a9 = param_1;
      led_phrase(t_54,g_c34b0,0xac);
      t_3c = t_54;
      t_48 = WiFi.SSID();
      t_3c += t_48;
      t_30 = t_3c;
      param_2 = t_30;
      goto LAB_4011aaa4;
    }
    goto LAB_4011a4c1;
  }
  bVar14 = g_c3494;
  switch(param_1) {
  case '\x01':
    if (g_c3319 != 0) {
      g_c34a5 = random(0, 3);
      if (g_c34a5 == '\x01') {
        g_c3494 = 0;
        g_c3490 = 2000;
        t_54 = "\x01" "-";
        t_3c = t_54;
        t_48 = "\x03";
        t_3c += t_48;
        t_30 = "\x02";
        t_3c += t_30;
        param_2 = t_3c;
      }
      else {
        g_c3494 = 1;
        g_c3490 = 0;
        param_2 = "";
      }
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case '\x02':
    if (g_c3319 != 0) {
      if (g_c34a5 == '\x01') {
        g_c3494 = 0;
        g_c3490 = 1000;
        pcVar15 = "NO INET";
        if (g_c2d29 == '\0') {
          pcVar15 = "NO WIFI";
        }
        t_30 = pcVar15;
        param_2 = t_30;
      }
      else {
        g_c3494 = 1;
        g_c3490 = 0;
        param_2 = "";
      }
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  default:
    g_c3494 = 1;
    g_c3490 = 0;
    param_2 = "";
    goto LAB_4011aaa4;
  case '\x04':
    g_c3494 = 1;
    g_c3490 = 0;
    param_2 = g_c3034;
    goto LAB_4011aaa4;
  case '\x05':
    g_c3494 = 1;
    g_c3490 = 0;
    param_2 = g_c3028;
    goto LAB_4011aaa4;
  case '\x06':
    iVar16 = (g_c301c != "");
    bVar14 = g_c3494;
    if (iVar16 == 0) break;
    rem1_date_parse();
    uVar19 = (uint)g_c2ee1;
    uVar20 = day();
    if (((uVar19 == uVar20) &&
        (uVar19 = (uint)g_c2ee0, uVar20 = month(), uVar19 == uVar20)) &&
       (iVar16 = year(), iVar16 == g_c2edc)) {
      g_c3494 = 1;
      g_c3490 = 0;
      param_2 = g_c301c;
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case '\a':
    iVar16 = (g_c3010 != "");
    bVar14 = g_c3494;
    if (iVar16 == 0) break;
    rem2_date_parse();
    uVar19 = (uint)g_c2ed9;
    uVar20 = day();
    if (((uVar19 == uVar20) &&
        (uVar19 = (uint)g_c2ed8, uVar20 = month(), uVar19 == uVar20)) &&
       (iVar16 = year(), iVar16 == g_c2ed4)) {
      g_c3494 = 1;
      g_c3490 = 0;
      param_2 = g_c3010;
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case '\b':
    uVar20 = (uint)g_c2eeb;
    if (uVar20 == 1) {
      g_c3494 = g_c2eeb;
      g_c3490 = 0;
      date_text(t_30, 1);
      param_2 = t_30;
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      g_c3494 = 0;
      g_c3490 = 2000;
      t_48 = "\x11";
      t_30 = t_48;
      led_phrase(t_3c,g_c34b0,0x70);
      t_30 += t_3c;
      param_2 = t_30;
      goto LAB_4011aaa4;
    }
    break;
  case '\t':
    uVar20 = (uint)g_c2eeb;
    if (uVar20 == 1) {
      g_c3494 = g_c2eeb;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      g_c3494 = 1;
      g_c3490 = 2000;
      weekday_text(t_30);
      param_2 = t_30;
      goto LAB_4011aaa4;
    }
    break;
  case '\n':
    uVar20 = (uint)g_c2eeb;
    if (uVar20 == 1) {
      g_c3494 = g_c2eeb;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      g_c3494 = 0;
      g_c3490 = 5000;
      day2_text(t_54);
      t_48 = t_54;
      t_48 += ".";
      month2_text(t_3c);
      t_48 += t_3c;
      t_48 += ".";
      year2_text(t_30);
      t_48 += t_30;
      param_2 = t_48;
      goto LAB_4011aaa4;
    }
    break;
  case '\r':
    uVar20 = (uint)g_c32e8;
    if (uVar20 == 1) {
      sensor_ds18b20();
      g_c3494 = bVar12;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      sensor_ds18b20();
      g_c3494 = 0;
      g_c3490 = 0x5dc;
      t_48 = "DS";
      t_30 = t_48;
      t_3c = "\t";
      t_30 += t_3c;
      param_2 = t_30;
      goto LAB_4011aaa4;
    }
    break;
  case '\x0e':
    uVar20 = (uint)g_c32e8;
    if (uVar20 == 1) {
      sensor_ds18b20();
      g_c3494 = bVar12;
      g_c3490 = 0;
      if (g_c3300 == -127.0) {
        param_2 = "DS not connected  \xa6";
      }
      else {
        t_30 = g_c32b8;
        t_3c = String(g_c3300, 1);
        t_30 += t_3c;
        t_30 += "\x1c" "\xd0" "\xa1";
        param_2 = t_30;
      }
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      sensor_ds18b20();
      g_c3494 = 0;
      g_c3490 = 5000;
      if (g_c3300 == -127.0) {
        t_30 = "\n";
        param_2 = t_30;
      }
      else {
        t_60 = "\x1b" " ";
        t_48 = t_60;
        t_54 = String(g_c3300, 1);
        t_48 += t_54;
        t_3c = "\x1c";
        t_48 += t_3c;
        t_30 = "\x1d";
        t_48 += t_30;
        param_2 = t_48;
      }
      goto LAB_4011aaa4;
    }
    break;
  case '\x0f':
    uVar20 = (uint)g_c32e5;
    if (uVar20 == 1) {
      g_c3494 = g_c32e5;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      if (g_c2ee5 == 1) {
        g_c3494 = 0;
        g_c3490 = 0x5dc;
        t_48 = "BMP";
        t_30 = t_48;
        t_3c = " \x13";
        t_30 += t_3c;
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
      if (g_c2ee5 == 0) {
        g_c3494 = g_c2ee5;
        g_c3490 = 0x5dc;
        t_48 = "BME";
        t_30 = t_48;
        t_3c = " \x13";
        t_30 += t_3c;
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
      if (g_c2ee5 == 2) {
        g_c3494 = 0;
        g_c3490 = 0x5dc;
        t_48 = "BMP";
        t_30 = t_48;
        t_3c = " \x13";
        t_30 += t_3c;
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
    }
    break;
  case '\x10':
    uVar20 = (uint)g_c32e5;
    if (uVar20 == 1) {
      sensor_bosch();
      uVar20 = (uint)g_c2ee5;
      if (uVar20 == 1) {
        if ((g_c3314 == 0.0) && (g_c330c == 0.0)) {
          g_c3494 = g_c2ee5;
          g_c3490 = 0;
          t_30 = "BMP280 : Not Connected";
          param_2 = t_30;
        }
        else {
          g_c3494 = 1;
          g_c3490 = 0;
          t_60 = g_c32ac;
          t_6c = String(g_c3314, 1);
          t_60 += t_6c;
          t_54 = "\x1c" "\xd0" "\xa1" " ";
          t_60 += t_54;
          led_phrase(t_48,g_c34b0,0x48);
          t_60 += t_48;
          t_3c = String(g_c330c, 0);
          t_60 += t_3c;
          led_phrase(t_30,g_c34b0,0x4b);
          t_60 += t_30;
          param_2 = t_60;
        }
        goto LAB_4011aaa4;
      }
      if (uVar20 == 0) {
        if (((g_c3314 == 0.0) && (g_c330c == 0.0)) && (g_c3310 == 0.0)) {
          g_c3494 = bVar10;
          g_c3490 = uVar20;
          t_30 = "BME280 : Not Connected";
          param_2 = t_30;
        }
        else {
          g_c3494 = 1;
          g_c3490 = 0;
          t_84 = g_c32ac;
          t_90 = String(g_c3314, 1);
          t_84 += t_90;
          t_78 = "\x1c" "\xd0" "\xa1" " ";
          t_84 += t_78;
          led_phrase(t_6c,g_c34b0,0x47);
          t_84 += t_6c;
          t_60 = String(round((double)g_c3310), 0);
          t_84 += t_60;
          t_54 = "% ";
          t_84 += t_54;
          led_phrase(t_48,g_c34b0,0x48);
          t_84 += t_48;
          t_3c = String(g_c330c, 0);
          t_84 += t_3c;
          led_phrase(t_30,g_c34b0,0x4b);
          t_84 += t_30;
          param_2 = t_84;
        }
        goto LAB_4011aaa4;
      }
      bVar14 = g_c3494;
      if (uVar20 == 2) {
        if ((g_c3314 == 0.0) && (g_c330c == 0.0)) {
          g_c3494 = bVar10;
          g_c3490 = 0;
          t_30 = "BMP180 : Not Connected";
          param_2 = t_30;
        }
        else {
          g_c3494 = 1;
          g_c3490 = 0;
          t_60 = g_c32ac;
          t_6c = String(g_c3314, 1);
          t_60 += t_6c;
          t_54 = "\x1c" "\xd0" "\xa1" " ";
          t_60 += t_54;
          led_phrase(t_48,g_c34b0,0x48);
          t_60 += t_48;
          t_3c = String(g_c330c, 0);
          t_60 += t_3c;
          led_phrase(t_30,g_c34b0,0x4b);
          t_60 += t_30;
          param_2 = t_60;
        }
        goto LAB_4011aaa4;
      }
    }
    else {
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 == 2) {
        sensor_bosch();
        if (g_c2ee5 == 1) {
          if ((g_c3314 == 0.0) && (g_c330c == 0.0)) {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_30 = "\x04";
            param_2 = t_30;
          }
          else {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_60 = "\x1b" " ";
            t_48 = t_60;
            t_54 = String(g_c3314, 1);
            t_48 += t_54;
            t_3c = "\x1c";
            t_48 += t_3c;
            t_30 = "\x1d";
            t_48 += t_30;
            param_2 = t_48;
          }
          goto LAB_4011aaa4;
        }
        if (g_c2ee5 == 0) {
          if (((g_c3314 == 0.0) && (g_c330c == 0.0)) && (g_c3310 == 0.0)) {
            g_c3494 = g_c2ee5;
            g_c3490 = 5000;
            t_30 = "\x04";
            param_2 = t_30;
          }
          else {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_60 = "\x1b" " ";
            t_48 = t_60;
            t_54 = String(g_c3314, 1);
            t_48 += t_54;
            t_3c = "\x1c";
            t_48 += t_3c;
            t_30 = "\x1d";
            t_48 += t_30;
            param_2 = t_48;
          }
          goto LAB_4011aaa4;
        }
        bVar14 = g_c3494;
        if (g_c2ee5 == 2) {
          if ((g_c3314 == 0.0) && (g_c330c == 0.0)) {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_30 = "\x04";
            param_2 = t_30;
          }
          else {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_60 = "\x1b" " ";
            t_48 = t_60;
            t_54 = String(g_c3314, 1);
            t_48 += t_54;
            t_3c = "\x1c";
            t_48 += t_3c;
            t_30 = "\x1d";
            t_48 += t_30;
            param_2 = t_48;
          }
          goto LAB_4011aaa4;
        }
      }
    }
    break;
  case '\x11':
    uVar20 = (uint)g_c32e5;
    if (uVar20 == 1) {
      sensor_bosch();
      g_c3494 = bVar10;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 != 2) break;
    sensor_bosch();
    if (g_c2ee5 == 0) {
      if (((g_c3314 != 0.0) || (g_c330c != 0.0)) || (g_c3310 != 0.0)) {
        g_c3494 = 0;
        g_c3490 = 4000;
        t_54 = "\x1e" " ";
        t_3c = t_54;
        t_48 = String(round((double)g_c3310), 0);
        t_3c += t_48;
        t_30 = "%";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
    }
    else {
      bVar14 = g_c3494;
      if (2 < g_c2ee5) break;
    }
    goto LAB_40116864;
  case '\x12':
    uVar20 = (uint)g_c32e5;
    if (uVar20 == 1) {
      sensor_bosch();
      g_c3494 = bVar10;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 != 2) break;
    sensor_bosch();
    if (g_c2ee5 == 1) {
      if ((g_c3314 != 0.0) || (g_c330c != 0.0)) {
        g_c3494 = 0;
        g_c3490 = 4000;
        t_54 = "\x12";
        t_3c = t_54;
        t_48 = String(g_c330c, 0);
        t_3c += t_48;
        t_30 = "\x14";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
    }
    else if (g_c2ee5 == 0) {
      if (((g_c3314 != 0.0) || (g_c330c != 0.0)) || (g_c3310 != 0.0)) {
        g_c3494 = 0;
        g_c3490 = 4000;
        t_54 = "\x12";
        t_3c = t_54;
        t_48 = String(g_c330c, 0);
        t_3c += t_48;
        t_30 = "\x14";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
    }
    else {
      bVar14 = g_c3494;
      if (g_c2ee5 != 2) break;
      if ((g_c3314 != 0.0) || (g_c330c != 0.0)) {
        g_c3494 = 0;
        g_c3490 = 4000;
        t_54 = "\x12";
        t_3c = t_54;
        t_48 = String(g_c330c, 0);
        t_3c += t_48;
        t_30 = "\x14";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
    }
    goto LAB_40116864;
  case '\x13':
    uVar20 = (uint)g_c32e7;
    if (uVar20 == 1) {
      sensor_dht();
      g_c3494 = bVar11;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      sensor_dht();
      g_c3494 = 0;
      g_c3490 = 0x5dc;
      t_48 = "DHT ";
      t_30 = t_48;
      t_3c = "\r";
      t_30 += t_3c;
      param_2 = t_30;
      goto LAB_4011aaa4;
    }
    break;
  case '\x14':
    uVar20 = (uint)g_c32e7;
    if (uVar20 == 1) {
      sensor_dht();
      t_3c = String(g_c3308, 0);
      t_30 = "nan";
      uVar20 = t_3c.equals(t_30);
      g_c3494 = 1;
      if (uVar20 == 0) {
        g_c3490 = uVar20;
        t_54 = g_c32a0;
        t_60 = String(g_c3308, 1);
        t_54 += t_60;
        t_48 = "\x1c" "\xd0" "\xa1" " ";
        t_54 += t_48;
        led_phrase(t_3c,g_c34b0,0x47);
        t_54 += t_3c;
        t_30 = String(g_c3304, 0);
        t_54 += t_30;
        t_54 += "%";
        param_2 = t_54;
      }
      else {
        g_c3490 = 0;
        param_2 = "DHT: Not connected";
      }
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      sensor_dht();
      t_3c = String(g_c3308, 0);
      t_30 = "nan";
      iVar16 = t_3c.equals(t_30);
      if (iVar16 == 0) {
        g_c3494 = 0;
        g_c3490 = 5000;
        t_60 = "\x1b" " ";
        t_48 = t_60;
        t_54 = String(g_c3308, 1);
        t_48 += t_54;
        t_3c = "\x1c";
        t_48 += t_3c;
        t_30 = "\x1d";
        t_48 += t_30;
        param_2 = t_48;
      }
      else {
        g_c3494 = 0;
        g_c3490 = 5000;
        t_30 = "DHT\x04";
        param_2 = t_30;
      }
      goto LAB_4011aaa4;
    }
    break;
  case '\x15':
    uVar20 = (uint)g_c32e7;
    if (uVar20 == 1) {
      sensor_dht();
      g_c3494 = bVar11;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 != 2) break;
    sensor_dht();
    t_3c = String(g_c3308, 0);
    t_30 = "nan";
    iVar16 = t_3c.equals(t_30);
    if (iVar16 == 0) {
      g_c3494 = 0;
      g_c3490 = 4000;
      t_54 = "\x1e" " ";
      t_3c = t_54;
      t_48 = String(g_c3304, 0);
      t_3c += t_48;
      t_30 = "%";
      t_3c += t_30;
      param_2 = t_3c;
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case '\x16':
    uVar20 = (uint)g_c32e2;
    if (uVar20 == 1) {
      sensor_other();
      g_c3494 = bVar9;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      sensor_other();
      if (g_c32e3 == 1) {
        g_c3494 = 0;
        g_c3490 = 0x5dc;
        t_48 = "HDC ";
        t_30 = t_48;
        t_3c = "\x13";
        t_30 += t_3c;
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
      if (g_c32e3 == 0) {
        g_c3494 = g_c32e3;
        g_c3490 = 0x5dc;
        t_48 = "AM ";
        t_30 = t_48;
        t_3c = "\r";
        t_30 += t_3c;
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
      bVar14 = g_c3494;
      if (g_c32e3 == 2) {
        g_c3494 = 0;
        g_c3490 = 0x5dc;
        t_48 = "AHT ";
        t_30 = t_48;
        t_3c = "\r";
        t_30 += t_3c;
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
    }
    break;
  case '\x17':
    uVar20 = (uint)g_c32e2;
    if (uVar20 == 1) {
      sensor_other();
      if (g_c32e3 == 1) {
        if (g_c32fc <= 100.0) {
          if ((g_c32fc == 0.0) || (g_c32f8 == 0.0)) {
            g_c3494 = 1;
            g_c3490 = 0;
            t_30 = "HDC1080: Not Connected";
            param_2 = t_30;
          }
          else {
            g_c3494 = g_c32e3;
            g_c3490 = 0;
            t_54 = g_c3294;
            t_60 = String(g_c32fc, 1);
            t_54 += t_60;
            t_48 = "\x1c" "\xd0" "\xa1" " ";
            t_54 += t_48;
            led_phrase(t_3c,g_c34b0,0x47);
            t_54 += t_3c;
            t_30 = String(g_c32f8, 0);
            t_54 += t_30;
            t_54 += "%";
            param_2 = t_54;
          }
        }
        else {
          g_c3494 = g_c32e3;
          g_c3490 = 0;
          t_30 = "HDC1080: Not Connected";
          param_2 = t_30;
        }
        goto LAB_4011aaa4;
      }
      if (g_c32e3 == 0) {
        t_3c = String(g_c32fc, 0);
        t_30 = "nan";
        uVar20 = t_3c.equals(t_30);
        g_c3494 = 1;
        if (uVar20 == 0) {
          g_c3490 = uVar20;
          t_54 = g_c3294;
          t_60 = String(g_c32fc, 1);
          t_54 += t_60;
          t_48 = "\x1c" "\xd0" "\xa1" " ";
          t_54 += t_48;
          led_phrase(t_3c,g_c34b0,0x47);
          t_54 += t_3c;
          t_30 = String(g_c32f8, 0);
          t_54 += t_30;
          t_54 += "%";
          param_2 = t_54;
        }
        else {
          g_c3490 = 0;
          t_30 = "AM2320: Not Connected";
          param_2 = t_30;
        }
        goto LAB_4011aaa4;
      }
      bVar14 = g_c3494;
      if (g_c32e3 == 2) {
        t_3c = String(g_c32fc, 0);
        t_30 = "nan";
        uVar20 = t_3c.equals(t_30);
        g_c3494 = 1;
        if (uVar20 == 0) {
          g_c3490 = uVar20;
          t_54 = g_c3294;
          t_60 = String(g_c32fc, 1);
          t_54 += t_60;
          t_48 = "\x1c" "\xd0" "\xa1" " ";
          t_54 += t_48;
          led_phrase(t_3c,g_c34b0,0x47);
          t_54 += t_3c;
          t_30 = String(g_c32f8, 0);
          t_54 += t_30;
          t_54 += "%";
          param_2 = t_54;
        }
        else {
          g_c3490 = 0;
          t_30 = "AHT: Not Connected";
          param_2 = t_30;
        }
        goto LAB_4011aaa4;
      }
    }
    else {
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 == 2) {
        sensor_other();
        if (g_c32e3 == 1) {
          if (g_c32fc <= 100.0) {
            if ((g_c32fc == 0.0) || (g_c32f8 == 0.0)) {
              g_c3494 = 0;
              g_c3490 = 5000;
              t_30 = "\x04";
              param_2 = t_30;
            }
            else {
              g_c3494 = 0;
              g_c3490 = 5000;
              t_60 = "\x1b" " ";
              t_48 = t_60;
              t_54 = String(g_c32fc, 1);
              t_48 += t_54;
              t_3c = "\x1c";
              t_48 += t_3c;
              t_30 = "\x1d";
              t_48 += t_30;
              param_2 = t_48;
            }
          }
          else {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_30 = "\x04";
            param_2 = t_30;
          }
          goto LAB_4011aaa4;
        }
        if (g_c32e3 == 0) {
          t_3c = String(g_c32fc);
          t_30 = "nan";
          iVar16 = t_3c.equals(t_30);
          if (iVar16 == 0) {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_60 = "\x1b" " ";
            t_48 = t_60;
            t_54 = String(g_c32fc, 1);
            t_48 += t_54;
            t_3c = "\x1c";
            t_48 += t_3c;
            t_30 = "\x1d";
            t_48 += t_30;
            param_2 = t_48;
          }
          else {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_30 = "\x04";
            param_2 = t_30;
          }
          goto LAB_4011aaa4;
        }
        bVar14 = g_c3494;
        if (g_c32e3 == 2) {
          t_3c = String(g_c32fc, 0);
          t_30 = "nan";
          iVar16 = t_3c.equals(t_30);
          if (iVar16 == 0) {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_60 = "\x1b" " ";
            t_48 = t_60;
            t_54 = String(g_c32fc, 1);
            t_48 += t_54;
            t_3c = "\x1c";
            t_48 += t_3c;
            t_30 = "\x1d";
            t_48 += t_30;
            param_2 = t_48;
          }
          else {
            g_c3494 = 0;
            g_c3490 = 5000;
            t_30 = "\x04";
            param_2 = t_30;
          }
          goto LAB_4011aaa4;
        }
      }
    }
    break;
  case '\x18':
    uVar20 = (uint)g_c32e2;
    if (uVar20 == 1) {
      sensor_other();
      g_c3494 = bVar9;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 != 2) break;
    sensor_other();
    if (g_c32e3 == 1) {
      if (((g_c32fc <= 100.0) && (g_c32fc != 0.0)) && (g_c32f8 != 0.0)) {
        g_c3494 = 0;
        g_c3490 = 4000;
        t_54 = "\x1e" " ";
        t_3c = t_54;
        t_48 = String(g_c32f8, 0);
        t_3c += t_48;
        t_30 = "%";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
    }
    else if (g_c32e3 == 0) {
      t_3c = String(g_c32fc);
      t_30 = "nan";
      iVar16 = t_3c.equals(t_30);
      if (iVar16 == 0) {
        g_c3494 = 0;
        g_c3490 = 4000;
        t_54 = "\x1e" " ";
        t_3c = t_54;
        t_48 = String(g_c32f8, 0);
        t_3c += t_48;
        t_30 = "%";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
    }
    else {
      bVar14 = g_c3494;
      if (g_c32e3 != 2) break;
      t_3c = String(g_c32fc, 0);
      t_30 = "nan";
      iVar16 = t_3c.equals(t_30);
      if (iVar16 == 0) {
        g_c3494 = 0;
        g_c3490 = 4000;
        t_54 = "\x1e" " ";
        t_3c = t_54;
        t_48 = String(g_c32f8, 0);
        t_3c += t_48;
        t_30 = "%";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
    }
    goto LAB_40116864;
  case '\x19':
    uVar20 = (uint)g_c32e1;
    if (uVar20 == 1) {
      sensor_air();
      g_c3494 = bVar8;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      sensor_air();
      bVar14 = g_c3494;
      g_c3494 = 0;
      switch(g_c32e0) {
      case 0:
        g_c3490 = 0x5dc;
        t_48 = "SGP30 ";
        t_30 = t_48;
        t_3c = "\x13";
        t_30 += t_3c;
        param_2 = t_30;
        break;
      case 1:
        g_c3490 = 0x5dc;
        t_48 = "CCS811 ";
        t_30 = t_48;
        t_3c = "\x13";
        t_30 += t_3c;
        param_2 = t_30;
        break;
      case 2:
        g_c3490 = 0x5dc;
        t_48 = "MH-Z19b ";
        t_30 = t_48;
        t_3c = "\x13";
        t_30 += t_3c;
        param_2 = t_30;
        break;
      case 3:
        g_c3490 = 0x5dc;
        t_48 = "SCD40 ";
        t_30 = t_48;
        t_3c = "\x13";
        t_30 += t_3c;
        param_2 = t_30;
        break;
      case 4:
        g_c3490 = 0x5dc;
        t_48 = "SCD41 ";
        t_30 = t_48;
        t_3c = "\x13";
        t_30 += t_3c;
        param_2 = t_30;
        break;
      default:
        goto LAB_4011a4c1;
      }
      goto LAB_4011aaa4;
    }
    break;
  case '\x1a':
    uVar20 = (uint)g_c32e1;
    if (uVar20 == 1) {
      sensor_air();
      uVar20 = (uint)g_c32e0;
      if (uVar20 == 1) {
        g_c3494 = g_c32e0;
        g_c3490 = 0;
        t_60 = g_c3288;
        t_6c = "TVOC: ";
        t_60 += t_6c;
        t_54 = String(g_c32f4, 10);
        t_60 += t_54;
        t_48 = " ppb ";
        t_60 += t_48;
        t_3c = "eCO2: ";
        t_60 += t_3c;
        t_30 = String(g_c32f0, 10);
        t_60 += t_30;
        t_60 += " ppm";
        param_2 = t_60;
      }
      else if (uVar20 == 0) {
        g_c3490 = (uint)g_c1ba4;
        g_c3494 = bVar8;
        if (g_c3490 == 0) {
          t_30 = "SGP30: Not Connected";
          param_2 = t_30;
        }
        else {
          g_c3490 = uVar20;
          t_60 = g_c3288;
          t_6c = "TVOC: ";
          t_60 += t_6c;
          t_54 = String(g_c32f4, 10);
          t_60 += t_54;
          t_48 = " ppb ";
          t_60 += t_48;
          t_3c = "eCO2: ";
          t_60 += t_3c;
          t_30 = String(g_c32f0, 10);
          t_60 += t_30;
          t_60 += " ppm";
          param_2 = t_60;
        }
      }
      else if (uVar20 < 5) {
        g_c3494 = bVar8;
        g_c3490 = 0;
        t_3c = g_c3288;
        t_48 = "CO2: ";
        t_3c += t_48;
        t_30 = String(g_c32f0, 10);
        t_3c += t_30;
        t_3c += " ppm";
        param_2 = t_3c;
      }
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      sensor_air();
      if (g_c32e0 == 1) {
        g_c3494 = 0;
        g_c3490 = 5000;
        t_54 = "TVOC ";
        t_3c = t_54;
        t_48 = String(g_c32f4, 10);
        t_3c += t_48;
        t_30 = "ppb";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
      if (g_c32e0 == 0) {
        if (g_c1ba4 == 0) {
          g_c3494 = g_c1ba4;
          g_c3490 = 5000;
          t_30 = "\x04";
          param_2 = t_30;
        }
        else {
          g_c3494 = g_c32e0;
          g_c3490 = 5000;
          t_54 = "TVOC ";
          t_3c = t_54;
          t_48 = String(g_c32f4, 10);
          t_3c += t_48;
          t_30 = "ppb";
          t_3c += t_30;
          param_2 = t_3c;
        }
        goto LAB_4011aaa4;
      }
      bVar14 = g_c3494;
      if (g_c32e0 < 5) {
        g_c3494 = 0;
        if (g_c32f0 - 0x15eU < 0x122b) {
          g_c3490 = 4000;
          t_54 = "CO2 ";
          t_3c = t_54;
          t_48 = String(g_c32f0, 10);
          t_3c += t_48;
          t_30 = "ppm";
          t_3c += t_30;
          param_2 = t_3c;
        }
        else {
          g_c3490 = 5000;
          t_30 = "\x04";
          param_2 = t_30;
        }
        goto LAB_4011aaa4;
      }
    }
    break;
  case '\x1b':
    uVar20 = (uint)g_c32e1;
    if (uVar20 == 1) {
      sensor_air();
      g_c3494 = bVar8;
      g_c3490 = 0;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar20 == 2) {
      sensor_air();
      if (g_c32e0 == 1) {
        g_c3494 = 0;
        g_c3490 = 4000;
        t_54 = "eCO2 ";
        t_3c = t_54;
        t_48 = String(g_c32f0, 10);
        t_3c += t_48;
        t_30 = "ppm";
        t_3c += t_30;
        param_2 = t_3c;
        goto LAB_4011aaa4;
      }
      if (g_c32e0 == 0) {
        if (g_c1ba4 != 0) {
          g_c3494 = g_c32e0;
          g_c3490 = 4000;
          t_54 = "eCO2 ";
          t_3c = t_54;
          t_48 = String(g_c32f0, 10);
          t_3c += t_48;
          t_30 = "ppm";
          t_3c += t_30;
          param_2 = t_3c;
          goto LAB_4011aaa4;
        }
        goto LAB_40116864;
      }
      bVar14 = g_c3494;
      if (g_c32e0 < 5) {
        g_c3494 = 1;
        g_c3490 = 0;
        param_2 = "";
        goto LAB_4011aaa4;
      }
    }
    break;
  case '\x1e':
    if (g_c3319 == 0) {
      uVar20 = (uint)g_c30c4;
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 == 1) {
        t_6c = "";
        if (g_c30c5 == 0) {
          g_c3494 = bVar3;
          g_c3490 = (uint)g_c30c5;
          t_6c = "";
        }
        else {
          iVar16 = 0x1f;
          do {
            mqtt_text(t_60,iVar16);
          (void)(t_60 != "");
            iVar16 = iVar16 + 1;
          } while (iVar16 != 0x23);
          g_c3494 = 0;
          g_c3490 = 2000;
          t_54 = "MQTT";
          t_3c = t_54;
          t_48 = "\x15";
          t_3c += t_48;
          t_6c = t_3c;
        }
        t_30 = t_6c;
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
      break;
    }
    goto LAB_40116864;
  case '\x1f':
    if (g_c3319 == 0) {
      uVar20 = (uint)g_c30c4;
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 != 1) break;
      t_3c = "";
      if (g_c30c5 == 0) {
        g_c3494 = bVar3;
LAB_401198b2:
        g_c3490 = 0;
        param_2 = "";
      }
      else {
        mqtt_text(t_30,0x1f);
        t_3c = t_30;
        iVar16 = (t_3c != "");
        g_c3494 = 1;
        if (iVar16 == 0) goto LAB_401198b2;
        g_c3490 = 0;
        param_2 = t_3c;
      }
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case ' ':
    if (g_c3319 == 0) {
      uVar20 = (uint)g_c30c4;
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 != 1) break;
      t_3c = "";
      if (g_c30c5 == 0) {
        g_c3494 = bVar3;
LAB_40119972:
        g_c3490 = 0;
        param_2 = "";
      }
      else {
        mqtt_text(t_30,0x20);
        t_3c = t_30;
        iVar16 = (t_3c != "");
        g_c3494 = 1;
        if (iVar16 == 0) goto LAB_40119972;
        g_c3490 = 0;
        param_2 = t_3c;
      }
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case '!':
    if (g_c3319 == 0) {
      uVar20 = (uint)g_c30c4;
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 != 1) break;
      t_3c = "";
      if (g_c30c5 == 0) {
        g_c3494 = bVar3;
LAB_40119a32:
        g_c3490 = 0;
        param_2 = "";
      }
      else {
        mqtt_text(t_30,0x21);
        t_3c = t_30;
        iVar16 = (t_3c != "");
        g_c3494 = 1;
        if (iVar16 == 0) goto LAB_40119a32;
        g_c3490 = 0;
        param_2 = t_3c;
      }
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case '\"':
    if (g_c3319 == 0) {
      uVar20 = (uint)g_c30c4;
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 != 1) break;
      t_3c = "";
      if (g_c30c5 == 0) {
        g_c3494 = bVar3;
LAB_40119af2:
        g_c3490 = 0;
        param_2 = "";
      }
      else {
        mqtt_text(t_30,0x22);
        t_3c = t_30;
        iVar16 = (t_3c != "");
        g_c3494 = 1;
        if (iVar16 == 0) goto LAB_40119af2;
        g_c3490 = 0;
        param_2 = t_3c;
      }
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case '%':
    uVar20 = (uint)g_c3139;
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if ((uVar20 == 1) && (uVar20 = (uint)g_c3319, uVar20 == 0)) {
      uVar19 = (uint)g_c3138;
      if (uVar19 == 0) {
        currency_fetch();
        g_c3494 = bVar6;
        g_c3490 = uVar19;
        param_2 = g_c1aec;
        goto LAB_4011aaa4;
      }
      if (uVar19 == 1) {
        currency_fetch();
        bitcoin_fetch();
        g_c3494 = bVar5;
        g_c3490 = uVar20;
        t_30 = g_c1aec;
        t_30 += g_c1ad8;
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
    }
    break;
  case '(':
    uVar20 = (uint)g_c3319;
    if (uVar20 != 0) break;
    uVar19 = (uint)g_c3157;
    if (uVar19 == 1) {
      iVar16 = (g_c3148 != "");
      if (iVar16 != 0) {
        weather_owm(g_c316c,g_c3168,g_c3148);
        g_c3494 = bVar7;
        g_c3490 = uVar20;
        param_2 = g_c1a6c;
        goto LAB_4011aaa4;
      }
    }
    else {
      if (uVar19 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar19;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar19 == 2) {
        iVar16 = (g_c313c != "");
        if (iVar16 != 0) {
          if (g_c3158 == 1) {
            weather_weatherbit_tomorrow(g_c316c,g_c3168,g_c313c);
          }
          weather_weatherbit_now(g_c316c,g_c3168,g_c313c);
          g_c3494 = 1;
          g_c3490 = 0;
          param_2 = g_c1a54;
          goto LAB_4011aaa4;
        }
      }
      else {
        if (uVar19 != 3) break;
        iVar16 = (g_c3148 != "");
        if (iVar16 != 0) {
          weather_yandex(g_c3148);
          g_c3494 = 1;
          g_c3490 = uVar20;
          param_2 = g_c1a34;
          goto LAB_4011aaa4;
        }
      }
    }
    goto LAB_40116864;
  case ')':
    uVar20 = (uint)g_c3319;
    if (uVar20 != 0) goto LAB_40119d24;
    uVar19 = (uint)g_c3157;
    if (uVar19 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar19;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if (uVar19 != 2) break;
    iVar16 = (g_c313c != "");
    if ((iVar16 != 0) && (g_c3158 == 1)) {
      g_c3494 = g_c3158;
      g_c3490 = uVar20;
      param_2 = g_c1a48;
      goto LAB_4011aaa4;
    }
    goto LAB_40116864;
  case ',':
LAB_40119d24:
    if (g_c2eea == 0) {
      g_c3494 = 1;
      g_c3490 = (uint)g_c2eea;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    uVar20 = (uint)g_c3319;
    if (uVar20 == 0) {
      jokes_fetch();
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = g_c1ac4;
      goto LAB_4011aaa4;
    }
    break;
  case '/':
    uVar20 = (uint)g_c30f8;
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if ((uVar20 == 1) && (uVar20 = (uint)g_c3319, uVar20 == 0)) {
      iVar16 = (g_c30e0 != "");
      if ((iVar16 != 0) && (iVar16 = (g_c30ec == "gn"), iVar16 != 0)) {
        news_gnews();
        g_c3494 = bVar4;
        g_c3490 = uVar20;
        param_2 = g_c30fc;
        goto LAB_4011aaa4;
      }
      iVar16 = (g_c30e0 != "");
      bVar14 = g_c3494;
      if ((iVar16 != 0) &&
         (iVar16 = (g_c30ec == "ndio"), bVar14 = g_c3494, iVar16 != 0)) {
        news_newsdata();
        g_c3494 = 1;
        g_c3490 = 0;
        param_2 = g_c30fc;
        goto LAB_4011aaa4;
      }
    }
    break;
  case '2':
    uVar20 = (uint)g_c2ffc;
    if (uVar20 == 0) {
      g_c3494 = 1;
      g_c3490 = uVar20;
      param_2 = "";
      goto LAB_4011aaa4;
    }
    if ((uVar20 != 1) || (uVar20 = (uint)g_c3319, uVar20 != 0)) break;
    iVar16 = (g_c2fcc != "");
    if ((iVar16 != 0) && (iVar16 = (g_c2fd8 != ""), iVar16 != 0)) {
      narodmon_read_cfg();
      iVar16 = (g_c2fe4 != "");
      if (iVar16 != 0) {
        g_c3494 = bVar2;
        g_c3490 = uVar20;
        param_2 = g_c2fe4;
        goto LAB_4011aaa4;
      }
    }
    goto LAB_40116864;
  case '5':
    uVar20 = (uint)g_c3319;
    if (uVar20 != 0) break;
    uVar19 = (uint)g_c2efc;
    if (uVar19 == 1) {
      iVar16 = (g_c2f0c != "");
      if ((iVar16 != 0) && (iVar16 = (g_c2f00 != ""), iVar16 != 0)) {
        youtube_fetch();
        g_c3494 = bVar1;
        g_c3490 = uVar20;
        param_2 = g_c1c80;
        goto LAB_4011aaa4;
      }
    }
    else {
      if (uVar19 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar19;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar19 != 2) break;
      iVar16 = (g_c2f0c != "");
      if ((iVar16 != 0) && (iVar16 = (g_c2f00 != ""), iVar16 != 0)) {
        youtube_fetch();
        g_c3494 = bVar13;
        g_c3490 = 0x5dc;
        t_30 = "\x0e" "\x0f";
        param_2 = t_30;
        goto LAB_4011aaa4;
      }
    }
LAB_40116864:
    g_c3494 = 1;
    g_c3490 = 0;
    param_2 = "";
    goto LAB_4011aaa4;
  case '6':
    if (g_c3319 == 0) {
      uVar20 = (uint)g_c2efc;
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 == 2) {
        iVar16 = (g_c2f0c != "");
        if ((iVar16 != 0) && (iVar16 = (g_c2f00 != ""), iVar16 != 0)) {
          youtube_fetch();
          g_c3494 = bVar13;
          g_c3490 = 0x5dc;
          t_30 = String(g_c1c7c, 10);
          param_2 = t_30;
          goto LAB_4011aaa4;
        }
        goto LAB_40116864;
      }
    }
    break;
  case '7':
    if (g_c3319 == 0) {
      uVar20 = (uint)g_c2efc;
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 == 2) {
        iVar16 = (g_c2f0c != "");
        if ((iVar16 != 0) && (iVar16 = (g_c2f00 != ""), iVar16 != 0)) {
          youtube_fetch();
          g_c3494 = bVar13;
          g_c3490 = 0x5dc;
          t_30 = "\x0e" "\x10";
          param_2 = t_30;
          goto LAB_4011aaa4;
        }
        goto LAB_40116864;
      }
    }
    break;
  case '8':
    if (g_c3319 == 0) {
      uVar20 = (uint)g_c2efc;
      if (uVar20 == 0) {
        g_c3494 = 1;
        g_c3490 = uVar20;
        param_2 = "";
        goto LAB_4011aaa4;
      }
      if (uVar20 == 2) {
        iVar16 = (g_c2f0c != "");
        if ((iVar16 != 0) && (iVar16 = (g_c2f00 != ""), iVar16 != 0)) {
          youtube_fetch();
          g_c3494 = bVar13;
          g_c3490 = 0x5dc;
          t_30 = String(g_c1c78, 10);
          param_2 = t_30;
          goto LAB_4011aaa4;
        }
        goto LAB_40116864;
      }
    }
  }
LAB_4011a4c1:
  g_c3494 = bVar14;
  param_2 = "";
LAB_4011aaa4:
  return;
}
