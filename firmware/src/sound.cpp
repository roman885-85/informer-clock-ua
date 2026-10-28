// Перенесено з 2.7.1 (чернетка xlat, доведена вручну)
#include "fw.h"

// @400e3500 (FUN_400e3500)
void f_400e3500()
{
  int8_t cVar1;
  int32_t iVar2;
  int8_t cVar3;
  uint8_t uVar4;
  int32_t iVar5;
  uint8_t *puVar6;
  cVar3 = g_c19d9;
  iVar2 = g_c19cc;
  cVar1 = g_c19c8;
  puVar6 = &g_c19c8;
  if (g_c19c8 == '\0') {
    g_c19d9 = g_c19c8;
LAB_400e3539:
    *puVar6 = 1;
    return;
  }
  if (g_c19c8 != '\x01') {
    return;
  }
  if (g_c19d9 == '\0') {
    iVar5 = millis();
    puVar6 = &g_c19c6;
    if ((uint)(iVar5 - iVar2) < 200) goto LAB_400e3539;
    uVar4 = random(1, 0x13);
    myDFPlayer.playFolder(0xf, uVar4);
    delay(0x32);
    g_c19d9 = cVar1;
    g_c19d4 = millis();
    g_c19d0 = millis();
    g_c19c6 = cVar3;
  }
  return;
}
// @400e3570 (voice_hourly)
// strings: "MP3 Player - hour: "
void voice_hourly()
{
  String t_30, t_3c, t_48;  // тимчасові
  int8_t cVar1;
  uint8_t uVar2;
  uint8_t uVar3;
  uint32_t uVar4;
  int32_t iVar5;
  int32_t iVar6;
  uint32_t uVar7;
  uint32_t uVar8;
  if (g_c2f1a != g_c2f1c) {
    if (g_c19d8 == '\0') {
      g_c19d8 = '\x01';
      g_c2f1a = g_c2f1c;
      goto LAB_400e37dd;
    }
    if ((g_c2f1d == '\0') || ((g_c2f1d == '\x01' && (g_c2e70 != '\0')))) {
      myDFPlayer.playFolder(g_c2f1b, 0x32);
      delay(0x32);
    }
    g_c2f1a = g_c2f1c;
  }
  uVar8 = (uint)g_c2f21;
  uVar4 = hour();
  if (uVar8 != uVar4) {
    if (g_c2f1d == '\x01') {
      iVar5 = time_in_range(g_c2e5c,g_c2e58);
      g_c2e70 = (char)iVar5;
      if (iVar5 == 0) {
        g_c2f21 = hour();
        goto LAB_400e37dd;
      }
    }
    if (g_c19c5 == '\0') {
      g_c19cc = millis();
      g_c19c5 = '\x01';
      myDFPlayer.volume(g_c2f1c);
      delay(0x32);
    }
    f_400e3500();
    iVar5 = g_c19d4;
    if ((g_c19c6 != '\x01') && (iVar6 = millis(), 0xbe9 < (uint)(iVar6 - iVar5))) {
      if (g_bec04 == '\0') {
        myDFPlayer.playFolder(g_c2f1b, 0x1e);
        delay(0x32);
        g_bec04 = '\x01';
      }
      iVar5 = g_c19d0;
      iVar6 = millis();
      if (0xfef < (uint)(iVar6 - iVar5)) {
        g_bec04 = '\0';
        g_c19d0 = millis();
        g_c19d4 = millis();
        cVar1 = g_c19c7;
        g_c19c5 = '\0';
        if (g_c19c7 == '\0') {
          g_c19c7 = '\x01';
          g_c2f21 = hour();
          g_c19d9 = cVar1;
        }
        else if (g_c19c7 == '\x01') {
          t_48 = "MP3 Player - hour: ";
          t_30 = t_48;
          uVar7 = hour();
          t_3c = String(uVar7);
          t_30 += t_3c;
          dbg_ws_print(t_30);
          iVar5 = hour();
          uVar2 = g_c2f1b;
          if (iVar5 == 0) {
            myDFPlayer.playFolder(g_c2f1b, 0x33);
            delay(0x32);
            g_c2f21 = hour();
            g_c19d9 = '\0';
          }
          else {
            uVar3 = hour();
            myDFPlayer.playFolder(uVar2, uVar3);
            delay(0x32);
            g_c2f21 = hour();
            g_c19d9 = '\0';
          }
        }
      }
    }
  }
LAB_400e37dd:
  return;
}
// @400e37f0 (FUN_400e37f0)
void f_400e37f0()
{
  uint32_t uVar1;
  if (g_c2fb4 == '\x02') {
    digitalWrite(2, 1);
    delay(0x1e);
    digitalWrite(2, 0);
    delay(0x32);
    digitalWrite(2, 1);
    delay(0x1e);
    digitalWrite(2, 0);
    delay(0x32);
    digitalWrite(2, 1);
    delay(200);
  }
  else {
    if (g_c2fb4 == '\x03') {
      digitalWrite(2, 1);
      delay(10);
      digitalWrite(2, 0);
      delay(10);
      digitalWrite(2, 1);
      delay(10);
      digitalWrite(2, 0);
      delay(10);
      digitalWrite(2, 1);
      delay(10);
      digitalWrite(2, 0);
      delay(10);
      digitalWrite(2, 1);
      delay(10);
      digitalWrite(2, 0);
      delay(10);
      digitalWrite(2, 1);
      delay(10);
      digitalWrite(2, 0);
      delay(10);
      digitalWrite(2, 1);
      delay(10);
      digitalWrite(2, 0);
      delay(10);
      digitalWrite(2, 1);
      delay(10);
      digitalWrite(2, 0);
      delay(10);
      digitalWrite(2, 1);
      uVar1 = 10;
    }
    else {
      if (g_c2fb4 != '\x01') {
        return;
      }
      digitalWrite(2, 1);
      uVar1 = 200;
    }
    delay(uVar1);
  }
  digitalWrite(2, 0);
  delay(100);
  return;
}
// @400e391c (buzzer_hourly)
void buzzer_hourly()
{
  uint32_t uVar1;
  int32_t iVar2;
  uint32_t uVar3;
  if (g_c34a9 != '\x03') {
    return;
  }
  if (g_c19c4 == '\0') {
    g_c2f21 = hour();
    pinMode(2, OUTPUT);
    g_c19c4 = '\x01';
  }
  else if (g_c19c4 == '\x01') {
    g_c2f21 = hour();
    g_c19c4 = '\x02';
  }
  uVar3 = (uint)g_c2f21;
  uVar1 = hour();
  if (uVar3 == uVar1) {
    return;
  }
  if (g_c2f1d == '\x01') {
    iVar2 = time_in_range(g_c2e5c,g_c2e58);
    g_c2e70 = (undefined1)iVar2;
    if (iVar2 == 0) goto LAB_400e3988;
  }
  f_400e37f0();
LAB_400e3988:
  g_c2f21 = hour();
  return;
}
// @400e3a60 (FUN_400e3a60)
// strings: "Get location: error"
void f_400e3a60()
{
  String t_30;  // тимчасові
  t_30 = "Get location: error";
  dbg_ws_print(t_30);
  return;
}
// @400e3ab4: заповнити 8 полів погоди
void weather_set8(String *w, const String &a, const String &b, const String &c, const String &d, const String &e, const String &f, const String &g, const String &h) {
  w[0] = a; w[1] = b; w[2] = c; w[3] = d; w[4] = e; w[5] = f; w[6] = g; w[7] = h;
}
// @400e3afc: заповнити 7 полів погоди на завтра
void weather_set7(String *w, const String &a, const String &b, const String &c, const String &d, const String &e, const String &f, const String &g) {
  w[0] = a; w[1] = b; w[2] = c; w[3] = d; w[4] = e; w[5] = f; w[6] = g;
}
// @400e43d8 (FUN_400e43d8)
void f_400e43d8()
{
  uint32_t uVar1;
  uint32_t uStack_26;
  uint8_t uStack_23;
  uint8_t uStack_22;
  uint8_t uStack_21;
  if (g_c2fc5 == 2) {
    uVar1 = 5000;
  }
  else if (g_c2fc5 < 3) {
    uVar1 = 0x14;
    if (g_c2fc5 != 1) {
      return;
    }
  }
  else {
    if (g_c2fc5 != 3) {
      if (g_c2fc5 != 4) {
        return;
      }
      if (g_c1a94 < 0xf) {
        g_c1a90 = 0x96;
      }
      else {
        g_c1a90 = 0x28;
        if (0x3b < g_c1a94) {
          g_c1a94 = 0;
        }
      }
      uStack_23 = random(0, 0xfe);
      uStack_22 = 0xff;
      uStack_21 = 0xff;
      led_fill_all(CHSV(uStack_23, uStack_22, uStack_21));
      goto LAB_400e445a;
    }
    uVar1 = 0x32;
  }
  g_c1a90 = uVar1;
LAB_400e445a:
  led_alarm_tick();
  return;
}
// @400e466c (FUN_400e466c)
void f_400e466c()
{
  int32_t iVar1;
  int32_t iVar2;
  iVar1 = g_c1a9c;
  iVar2 = millis();
  if (599999 < (uint)(iVar2 - iVar1)) {
    g_c1a9a = 1;
    g_c1a97 = weekday();
    g_c1a89 = 1;
    g_c1a97 = weekday();
    g_c1a8d = 0;
    g_c1a89 = 1;
    display_restart_content();
  }
  return;
}
// @400e46b0 (alarm_check)
// strings: "Alarm"
void alarm_check()
{
  uint8_t uVar1;
  int32_t iVar2;
  int32_t iVar3;
  uint32_t uVar4;
  uint32_t uVar5;
  uint8_t *puVar6;
  uint32_t uVar7;
  uint32_t uVar8;
  g_c1a9c = millis();
  if ((byte)(g_c2fc7 - 4U) < 4) {
    myDFPlayer.volume(0);
    delay(0x32);
  }
  pinMode(2, OUTPUT);
  g_c1a8e = '\0';
  uVar4 = 0;
  if (g_c2fc5 != '\x02') {
    uVar4 = 0xf;
  }
  P.setIntensity(uVar4);
  g_c1a89 = '\0';
  g_c1a8c = 0;
  P.setFont(0, (MD_MAX72XX::fontType_t *)font_font_3f43c1d4);
  P.setFont(1, (MD_MAX72XX::fontType_t *)font_font_3f43c28d);
  if (1 < P.getZoneCount()) P.setCharSpacing(1, 0);
  if (P.getZoneCount() != 0) P.setCharSpacing(0, 0);
  P.displayZoneText(1, "\x06" "\x06" "\x06" "\x06", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
  P.displayZoneText(0, "\x06" "\x06" "\x06" "\x06", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
  P.displayClear();
  P.displayReset();
  P.displayAnimate();
  if (g_c34ba == '\x04') {
    uVar7 = 0;
    uVar5 = 0;
    uVar4 = 4;
    uVar8 = 4;
  }
  else {
    if (g_c34ba != '\b') goto LAB_400e4821;
    P.displayZoneText(0, "\x01" "\x02", (textPosition_t)0, 0, 0, (textEffect_t)4, (textEffect_t)4);
    P.displayZoneText(1, "\x01" "\x02", (textPosition_t)0, 0, 0, (textEffect_t)4, (textEffect_t)4);
    if (g_c34b8 != '\b') goto LAB_400e4821;
    P.setFont(3, (MD_MAX72XX::fontType_t *)font_font_3f43c1d4);
    P.displayZoneText(2, "Alarm", (textPosition_t)1, 0, 0, (textEffect_t)1, (textEffect_t)0);
    uVar8 = 0;
    uVar4 = 1;
    uVar7 = 1;
    uVar5 = 3;
  }
  P.displayZoneText(uVar5, "\x03", (textPosition_t)uVar7, 0, 0, (textEffect_t)uVar4, (textEffect_t)uVar8);
LAB_400e4821:
  P.displayAnimate();
  led_off();
  do {
    if (g_c1a89 != '\0') {
      return;
    }
    yield();
    f_400e466c();
    if ((byte)(g_c2fc7 - 4U) < 4) {
      f_400e1874();
    }
    if (g_c2fc5 != '\0') {
      f_400e43d8();
    }
    f_400e15f0();
    if ((0x2f < g_c1a8b) && (g_c1a89 == '\0')) {
      f_400e1520();
      g_c1a8b = 0;
    }
    iVar2 = millis();
LAB_400e487d:
    while (iVar3 = digitalRead(0), iVar3 == 0) {
      P.displayAnimate();
      P.displayReset();
      if (g_c34ba == '\x04') {
        if (g_c2c17 == '\0') {
          puVar6 = "\x06" "\x06" "\x06" "\x06";
        }
        else {
LAB_400e4904:
          puVar6 = "\x04";
        }
LAB_400e48c6:
        P.displayZoneText(0, puVar6, (textPosition_t)1, 0, 0, (textEffect_t)1, (textEffect_t)0);
        delay(0x1e);
        g_c2c17 = g_c2c17 ^ 1;
      }
      else if (g_c34ba == '\b') {
        if (g_c2c17 != '\0') {
          P.displayZoneText(1, "\x03", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
          goto LAB_400e4904;
        }
        P.displayZoneText(1, "\x03", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
        puVar6 = "\x06" "\x06" "\x06" "\x06";
        goto LAB_400e48c6;
      }
      P.displayAnimate();
      iVar3 = millis();
      if (1999 < (uint)(iVar3 - iVar2)) goto LAB_400e4949;
      iVar3 = millis();
      if ((0x31 < (uint)(iVar3 - iVar2)) && (iVar3 = digitalRead(0), iVar3 != 0)) {
        g_c1a9a = 1;
        g_c1a97 = weekday();
        g_c1a89 = 1;
        g_c1a97 = weekday();
        g_c1a89 = '\x01';
        g_c1a8d = 1;
        if (g_c1a8e != '\x01') {
          if ((byte)(g_c2fc7 - 4U) < 4) {
            myDFPlayer.volume(g_c2f1c);
            delay(0x32);
            myDFPlayer.stop();
          }
          goto LAB_400e49ad;
        }
      }
    }
    if ((g_c2fc7 != '\0') && (g_c1a89 == '\0')) {
      f_400e1800();
    }
  } while( true );
LAB_400e4949:
  g_c1a9a = 1;
  g_c1a97 = weekday();
  g_c1a89 = 1;
  g_c1a97 = weekday();
  g_c1a89 = '\x01';
  g_c1a8d = 0;
  if (g_c1a8e != '\x01') {
    if ((byte)(g_c2fc7 - 4U) < 4) {
      myDFPlayer.volume(g_c2f1c);
      delay(0x32);
      uVar1 = random(0x32, 0x36);
      myDFPlayer.playFolder(0x10, uVar1);
    }
LAB_400e49ad:
    f_400e18ac();
    display_restart_content();
  }
  goto LAB_400e487d;
}
// @400e4a44 (FUN_400e4a44)
void f_400e4a44()
{
  int32_t iVar1;
  int32_t iVar2;
  iVar1 = g_c175c;
  iVar2 = millis();
  if (299999 < (uint)(iVar2 - iVar1)) {
    g_c175c = millis();
    alarm_check();
  }
  return;
}
// @400e4a64 (alarm_day_match)
// strings: " alarm_day vs " " today"
void alarm_day_match()
{
  String t_30, t_3c, t_48;  // тимчасові
  uint32_t uVar1;
  uint32_t uVar2;
  uint32_t uVar4;
  int32_t iVar5;
  f_400e1480();
  if (g_c1a8d == '\x01') {
    f_400e4a44();
  }
  uVar1 = (uint)g_c1a99;
  uVar2 = hour();
  if ((uVar1 != uVar2) || (uVar1 = (uint)g_c1a98, uVar2 = minute(), uVar1 != uVar2))
  goto LAB_400e4bd9;
  if (g_c1a9a == '\0') {
    t_48 = String(g_c2fc6, 10);
    t_3c = t_48;
    t_3c += " alarm_day vs ";
    uVar4 = day();
    t_30 = String(uVar4);
    t_3c += t_30;
    t_3c += " today";
    dbg_ws_print(t_3c);
    uVar1 = (uint)g_c2fc6;
    if (uVar1 == 9) {
      iVar5 = weekday();
      if ((((iVar5 == 2) || (iVar5 = weekday(), iVar5 == 3)) ||
          (iVar5 = weekday(), iVar5 == 4)) ||
         ((iVar5 = weekday(), iVar5 == 5 || (iVar5 = weekday(), iVar5 == 6))))
      goto LAB_400e4b91;
    }
    else if (uVar1 == 10) {
      iVar5 = weekday();
      if ((iVar5 == 7) || (iVar5 = weekday(), iVar5 == 1)) goto LAB_400e4b91;
    }
    else if ((uVar1 == 8) || (uVar2 = weekday(), uVar1 == uVar2)) {
LAB_400e4b91:
      alarm_check();
    }
  }
  uVar1 = (uint)g_c1a97;
  uVar2 = weekday();
  if (uVar1 != uVar2) {
    g_c1a9a = '\0';
  }
LAB_400e4bd9:
  return;
}
// @400e4bec (FUN_400e4bec)
void alarm_tick(uint32_t param_1)
{
  int32_t iVar1;
  int32_t iVar2;
  iVar1 = g_c1730;
  if (g_c2fc8 == '\x01') {
    iVar2 = millis();
    if (param_1 <= (uint)(iVar2 - iVar1)) {
      g_c1730 = millis();
      alarm_day_match();
    }
  }
  return;
}
// @400e4c14 (FUN_400e4c14)
// strings: "=======================================================" "===================="
void f_400e4c14(int8_t param_1)
{
  String t_30;  // тимчасові
  if (param_1 == '\x01') {
    t_30 = "=======================================================";
    dbg_ws_print(t_30);
  }
  else {
    if (param_1 != '\x02') goto LAB_400e4c7e;
    t_30 = "====================";
    dbg_ws_print(t_30);
  }
LAB_400e4c7e:
  return;
}
