// Фоторезистор (GPIO34): рівень яскравості за освітленням. Перенесено з 2.7.1 (чернетка xlat, доведена вручну)
#include "fw.h"

// @400de620 (photo_brightness_read)
// strings: "Display INFO: Photoresistor data: " " , Treshold UI selected:  " " , Treshold:  " " , brightness=" " , Treshold:  " " , brightness="
void photo_brightness_read()
{
  String t_30, t_3c, t_48, t_54, t_60, t_6c, t_78, t_84, t_90;  // тимчасові
  uint32_t uVar1;
  int32_t iVar2;
  uint8_t bVar4;
  if ((g_c2ee9 != '\x01') && (g_c2f25 != '\x01')) goto LAB_400debfc;
  uVar1 = analogRead(0x22);
  g_c3284 = (uint16_t)uVar1;
  iVar2 = (uint)g_c34b2 * 0x44;
  if (g_c2f25 == '\0') {
    if ((uVar1 < 1000) || (bVar4 = 0xf, iVar2 <= (int)uVar1)) {
      if (((uVar1 - 0x3ac & 0xffff) < 0x3b) && ((int)uVar1 < iVar2)) {
        bVar4 = 0xe;
        goto LAB_400de690;
      }
      if ((((((uVar1 - 900 & 0xffff) < 0x27) && (bVar4 = 0xd, (int)uVar1 < iVar2)) ||
           (((uVar1 - 0x348 & 0xffff) < 0x27 && (bVar4 = 0xc, (int)uVar1 < iVar2)))) ||
          (((uVar1 - 0x30c & 0xffff) < 0x3b && (bVar4 = 0xb, (int)uVar1 < iVar2)))) ||
         ((((((uVar1 - 0x2e4 & 0xffff) < 0x27 && (bVar4 = 10, (int)uVar1 < iVar2)) ||
            (((((uVar1 - 0x2a8 & 0xffff) < 0x3b && (bVar4 = 9, (int)uVar1 < iVar2)) ||
              (((uVar1 - 0x280 & 0xffff) < 0x27 && (bVar4 = 8, (int)uVar1 < iVar2)))) ||
             (((uVar1 - 600 & 0xffff) < 0x27 && (bVar4 = 7, (int)uVar1 < iVar2)))))) ||
           (((uVar1 - 0x226 & 0xffff) < 0x31 && (bVar4 = 6, (int)uVar1 < iVar2)))) ||
          (((((uVar1 - 500 & 0xffff) < 0x31 && (bVar4 = 5, (int)uVar1 < iVar2)) ||
            (((uVar1 - 0x1c2 & 0xffff) < 0x31 && (bVar4 = 4, (int)uVar1 < iVar2)))) ||
           ((((uVar1 - 400 & 0xffff) < 0x31 && (bVar4 = 3, (int)uVar1 < iVar2)) ||
            (((uVar1 - 200 & 0xffff) < 199 && (bVar4 = 2, (int)uVar1 < iVar2))))))))))
      goto LAB_400de690;
      if (0x62 < (uVar1 - 100 & 0xffff)) {
        if ((0x62 < uVar1) || (bVar4 = 0, iVar2 <= (int)uVar1)) goto LAB_400de7bc;
        goto LAB_400de690;
      }
      bVar4 = 1;
      if ((int)uVar1 < iVar2) goto LAB_400de690;
LAB_400de7bc:
      if (iVar2 < (int)uVar1) {
        g_bf041 = g_c34b2;
      }
    }
    else {
LAB_400de690:
      g_bf041 = bVar4;
    }
    t_90 = "Display INFO: Photoresistor data: ";
    t_78 = t_90;
    t_84 = String(g_c3284);
    t_78 += t_84;
    t_6c = " , Treshold UI selected:  ";
    t_78 += t_6c;
    t_60 = String(g_c34b2, 10);
    t_78 += t_60;
    t_54 = " , Treshold:  ";
    t_78 += t_54;
    t_48 = String(iVar2);
    t_78 += t_48;
    t_3c = " , brightness=";
    t_78 += t_3c;
    t_30 = String(g_bf041, 10);
    t_78 += t_30;
    dbg_ws_print(t_78);
  }
  else {
    if ((uVar1 < 1000) || (iVar2 <= (int)uVar1)) {
      if (((uVar1 - 0x3ac & 0xffff) < 0x3b) && ((int)uVar1 < iVar2)) {
        g_bf041 = 0xe;
        g_c2ee3 = 0xe0;
      }
      else if (((uVar1 - 900 & 0xffff) < 0x27) && ((int)uVar1 < iVar2)) {
        g_bf041 = 0xd;
        g_c2ee3 = 0xd0;
      }
      else if (((uVar1 - 0x348 & 0xffff) < 0x27) && ((int)uVar1 < iVar2)) {
        g_bf041 = 0xc;
        g_c2ee3 = 0xc0;
      }
      else if (((uVar1 - 0x30c & 0xffff) < 0x3b) && ((int)uVar1 < iVar2)) {
        g_bf041 = 0xb;
        g_c2ee3 = 0xb0;
      }
      else if (((uVar1 - 0x2e4 & 0xffff) < 0x27) && ((int)uVar1 < iVar2)) {
        g_bf041 = 10;
        g_c2ee3 = 0xa0;
      }
      else if (((uVar1 - 0x2a8 & 0xffff) < 0x3b) && ((int)uVar1 < iVar2)) {
        g_bf041 = 9;
        g_c2ee3 = 0x90;
      }
      else if (((uVar1 - 0x280 & 0xffff) < 0x27) && ((int)uVar1 < iVar2)) {
        g_bf041 = 8;
        g_c2ee3 = 0x80;
      }
      else if (((uVar1 - 600 & 0xffff) < 0x27) && ((int)uVar1 < iVar2)) {
        g_bf041 = 7;
        g_c2ee3 = 0x70;
      }
      else if (((uVar1 - 0x226 & 0xffff) < 0x31) && ((int)uVar1 < iVar2)) {
        g_bf041 = 6;
        g_c2ee3 = 0x60;
      }
      else if (((uVar1 - 500 & 0xffff) < 0x31) && ((int)uVar1 < iVar2)) {
        g_bf041 = 5;
        g_c2ee3 = 0x50;
      }
      else if (((uVar1 - 0x1c2 & 0xffff) < 0x31) && ((int)uVar1 < iVar2)) {
        g_bf041 = 4;
        g_c2ee3 = 0x40;
      }
      else if (((uVar1 - 400 & 0xffff) < 0x31) && ((int)uVar1 < iVar2)) {
        g_bf041 = 3;
        g_c2ee3 = 0x30;
      }
      else if (((uVar1 - 200 & 0xffff) < 199) && ((int)uVar1 < iVar2)) {
        g_bf041 = 2;
        g_c2ee3 = 0x20;
      }
      else if ((uVar1 - 100 & 0xffff) < 99) {
        if ((int)uVar1 < iVar2) {
          g_bf041 = 1;
          g_c2ee3 = 0x10;
        }
        else {
LAB_400dea3d:
          if (iVar2 < (int)uVar1) {
            g_bf041 = g_c34b2;
            goto LAB_400dea43;
          }
        }
      }
      else {
        if ((0x62 < uVar1) || (iVar2 <= (int)uVar1)) goto LAB_400dea3d;
        g_bf041 = 0;
        g_c2ee3 = 8;
      }
    }
    else {
      g_bf041 = 0xf;
LAB_400dea43:
      g_c2ee3 = 0xff;
    }
    t_90 = "Display INFO: Photoresistor data: ";
    t_78 = t_90;
    t_84 = String(g_c3284);
    t_78 += t_84;
    t_6c = " , Treshold UI selected:  ";
    t_78 += t_6c;
    t_60 = String(g_c34b2, 10);
    t_78 += t_60;
    t_54 = " , Treshold:  ";
    t_78 += t_54;
    t_48 = String(iVar2);
    t_78 += t_48;
    t_3c = " , brightness=";
    t_78 += t_3c;
    t_30 = String(g_bf041, 10);
    t_78 += t_30;
    dbg_ws_print(t_78);
  }
LAB_400debfc:
  return;
}
