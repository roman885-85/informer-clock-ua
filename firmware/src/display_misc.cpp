// Перенесено з 2.7.1 (чернетка xlat, доведена вручну)
// EN: Display helpers: night/day schedule, brightness modes, messages, typography fixes (from 2.7.1).
#include "fw.h"

// @400e1480 (FUN_400e1480)
void f_400e1480()
{
  String t_30;  // тимчасові
  int32_t iVar1;
  if (g_c2fc4 == '\0') {
    iVar1 = g_c2fb8.length();
    t_30 = g_c2fb8.substring(0, iVar1 + -3);
    g_c1a99 = t_30.toInt();
    t_30 = g_c2fb8.substring(3, 5);
    g_c1a98 = t_30.toInt();
    g_c2fc4 = '\x01';
  }
  return;
}
// @400e1520 (FUN_400e1520)
void f_400e1520()
{
  uint8_t uVar1;
  uint8_t *puVar2;
  uint32_t uVar3;
  uint32_t uVar4;
  uint32_t uVar5;
  g_c1a88 = g_c1a88 ^ 1;
  if (g_c1a88 == 0) {
    if (g_c34ba != '\x04') {
      if (g_c34ba != '\b') goto LAB_400e15d9;
      P.displayZoneText(0, "\x01" "\x02", (textPosition_t)2, 0, 0, (textEffect_t)5, (textEffect_t)5);
      uVar5 = 5;
      uVar4 = 5;
      uVar3 = 2;
      goto LAB_400e15cf;
    }
    uVar5 = 5;
    uVar4 = 5;
    uVar3 = 2;
    puVar2 = "\x03";
    uVar1 = 0;
  }
  else if (g_c34ba == '\x04') {
    uVar3 = 0;
    puVar2 = "\x03";
    uVar1 = 0;
    uVar4 = 4;
    uVar5 = 4;
  }
  else {
    if (g_c34ba != '\b') goto LAB_400e15d9;
    P.displayZoneText(0, "\x01" "\x02", (textPosition_t)0, 0, 0, (textEffect_t)4, (textEffect_t)4);
    uVar5 = 4;
    uVar4 = 4;
    uVar3 = 0;
LAB_400e15cf:
    puVar2 = "\x01" "\x02";
    uVar1 = 1;
  }
  P.displayZoneText(uVar1, puVar2, (textPosition_t)uVar3, 0, 0, (textEffect_t)uVar4, (textEffect_t)uVar5);
LAB_400e15d9:
  P.displayReset();
  g_bec0d = random(8, 0x23);
  return;
}
// @400e15f0 (FUN_400e15f0)
void f_400e15f0()
{
  int32_t iVar1;
  int32_t iVar2;
  uint32_t uVar3;
  iVar1 = g_c1768;
  uVar3 = (uint)g_bec0d;
  iVar2 = millis();
  if (uVar3 <= (uint)(iVar2 - iVar1)) {
    g_c1768 = millis();
    P.displayAnimate();
    g_c1a8b = g_c1a8b + '\x01';
  }
  return;
}
// @400e1624 (FUN_400e1624)
void f_400e1624()
{
  uint32_t uVar1;
  if (g_c2fc7 == 5) {
    uVar1 = 45000;
  }
  else if (g_c2fc7 < 6) {
    uVar1 = 50000;
    if (g_c2fc7 != 4) {
      return;
    }
  }
  else if (g_c2fc7 == 6) {
    uVar1 = 150000;
  }
  else {
    uVar1 = 29000;
    if (g_c2fc7 != 7) {
      return;
    }
  }
  g_c1aa0 = uVar1;
  return;
}
// @400e1660 (FUN_400e1660)
void f_400e1660()
{
  if (g_c2fc7 == '\x02') {
    switch(g_c1a95) {
    case 0:
      g_c1a95 = 1;
      g_c1a96 = 1;
      g_c1aa0 = 10;
      return;
    case 1:
      g_c1a95 = 2;
      g_c1a96 = 0;
      g_c1aa0 = 10;
      return;
    case 2:
      g_c1a95 = 3;
      break;
    case 3:
      g_c1a95 = 4;
      g_c1a96 = 0;
      g_c1aa0 = 10;
      return;
    case 4:
      g_c1a95 = 5;
      break;
    case 5:
      g_c1a95 = 6;
      g_c1a96 = 0;
      g_c1aa0 = 10;
      return;
    case 6:
      g_c1a95 = 7;
      break;
    case 7:
      g_c1a95 = 8;
      break;
    case 8:
      g_c1a95 = 9;
      g_c1a96 = 0;
      g_c1aa0 = 10;
      return;
    case 9:
      g_c1a95 = 10;
      break;
    case 10:
      g_c1a95 = 0xb;
      g_c1a96 = 0;
      g_c1aa0 = 10;
      return;
    case 0xb:
      g_c1a95 = 0xc;
      break;
    case 0xc:
      g_c1a95 = 0xd;
      g_c1a96 = 0;
      g_c1aa0 = 10;
      return;
    case 0xd:
      g_c1a95 = 0xe;
      break;
    case 0xe:
      g_c1a95 = 0xf;
      g_c1a96 = 0;
      g_c1aa0 = 100;
      return;
    default:
      goto LAB_400e17fd;
    }
    g_c1a96 = 1;
    g_c1aa0 = 10;
    return;
  }
  if (g_c2fc7 != '\x03') {
    if (g_c2fc7 != '\x01') {
      return;
    }
    switch(g_c1a95) {
    case 0:
LAB_400e17f4:
      g_c1a95 = 1;
      g_c1a96 = 1;
      g_c1aa0 = 0x46;
      return;
    case 1:
      g_c1a95 = 2;
      g_c1a96 = 0;
      g_c1aa0 = 0x32;
      return;
    case 2:
      g_c1a95 = 3;
      break;
    case 3:
      g_c1a95 = 4;
      g_c1a96 = 0;
      g_c1aa0 = 0x32;
      return;
    case 4:
      g_c1a95 = 5;
      break;
    case 5:
      g_c1a95 = 6;
      g_c1a96 = 0;
      g_c1aa0 = 0x32;
      return;
    case 6:
      g_c1a95 = 7;
      break;
    case 7:
      g_c1a95 = 8;
      g_c1a96 = 0;
      g_c1aa0 = 300;
      return;
    default:
LAB_400e17fd:
      g_c1a95 = 0;
      return;
    }
    g_c1a96 = 1;
    g_c1aa0 = 0x46;
    return;
  }
  switch(g_c1a95) {
  case 0:
    goto LAB_400e17f4;
  case 1:
    g_c1a95 = 2;
    g_c1a96 = 0;
    g_c1aa0 = 100;
    return;
  case 2:
    g_c1a95 = 3;
    break;
  case 3:
    g_c1a95 = 4;
    g_c1a96 = 0;
    g_c1aa0 = 0x23;
    return;
  case 4:
    g_c1a95 = 5;
    break;
  case 5:
    g_c1a95 = 6;
    g_c1a96 = 0;
    g_c1aa0 = 0x23;
    return;
  case 6:
    g_c1a95 = 7;
    break;
  case 7:
    g_c1a95 = 8;
    g_c1a96 = 0;
    g_c1aa0 = 0x96;
    return;
  case 8:
    g_c1a95 = 9;
    break;
  case 9:
    g_c1a95 = 10;
    g_c1a96 = 0;
    g_c1aa0 = 300;
    return;
  case 10:
    g_c1a95 = 0xb;
    g_c1a96 = 1;
    g_c1aa0 = 0x46;
    return;
  case 0xb:
    g_c1a95 = 0xc;
    g_c1a96 = 0;
    g_c1aa0 = 100;
    return;
  case 0xc:
    g_c1a95 = 0xd;
    g_c1a96 = 1;
    g_c1aa0 = 0x8c;
    return;
  case 0xd:
    g_c1a95 = 0xe;
    g_c1a96 = 0;
    g_c1aa0 = 100;
    return;
  default:
    goto LAB_400e17fd;
  }
  g_c1a96 = 1;
  g_c1aa0 = 0x23;
  return;
}
// @400e1800 (FUN_400e1800)
void f_400e1800()
{
  uint32_t uVar1;
  int32_t iVar2;
  int32_t iVar3;
  iVar2 = g_c1aa4;
  uVar1 = g_c1aa0;
  if ((byte)(g_c2fc7 - 4U) < 4) {
    iVar3 = millis();
    if (uVar1 <= (uint)(iVar3 - iVar2)) {
      f_400e1624();
      g_c1aa4 = millis();
      myDFPlayer.playFolder(0x10, g_c2fc7);
      return;
    }
  }
  else if ((byte)(g_c2fc7 - 1U) < 3) {
    iVar3 = millis();
    if (uVar1 <= (uint)(iVar3 - iVar2)) {
      f_400e1660();
      g_c1aa4 = millis();
      if ((g_c1a96 != '\0') && (g_c1a96 != '\x01')) {
        return;
      }
      digitalWrite(2, g_c1a96);
    }
  }
  return;
}
// @400e1874 (FUN_400e1874)
void f_400e1874()
{
  int32_t iVar1;
  int32_t iVar2;
  iVar1 = g_c1758;
  iVar2 = millis();
  if (1999 < (uint)(iVar2 - iVar1)) {
    g_c1758 = millis();
    if (g_c1a8c < 0x1f) {
      myDFPlayer.volume(g_c1a8c);
      g_c1a8c = g_c1a8c + 2;
    }
  }
  return;
}
// @400e18ac (FUN_400e18ac)
void f_400e18ac()
{
  int32_t iVar1;
  uint8_t *puVar2;
  uint32_t uVar3;
  iVar1 = 0x14;
  if (g_c34ba != '\b') {
    do {
      if (g_c2c17 == '\0') {
        puVar2 = "\x06" "\x06" "\x06" "\x06";
      }
      else if (g_c1a8d == '\x01') {
        puVar2 = "\x07";
      }
      else {
        puVar2 = "\t";
      }
      P.displayZoneText(0, puVar2, (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
      g_c2c17 = g_c2c17 ^ 1;
      P.displayAnimate();
      iVar1 = iVar1 + -1;
      delay(100);
    } while (iVar1 != 0);
    return;
  }
  do {
    if (g_c2c17 == '\0') {
      P.displayZoneText(1, "\x06" "\x06" "\x06" "\x06", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
      uVar3 = 0;
      puVar2 = "\x06" "\x06" "\x06" "\x06";
    }
    else if (g_c1a8d == '\x01') {
      P.displayZoneText(1, "\x05", (textPosition_t)0, 0, 0, (textEffect_t)1, (textEffect_t)0);
      uVar3 = 0;
      puVar2 = "\x05";
    }
    else {
      P.displayZoneText(1, "\x07", (textPosition_t)1, 0, 0, (textEffect_t)1, (textEffect_t)0);
      uVar3 = 1;
      puVar2 = "\x08";
    }
    P.displayZoneText(0, puVar2, (textPosition_t)uVar3, 0, 0, (textEffect_t)1, (textEffect_t)0);
    g_c2c17 = g_c2c17 ^ 1;
    P.displayAnimate();
    iVar1 = iVar1 + -1;
    delay(100);
  } while (iVar1 != 0);
  return;
}
// @400e1a0c (FUN_400e1a0c)
// strings: "null" "NULL"
void str_or_empty(String &param_1, const String &param_2)
{
  int32_t iVar1;
  iVar1 = (param_2 == "");
  if (((iVar1 == 0) && (iVar1 = (param_2 == "null"), iVar1 == 0)) &&
     (iVar1 = (param_2 == "NULL"), iVar1 == 0)) {
    param_1 = param_2;
    return;
  }
  param_1 = "";
  return;
}
// @400e1a48: заміна типографських символів у тексті з інтернету
void text_typography_fix(String &out, String s) {
  s.replace("’", "\'"); s.replace("“", "\'"); s.replace("”", "\'"); s.replace("`", "\'"); s.replace("‘", "\'"); s.replace("„", "\'");
  s.replace("•", "-"); s.replace("»", "\'"); s.replace("«", "\'"); s.replace("—", "-"); s.replace("…", "..."); s.replace("\\n", " ");
  s.replace((char)10, (char)0x20);
  s.replace((char)0xd, (char)0x20);
  out = s;
}
// @400e1da4 (display_schedule_wake_shutdown)
// strings: "Display: wake up" "Display: wake up" "Display: shutdown (schedule)"
void display_schedule_wake_shutdown()
{
  String t_30;  // тимчасові
  uint8_t uVar1;
  uint8_t *puVar2;
  if (g_c34b1 == '\0') {
    if (g_c1a28 == '\0') goto LAB_400e1ec1;
    P.setIntensity(0);
    P.displayShutdown(0);
    P.setIntensity(0);
    t_30 = "Display: wake up";
    dbg_ws_print(t_30);
LAB_400e1e4c:
    fx_bright_actual(0);                         // нове: після пробудження — плавно від 0 до рівня за світлом
    uVar1 = 0;
    g_c1a28 = '\0';
    g_c34b4 = 0x18;
    g_c34a2 = 0;
    g_c34a1 = 0;
    puVar2 = &g_c34a0;
  }
  else {
    if (g_c34b1 != '\x01') goto LAB_400e1ec1;
    if (g_c2e71 != '\0') {
      if (g_c1a28 == '\0') goto LAB_400e1ec1;
      P.setIntensity(0);
      P.displayShutdown(0);
      P.setIntensity(0);
      t_30 = "Display: wake up";
      dbg_ws_print(t_30);
      goto LAB_400e1e4c;
    }
    if (g_c1a28 != '\0') goto LAB_400e1ec1;
    P.displayShutdown(1);
    t_30 = "Display: shutdown (schedule)";
    dbg_ws_print(t_30);
    puVar2 = &g_c1a28;
    uVar1 = 1;
  }
  *puVar2 = uVar1;
LAB_400e1ec1:
  return;
}
// @400e1ed4 (display_brightness_tick)
// strings: "Display: shutdown"
void display_brightness_tick(uint32_t param_1)
{
  String t_30;  // тимчасові
  int32_t iVar1;
  int32_t iVar2;
  uint8_t *puVar3;
  iVar1 = g_c1754;
  if (g_c3080 == '\0') {
    iVar2 = millis();
    if (param_1 <= (uint)(iVar2 - iVar1)) {
      g_c1754 = millis();
      if (g_c2ee9 == '\x01') {
        display_schedule_wake_shutdown();
        // виправлено: у автора тут P.setIntensity(g_bf041) — стрибок на кілька рівнів; тепер плавно (bright.cpp)
        g_c34b4 = '\x18';
      }
      else if (g_c2ee9 == '\0') {
        display_schedule_wake_shutdown();
        if (g_c34b4 != g_c2ee8) {
          P.setIntensity(g_c2ee8);
          g_c34b4 = g_c2ee8;
        }
      }
      else if ((g_c2ee9 == '\x02') && (g_c2f20 == '\x01')) {
        g_c34b4 = '\x18';
        g_c2f20 = 'c';
        if (g_c2e71 == '\0') {
          if (g_c34b1 == '\0') {
            display_schedule_wake_shutdown();
          }
          else if ((g_c34b1 == '\x01') && (g_c1a28 == '\0')) {
            P.displayShutdown(1);
            t_30 = "Display: shutdown";
            dbg_ws_print(t_30);
            g_c1a28 = '\x01';
          }
          puVar3 = &g_c2ee6;
        }
        else {
          display_schedule_wake_shutdown();
          puVar3 = &g_c2ee7;
        }
        P.setIntensity(*puVar3);
      }
    }
  }
  else {
    P.displayShutdown(1);
  }
  return;
}
// @400e3b3c (content_cleanup_30s)
void content_cleanup_30s(uint32_t param_1)
{
  String t_30;  // тимчасові
  int32_t iVar1;
  int32_t iVar2;
  iVar1 = g_c172c;
  iVar2 = millis();
  if (param_1 <= (uint)(iVar2 - iVar1)) {
    g_c172c = millis();
    if (g_c3139 != '\x01') {
      t_30 = "";
      g_c1aec = t_30;
      g_bf048 = NAN;
    }
    if (g_c3138 != '\x01') {
      t_30 = "";
      g_c1ad8 = t_30;
      g_bf044 = NAN;
    }
    if (g_c2eea == '\0') {
      t_30 = "";
      g_c1ac4 = t_30;
    }
    if (g_c30f8 != '\x01') {
      t_30 = "";
      g_c30fc = t_30;
    }
    if (g_c2ffc != '\x01') {
      t_30 = "";
      g_c2fe4 = t_30;
      t_30 = "";
      g_c2ff0 = t_30;
    }
    if (g_c3157 != 1) {
      t_30 = "";
      g_c1a6c = t_30;
    }
    if (g_c3157 != 2) {
      t_30 = "";
      g_c1a54 = t_30;
      t_30 = "";
      g_c1a48 = t_30;
      weather_clear8(g_c31c4);
      t_30 = "";
      g_c3170[0] = t_30;
      t_30 = "";
      g_c317c = t_30;
      t_30 = "";
      g_c3188 = t_30;
      t_30 = "";
      g_c3194 = t_30;
      t_30 = "";
      g_c31a0 = t_30;
      t_30 = "";
      g_c31ac = t_30;
      t_30 = "";
      g_c31b8 = t_30;
    }
    if (g_c3157 != 3) {
      t_30 = "";
      g_c1a34 = t_30;
    }
    if ((g_c3157 & 0xfd) != 1) {
      weather_clear8(g_c3224);
    }
  }
  return;
}
// @400e3e5c (FUN_400e3e5c)
// strings: "            "
void f_400e3e5c()
{
  String t_30, t_3c, t_48, t_54, t_60, t_6c;  // тимчасові
  uint8_t uVar1;
  uint8_t bVar2;
  int32_t iVar3;
  uint8_t *puVar4;
  uint32_t uVar5;
  int32_t iVar6;
  int8_t cVar7;
  int8_t cVar8;
  int8_t cVar9;
  uint32_t uVar10;
  uint8_t *puVar11;
  cVar7 = g_c1728;
  if (g_c1728 != '\0') goto LAB_400e41e5;
  t_6c = g_c3004;
  t_60 = "";
  g_c3004 = t_60;
  g_c3001 = cVar7;
  iVar3 = (t_6c == "");
  if (iVar3 == 0) {
    g_c1728 = 1;
    if (g_c2ee9 == '\0') {
      puVar4 = &g_c2ee8;
    }
    else {
      puVar4 = &g_bf041;
    }
    uVar1 = *puVar4;
    if (g_c2f19 == '\x01') {
      myDFPlayer.volume(g_c2f1c);
      myDFPlayer.playFolder(0xf, 1);
    }
    P.displayShutdown(0);
    P.displayClear();
    bVar2 = g_c34bc->getDeviceCount();
    g_c34bc->control((MD_MAX72XX::controlRequest_t)2, 5);
    cVar7 = '\x02';
    do {
      g_c34bc->control((MD_MAX72XX::controlRequest_t)10, 0);
      g_c34bc->clear();
      for (uVar10 = 0; (uVar10 & 0xffff) < (uint)bVar2 << 3; uVar10 = uVar10 + 1) {
        iVar3 = 0;
        do {
          if ((iVar3 + uVar10 & 1) == 0) {
            g_c34bc->setPoint(iVar3, uVar10, 1);
          }
          iVar3 = iVar3 + 1;
        } while (iVar3 != 8);
      }
      g_c34bc->control((MD_MAX72XX::controlRequest_t)10, 1);
      delay(0xaa);
      g_c34bc->clear();
      delay(0x82);
      cVar7 = cVar7 + -1;
    } while (cVar7 != '\0');
    P.setIntensity(uVar1);
    g_c34a3 = 99;
    if (g_c2eef == '\x04') {
      puVar11 = font_sys_lang_ro;
LAB_400e3fd3:
      P.setFont(0, (MD_MAX72XX::fontType_t *)puVar11);
      g_c34a3 = 4;
    }
    else {
      if (g_c2eef == '\x06') {
        puVar11 = font_sys_lang_pl;
        goto LAB_400e3fd3;
      }
      sys_font_apply();
    }
    memset((void *)g_c1628,0,0x100);
    t_54 = "            ";
    t_48 = t_54;
    t_48 += t_6c;
    t_48 += "            ";
    utf8_to_led(t_48,g_c1628,0x100);
    cVar7 = '\x05';
    if (g_bf04d < 0x46) {
      cVar7 = 'F' - g_bf04d;
    }
    cVar9 = g_c2f18;
    if (g_c2f18 == '\0') {
      cVar9 = '\x01';
    }
    cVar8 = '\0';
    do {
      P.displayClear(0);
      P.displayZoneText(0, g_c1628, (textPosition_t)0, cVar7, 0, (textEffect_t)4, (textEffect_t)4);
      P.displayReset(0);
      iVar3 = millis();
      while (iVar6 = P.getZoneStatus(0), iVar6 == 0) {
        loop_guard_feed();                       // нове: повідомлення з повторами може йти кілька хвилин
        P.displayAnimate();
        iVar6 = millis();
        if (25000 < (uint)(iVar6 - iVar3)) break;
        yield();
      }
      cVar8 = cVar8 + '\x01';
    } while (cVar9 != cVar8);
    P.displayClear();
    g_c34bc->control((MD_MAX72XX::controlRequest_t)10, 1);
    g_c34bc->clear();
    g_c34b4 = 0x18;
    g_c34a2 = 0;
    g_c34a1 = 0;
    g_c34a0 = 0;
    g_c34a8 = 0;
    g_c34a7 = 0;
    g_c34a6 = 0;
    g_c34a3 = 0xc;
    g_c3471 = 0;
    g_c34af = 0;
    g_c3001 = '\0';
    // Виправлено: повідомлення вмикає погашене табло (displayShutdown(0) вище), а гасити знову автор забув —
    // уночі за розкладом табло після повідомлення світило до ранку (розклад гасить лише при переході «день → ніч»).
    if (g_c1a28 != '\0' || g_c3080 != '\0') P.displayShutdown(1);
    t_3c = "";
    g_c3004 = t_3c;
    g_c241c = 0;
    t_30 = "";
    g_c2420 = t_30;
    g_c1728 = '\0';
  }
LAB_400e41e5:
  return;
}
// @400e4200 (FUN_400e4200)
void f_400e4200()
{
  if (g_c3001 != '\0') {
    f_400e3e5c();
  }
  return;
}
// @400e4210 (FUN_400e4210)
void f_400e4210()
{
  if (g_bed1c == '\x01') {
    mqtt_connect();
  }
  return;
}
// @400e4c90 (FUN_400e4c90)
void f_400e4c90()
{
  uint8_t uVar1;
  uint32_t uVar2;
  if (g_c34ba != '\b') {
    return;
  }
  if (g_c34b8 != '\b') {
    return;
  }
  if (g_c15d4 == g_c34b7) {
    return;
  }
  if (g_c3471 == '\0') {
    if (g_c34b7 == '\0') {
      P.setZone(0, 0, 7);
      P.setZone(1, 0xc, 0xf);
      uVar2 = 0xb;
      uVar1 = 8;
    }
    else {
      if (g_c34b7 != '\x01') goto LAB_400e4da8;
      P.setZone(0, 0, 7);
      P.setZone(1, 8, 0xb);
      uVar2 = 0xf;
      uVar1 = 0xc;
    }
    P.setZone(2, uVar1, uVar2);
    uVar2 = 0x11;
    uVar1 = 0x10;
  }
  else if (g_c34b7 == '\0') {
    P.setZone(0, 4, 7);
    P.setZone(1, 0xc, 0xf);
    P.setZone(2, 8, 0xb);
    uVar2 = 3;
    uVar1 = 0;
  }
  else {
    if (g_c34b7 != '\x01') goto LAB_400e4da8;
    P.setZone(0, 0, 3);
    P.setZone(1, 8, 0xb);
    P.setZone(2, 0xc, 0xf);
    uVar2 = 7;
    uVar1 = 4;
  }
  P.setZone(3, uVar1, uVar2);
LAB_400e4da8:
  g_c15d4 = g_c34b7 != '\0';
  return;
}
// @400e4db8 (night_mode_tick)
// strings: "NightMode clock=" "night" "day" "  bright=" "night" "day" "  sound=" "night" "day"
void night_mode_tick(uint32_t param_1)
{
  String t_30, t_3c, t_48, t_54, t_60;  // тимчасові
  int32_t iVar1;
  int32_t iVar2;
  char *pcVar3;
  iVar1 = g_c15d0;
  iVar2 = millis();
  if (param_1 <= (uint)(iVar2 - iVar1)) {
    g_c15d0 = millis();
    g_c2e72 = time_in_range(g_c2e6c,g_c2e68);
    g_c2e71 = time_in_range(g_c2e64,g_c2e60);
    g_c2e70 = time_in_range(g_c2e5c,g_c2e58);
    if (!fx_time_known()) { g_c2e72 = 1; g_c2e71 = 1; }   // виправлено: час невідомий (1970) — табло як удень
    t_60 = "NightMode clock=";
    t_48 = t_60;
    pcVar3 = "day";
    if (g_c2e72 == '\0') {
      pcVar3 = "night";
    }
    t_54 = pcVar3;
    t_48 += t_54;
    t_48 += "  bright=";
    pcVar3 = "day";
    if (g_c2e71 == '\0') {
      pcVar3 = "night";
    }
    t_3c = pcVar3;
    t_48 += t_3c;
    t_48 += "  sound=";
    pcVar3 = "day";
    if (g_c2e70 == '\0') {
      pcVar3 = "night";
    }
    t_30 = pcVar3;
    t_48 += t_30;
    dbg_ws_print(t_48);
    if (g_c3319 == '\0') {
      g_bed1c = 1;
    }
  }
  return;
}

// @400d2a60: очистити 8 полів погоди
void weather_clear8(String *w) { for (int i = 0; i < 8; i++) w[i] = String(""); }
