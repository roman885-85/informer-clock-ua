// Конфігурація: застосування ключів до змінних, заводські значення, міграція (перенесено з 2.7.1)
// EN: Configuration: applies config.json keys to variables, factory defaults, migration (from 2.7.1).
#include "fw.h"

// strings: "Cwi" "chds" "chdh" "chbm" "chO" "chZ" "adw" "al1" "host" "port" "user" "mpass"
void config_apply_all()  // @4011c684
{
  String t_108, t_114, t_120, t_12c, t_138, t_144, t_150, t_15c, t_168, t_174, t_180, t_18c, t_198, t_1a4, t_1b0, t_1bc, t_1c8, t_1d4, t_1e0, t_1ec, t_1f8, t_204, t_210, t_21c, t_228, t_234, t_240, t_24c, t_258, t_264, t_270, t_27c, t_288, t_294, t_2a0, t_2ac, t_2b8, t_2c4, t_2d0, t_2dc, t_2e8, t_2f4, t_30, t_300, t_30c, t_318, t_324, t_330, t_33c, t_348, t_354, t_360, t_36c, t_378, t_384, t_390, t_39c, t_3a8, t_3b4, t_3c, t_3c0, t_3cc, t_3d8, t_3e4, t_3f0, t_3fc, t_408, t_414, t_420, t_42c, t_438, t_444, t_450, t_45c, t_468, t_474, t_48, t_480, t_48c, t_498, t_4a4, t_4b0, t_4bc, t_4c8, t_4d4, t_4e0, t_4ec, t_4f8, t_504, t_510, t_51c, t_528, t_534, t_54, t_540, t_54c, t_558, t_564, t_570, t_57c, t_588, t_594, t_5a0, t_5ac, t_5b8, t_5c4, t_5d0, t_5dc, t_5e8, t_5f4, t_60, t_600, t_60c, t_618, t_624, t_630, t_63c, t_648, t_654, t_660, t_66c, t_678, t_684, t_690, t_69c, t_6a8, t_6b4, t_6c, t_6c0, t_6cc, t_6d8, t_6e4, t_6f0, t_6fc, t_708, t_714, t_720, t_72c, t_738, t_744, t_750, t_75c, t_768, t_774, t_78, t_780, t_78c, t_798, t_7a4, t_7b0, t_7bc, t_84, t_90, t_9c, t_a8, t_b4, t_c0, t_cc, t_d8, t_e4, t_f0, t_fc;  // тимчасові
  int32_t iVar1;
  uint32_t uVar2;
  uint32_t uVar3;
  uint8_t uVar4;
  int8_t cVar5;
  JsonDocument t_7fc;
  deserializeJson(t_7fc, g_c2ef0);
  t_7bc = t_7fc["Cwi"].as<String>();
  g_c33f8 = t_7bc.toInt();
  t_7b0 = t_7fc["chds"].as<String>();
  g_c32ed = t_7b0.toInt();
  t_7a4 = t_7fc["chdh"].as<String>();
  g_c32ec = t_7a4.toInt();
  t_798 = t_7fc["chbm"].as<String>();
  g_c32eb = t_798.toInt();
  t_78c = t_7fc["chO"].as<String>();
  g_c32ea = t_78c.toInt();
  t_780 = t_7fc["chZ"].as<String>();
  g_c32e9 = t_780.toInt();
  t_774 = t_7fc["adw"].as<String>();
  g_c3154 = t_774.toInt();
  t_768 = t_7fc["al1"].as<String>();
  g_c2fb8 = t_768;
  t_75c = t_7fc["host"].as<String>();
  g_c30b8 = t_75c;
  t_750 = t_7fc["port"].as<String>();
  g_c30ac = t_750;
  t_744 = t_7fc["user"].as<String>();
  g_c30a0 = t_744;
  t_738 = t_7fc["mpass"].as<String>();
  g_c3094 = t_738;
  t_72c = t_7fc["ha_en"].as<String>();
  g_c3090 = t_72c.toInt();
  t_720 = t_7fc["ha_name"].as<String>();
  g_c3084 = t_720;
  t_714 = t_7fc["secwn"].as<String>();
  g_c33bc = t_714;
  t_708 = t_7fc["secwp"].as<String>();
  g_c33b0 = t_708;
  t_6fc = t_7fc["nApi"].as<String>();
  g_c2fd8 = t_6fc;
  t_6f0 = t_7fc["nId"].as<String>();
  g_c2fcc = t_6f0;
  t_6e4 = t_7fc["AuN"].as<String>();
  g_c33a4 = t_6e4;
  t_6d8 = t_7fc["AuP"].as<String>();
  g_c3398 = t_6d8;
  t_6cc = t_7fc["ssid"].as<String>();
  g_c33ec = t_6cc;
  t_6c0 = t_7fc["pass"].as<String>();
  g_c33e0 = t_6c0;
  t_6b4 = t_7fc["curr_a"].as<String>();
  g_c312c = t_6b4;
  t_6a8 = t_7fc["curr_b"].as<String>();
  g_c3120 = t_6a8;
  t_69c = t_7fc["btc_VAL"].as<String>();
  g_c3114 = t_69c;
  t_690 = t_7fc["tLangc"].as<String>();
  g_c3108 = t_690;
  t_684 = t_7fc["ntp_srv"].as<String>();
  g_c3434 = t_684;
  t_678 = t_7fc["nz"].as<String>();
  g_c342c = t_678.toInt();
  t_66c = t_7fc["tz"].as<String>();
  g_c3430 = t_66c.toFloat();
  t_660 = t_7fc["t_m"].as<String>();
  g_c344c = t_660;
  t_654 = t_7fc["d_m"].as<String>();
  g_c3440 = t_654;
  t_648 = t_7fc["d1_rem"].as<String>();
  g_c2ec8 = t_648;
  t_63c = t_7fc["d2_rem"].as<String>();
  g_c2ebc = t_63c;
  t_630 = t_7fc["ApiKey"].as<String>();
  g_c3148 = t_630;
  t_624 = t_7fc["ApiKey2"].as<String>();
  g_c313c = t_624;
  t_618 = t_7fc["apinews"].as<String>();
  g_c30e0 = t_618;
  t_60c = t_7fc["ayt"].as<String>();
  g_c2f0c = t_60c;
  t_600 = t_7fc["Yid"].as<String>();
  g_c2f00 = t_600;
  t_5f4 = t_7fc["Cat_news"].as<String>();
  g_c30d4 = t_5f4;
  t_5e8 = t_7fc["descr_n"].as<String>();
  g_c30c6 = t_5e8.toInt();
  t_5dc = t_7fc["MQ_ds"].as<String>();
  g_c3070 = t_5dc;
  t_5d0 = t_7fc["MQ_DHT"].as<String>();
  g_c3064 = t_5d0;
  t_5c4 = t_7fc["MQ_BM"].as<String>();
  g_c3058 = t_5c4;
  t_5b8 = t_7fc["MQ_O"].as<String>();
  g_c304c = t_5b8;
  t_5ac = t_7fc["MQZ"].as<String>();
  g_c3040 = t_5ac;
  config_apply_mqtt_texts(t_7fc);
  t_5a0 = t_7fc["text"].as<String>();
  g_c3034 = t_5a0;
  t_594 = t_7fc["text2"].as<String>();
  g_c3028 = t_594;
  t_588 = t_7fc["text3"].as<String>();
  g_c301c = t_588;
  t_57c = t_7fc["text4"].as<String>();
  g_c3010 = t_57c;
  t_570 = t_7fc["textds"].as<String>();
  g_c32b8 = t_570;
  t_564 = t_7fc["txbmp"].as<String>();
  g_c32ac = t_564;
  t_558 = t_7fc["textdht"].as<String>();
  g_c32a0 = t_558;
  t_54c = t_7fc["txO"].as<String>();
  g_c3294 = t_54c;
  t_540 = t_7fc["txA"].as<String>();
  g_c3288 = t_540;
  t_534 = t_7fc["lat"].as<String>();
  g_c316c = t_534.toFloat();
  t_528 = t_7fc["lon"].as<String>();
  g_c3168 = t_528.toFloat();
  t_51c = t_7fc["cl_sprite"].as<String>();
  g_c3495 = t_51c.toInt();
  t_510 = t_7fc["un"].as<String>();
  g_c3159 = t_510.toInt();
  t_504 = t_7fc["myci"].as<String>();
  g_c315c = t_504;
  t_4f8 = t_7fc["tomorrow_w"].as<String>();
  g_c3158 = t_4f8.toInt();
  if (g_c3154 == 0) {
    g_c19b0 = 0;
  }
  else if ((g_c3154 == 1) && (g_c2d29 != '\0')) {
    FxStatusScope fxs(FXS_DATA);                 // нове: стан на табло під час запуску
    geolocate_ip();
  }
  t_4ec = t_7fc["tempds"].as<String>();
  g_c32e8 = t_4ec.toInt();
  t_4e0 = t_7fc["tempdht"].as<String>();
  g_c32e7 = t_4e0.toInt();
  t_4d4 = t_7fc["tempdht_ch"].as<String>();
  g_c32e6 = t_4d4.toInt();
  t_4c8 = t_7fc["tbmp"].as<String>();
  g_c32e5 = t_4c8.toInt();
  t_4bc = t_7fc["bebp"].as<String>();
  g_c2ee5 = t_4bc.toInt();
  t_4b0 = t_7fc["tcO"].as<String>();
  g_c32dc = t_4b0.toFloat();
  t_4a4 = t_7fc["tcorrds"].as<String>();
  g_c32d8 = t_4a4.toFloat();
  t_498 = t_7fc["tcbmp"].as<String>();
  g_c32d4 = t_498.toFloat();
  t_48c = t_7fc["tcAHT"].as<String>();
  g_c32d0 = t_48c.toFloat();
  t_480 = t_7fc["tcAM"].as<String>();
  g_c32cc = t_480.toFloat();
  t_474 = t_7fc["tcorr"].as<String>();
  g_c32c8 = t_474.toFloat();
  t_468 = t_7fc["tcHDC"].as<String>();
  g_c32c4 = t_468.toFloat();
  t_45c = t_7fc["matrixa"].as<String>();
  g_c34ba = t_45c.toInt();
  t_450 = t_7fc["D"].as<String>();
  g_c34b1 = t_450.toInt();
  t_444 = String(g_c34b9, 10);
  t_438 = t_7fc["num_matrx"].as<String>();
  iVar1 = (t_444 != t_438);
  if (iVar1 == 0) {
    t_420 = t_7fc["num_matrx"].as<String>();
    g_c34b9 = t_420.toInt();
  }
  else {
    t_42c = t_7fc["num_matrx"].as<String>();
    g_c34b9 = t_42c.toInt();
    P.setZone(0, 0, g_c34b9 + -1);
  }
  t_414 = t_7fc["Sl"].as<String>();
  g_c2f22 = t_414.toInt();
  t_408 = t_7fc["num_matr8"].as<String>();
  g_c34b8 = t_408.toInt();
  t_3fc = t_7fc["c_position"].as<String>();
  g_c34b7 = t_3fc.toInt();
  t_3f0 = t_7fc["lang"].as<String>();
  g_c2eef = t_3f0.toInt();
  t_3e4 = t_7fc["bud"].as<String>();
  g_c2fc8 = t_3e4.toInt();
  t_3d8 = t_7fc["fw_up"].as<String>();
  g_c2eec = t_3d8.toInt();
  t_3cc = t_7fc["onoffv"].as<String>();
  g_c2fc9 = t_3cc.toInt();
  t_3c0 = t_7fc["z"].as<String>();
  g_c2fb4 = t_3c0.toInt();
  t_3b4 = t_7fc["led_speed"].as<String>();
  g_c2f29 = t_3b4.toInt();
  t_3a8 = t_7fc["led_pix"].as<String>();
  g_c2f28 = t_3a8.toInt();
  t_39c = t_7fc["stat_ledo"].as<String>();
  g_c2f1e = t_39c.toInt();
  t_30 = t_7fc["stat_leds"].as<String>();
  iVar1 = (t_30 == "");
  if (iVar1 == 0) {
    g_bf042 = t_30.toInt();
  }
  else {
    g_bf042 = 0xff;
  }
  t_390 = t_7fc["led_fx"].as<String>();
  g_c2f27 = t_390.toInt();
  t_384 = t_7fc["led_fx_tw"].as<String>();
  g_c2f26 = t_384.toInt();
  t_378 = t_7fc["LDbrch"].as<String>();
  g_c2f25 = t_378.toInt();
  t_36c = t_7fc["led_brg"].as<String>();
  g_c2f24 = t_36c.toInt();
  t_360 = t_7fc["rnd_p_chg"].as<String>();
  g_c2f23 = t_360.toInt();
  t_354 = t_7fc["pAn"].as<String>();
  g_c2eed = t_354.toInt();
  t_348 = t_7fc["speed"].as<String>();
  g_bf04d = t_348.toInt();
  t_33c = t_7fc["font_l"].as<String>();
  g_c349c = t_33c.toInt();
  t_330 = t_7fc["fs"].as<String>();
  g_c349b = t_330.toInt();
  t_324 = t_7fc["bigclock"].as<String>();
  g_c349a = t_324.toInt();
  t_318 = t_7fc["smallclock"].as<String>();
  g_c3499 = t_318.toInt();
  t_30c = t_7fc["cl_effect"].as<String>();
  g_c3498 = t_30c.toInt();
  t_300 = t_7fc["cl_effect_sm"].as<String>();
  g_c3497 = t_300.toInt();
  t_2f4 = t_7fc["font_B"].as<String>();
  g_c3496 = t_2f4.toInt();
  t_2e8 = t_7fc["tAir"].as<String>();
  g_c32e1 = t_2e8.toInt();
  t_2dc = t_7fc["Ai"].as<String>();
  g_c32e0 = t_2dc.toInt();
  t_2d0 = t_7fc["nz"].as<String>();
  g_c34ab = t_2d0.toInt();
  t_2c4 = t_7fc["An"].as<String>();
  g_c34ad = t_2c4.toInt();
  t_2b8 = t_7fc["Se"].as<String>();
  g_c32e3 = t_2b8.toInt();
  t_2ac = t_7fc["tSW"].as<String>();
  g_c32e2 = t_2ac.toInt();
  t_2a0 = t_7fc["currency_sh"].as<String>();
  g_c3139 = t_2a0.toInt();
  t_294 = t_7fc["weather_sh"].as<String>();
  g_c3157 = t_294.toInt();
  t_288 = t_7fc["citato"].as<String>();
  g_c2eea = t_288.toInt();
  t_27c = t_7fc["news_show"].as<String>();
  g_c30f8 = t_27c.toInt();
  t_270 = t_7fc["news_src"].as<String>();
  g_c30ec = t_270;
  uVar3 = (uint)g_bf043;
  t_264 = t_7fc["px"].as<String>();
  uVar2 = t_264.toInt();
  if (uVar3 != uVar2) {
    led_off();
  }
  t_258 = t_7fc["yout_an"].as<String>();
  g_c2efc = t_258.toInt();
  t_24c = t_7fc["px"].as<String>();
  g_bf043 = t_24c.toInt();
  t_240 = t_7fc["time_show"].as<String>();
  g_c3428 = t_240.toInt();
  t_234 = t_7fc["Cdat"].as<String>();
  g_c2eeb = t_234.toInt();
  t_228 = t_7fc["MQ"].as<String>();
  g_c30c4 = t_228.toInt();
  t_21c = t_7fc["bitcoin_sh"].as<String>();
  g_c3138 = t_21c.toInt();
  t_210 = t_7fc["bright"].as<String>();
  g_c2ee9 = t_210.toInt();
  t_204 = t_7fc["brg"].as<String>();
  g_c2ee8 = t_204.toInt();
  t_1f8 = t_7fc["brg_d"].as<String>();
  g_c2ee7 = t_1f8.toInt();
  t_1ec = t_7fc["brg_n"].as<String>();
  g_c2ee6 = t_1ec.toInt();
  t_1e0 = t_7fc["show_city"].as<String>();
  g_c3156 = t_1e0.toInt();
  t_1d4 = t_7fc["n_vc"].as<String>();
  g_c2f1d = t_1d4.toInt();
  t_1c8 = t_7fc["DFvol"].as<String>();
  g_c2f1c = t_1c8.toInt();
  t_1bc = t_7fc["DFgol"].as<String>();
  g_c2f1b = t_1bc.toInt();
  t_1b0 = t_7fc["cntr_news"].as<String>();
  g_c30c8 = t_1b0;
  t_1a4 = t_7fc["as1"].as<String>();
  g_c2fc7 = t_1a4.toInt();
  t_198 = t_7fc["ad1"].as<String>();
  g_c2fc6 = t_198.toInt();
  t_18c = t_7fc["aL1"].as<String>();
  g_c2fc5 = t_18c.toInt();
  t_180 = t_7fc["lt_snd"].as<String>();
  g_c2f19 = t_180.toInt();
  t_174 = t_7fc["lt_rep"].as<String>();
  g_c2f18 = t_174.toInt();
  t_168 = t_7fc["prg"].as<String>();
  g_c34b2 = t_168.toInt();
  t_15c = t_7fc["t_syn"].as<String>();
  g_c3461 = t_15c.toInt();
  t_150 = t_7fc["sumT"].as<String>();
  g_c32e4 = t_150.toInt();
  fx_time_offset_apply();                            // нове: зміна пояса/літнього часу — одразу (timefix.cpp)
  fx_wdbg_set(t_7fc["wdbg"].as<String>().toInt() == 1);   // нове: налагодження по Wi-Fi (wdbg.cpp)
  t_138 = t_7fc["M1"].as<String>();
  g_c307f = t_138.toInt();
  t_fc = t_7fc["M2"].as<String>();
  g_c307e = t_fc.toInt();
  t_f0 = t_7fc["M3"].as<String>();
  g_c307d = t_f0.toInt();
  t_b4 = t_7fc["M4"].as<String>();
  g_c307c = t_b4.toInt();
  t_a8 = t_7fc["nr"].as<String>();
  g_c2ffc = t_a8.toInt();
  if (((g_c34ba == '\b') && (g_c3498 != 0x11)) && (7 < g_c3498)) {
    t_6c = "cl_effect";
    t_60 = "5";
    config_set(t_6c,t_60);
  }
  t_144 = t_7fc["time_cd"].as<String>();
  t_138 = t_7fc["time_cn"].as<String>();
  t_12c = t_144.substring(0, 2);
  iVar1 = t_12c.toInt();
  t_120 = t_144.substring(3);
  g_c2e6c = t_120.toInt();
  g_c2e6c = iVar1 * 0x3c + g_c2e6c;
  t_114 = t_138.substring(0, 2);
  iVar1 = t_114.toInt();
  t_108 = t_138.substring(3);
  g_c2e68 = t_108.toInt();
  g_c2e68 = iVar1 * 0x3c + g_c2e68;
  t_fc = t_7fc["time_d"].as<String>();
  t_f0 = t_7fc["time_n"].as<String>();
  t_e4 = t_fc.substring(0, 2);
  iVar1 = t_e4.toInt();
  t_d8 = t_fc.substring(3);
  g_c2e64 = t_d8.toInt();
  g_c2e64 = iVar1 * 0x3c + g_c2e64;
  t_cc = t_f0.substring(0, 2);
  iVar1 = t_cc.toInt();
  t_c0 = t_f0.substring(3);
  g_c2e60 = t_c0.toInt();
  g_c2e60 = iVar1 * 0x3c + g_c2e60;
  t_b4 = t_7fc["t_dsp"].as<String>();
  t_a8 = t_7fc["t_nsp"].as<String>();
  t_9c = t_b4.substring(0, 2);
  iVar1 = t_9c.toInt();
  t_90 = t_b4.substring(3);
  g_c2e5c = t_90.toInt();
  g_c2e5c = iVar1 * 0x3c + g_c2e5c;
  t_84 = t_a8.substring(0, 2);
  iVar1 = t_84.toInt();
  t_78 = t_a8.substring(3);
  g_c2e58 = t_78.toInt();
  g_c2e58 = iVar1 * 0x3c + g_c2e58;
  t_6c = t_7fc["time_cdl"].as<String>();
  t_60 = t_7fc["time_cnl"].as<String>();
  t_54 = t_6c.substring(0, 2);
  iVar1 = t_54.toInt();
  t_48 = t_6c.substring(3);
  g_c2e54 = t_48.toInt();
  g_c2e54 = iVar1 * 0x3c + g_c2e54;
  t_3c = t_60.substring(0, 2);
  iVar1 = t_3c.toInt();
  t_30 = t_60.substring(3);
  g_c2e50 = t_30.toInt();
  g_c2e50 = iVar1 * 0x3c + g_c2e50;
  g_c1a9a = 0;
  g_c1a97 = 0x21;
  g_c2fc4 = 0;
  myDFPlayer.volume(g_c2f1c);
  g_c335b = 1;
  g_c2e72 = time_in_range(g_c2e6c,g_c2e68);
  g_c2e71 = time_in_range(g_c2e64,g_c2e60);
  g_c2e70 = time_in_range(g_c2e5c,g_c2e58);
  if (!fx_time_known()) { g_c2e72 = 1; g_c2e71 = 1; }   // виправлено: час невідомий (1970) — табло як удень
  if (g_c2c00 == g_c34ab) goto LAB_4011fc46;
  if (g_c34ba == '\x04') {
    uVar4 = 0;
    cVar5 = g_c34b9 + -1;
LAB_4011fc24:
    g_c34bc->clear(uVar4, cVar5);
  }
  else if (g_c34ba == '\b') {
    if (g_c34b8 == '\b') {
      cVar5 = '\v';
      uVar4 = 8;
      if (g_c34b7 == '\0') {
        cVar5 = '\x0f';
        uVar4 = 0xc;
      }
    }
    else {
      cVar5 = '\a';
      uVar4 = 4;
    }
    goto LAB_4011fc24;
  }
  g_c2c00 = g_c34ab;
  g_c34a2 = 0;
  g_c34a1 = 0;
  g_c34a0 = 0;
LAB_4011fc46:
  return;
}

void config_defaults() {  // @400ea480: заводські значення (150 ключів, порядок як в оригіналі)
  JsonDocument d;
  d["Cwi"] = "1";
  d["wizard_done"] = "0";
  d["ssid"] = "Your WI-FI network";
  d["pass"] = "Password";
  d["lang"] = "1";
  d["text"] = "Here your text";
  d["text2"] = "";
  d["text3"] = "";
  d["text4"] = "";
  d["d1_rem"] = "";
  d["d2_rem"] = "";
  d["speed"] = "47";
  d["brg"] = "5";
  d["brg_d"] = "5";
  d["brg_n"] = "1";
  d["bright"] = "0";
  d["prg"] = "15";
  d["matrixa"] = "4";
  d["D"] = "0";
  d["num_matrx"] = "4";
  d["num_matr8"] = "4";
  d["c_position"] = "0";
  d["An"] = "1";
  d["tempdht_ch"] = "11";
  d["font_l"] = "0";
  d["font_B"] = "0";
  d["fs"] = "0";
  d["led_brg"] = "50";
  d["led_speed"] = "31";
  d["led_pix"] = "0";
  d["fw_up"] = "0";
  d["time_show"] = "10";
  d["led_fx"] = "0";
  d["led_fx_tw"] = "2";
  d["stat_ledo"] = "94";
  d["stat_leds"] = "255";
  d["rnd_p_chg"] = "10";
  d["LDbrch"] = "0";
  d["time_cdl"] = "08:00";
  d["time_cnl"] = "23:00";
  d["ntp_srv"] = "pool.ntp.org";
  d["tz"] = "2";
  d["time_d"] = "08:00";
  d["time_n"] = "22:00";
  d["time_cd"] = "08:00";
  d["time_cn"] = "23:00";
  d["t_dsp"] = "08:00";
  d["t_nsp"] = "23:00";
  d["t_syn"] = "60";
  d["Cdat"] = "2";
  d["bigclock"] = "0";
  d["smallclock"] = "0";
  d["bcl_fx"] = "0";
  d["cl_effect"] = "5";
  d["cl_effect_sm"] = "8";
  d["cl_sprite"] = "0";
  d["t_m"] = "12:00";
  d["d_m"] = "";
  d["nz"] = "0";
  d["sumT"] = "0";
  d["lat"] = "51.509865";
  d["lon"] = "-0.118092";
  d["ApiKey"] = "";
  d["ApiKey2"] = "";
  d["weather_sh"] = "0";
  d["tomorrow_w"] = "0";
  d["show_city"] = "1";
  d["un"] = "0";
  d["myci"] = "";
  d["yout_an"] = "0";
  d["px"] = "22";
  d["ayt"] = "";
  d["Yid"] = "";
  d["chds"] = "0";
  d["chdh"] = "0";
  d["chbm"] = "0";
  d["chO"] = "0";
  d["chZ"] = "0";
  d["adw"] = "0";
  d["curr_a"] = "EUR";
  d["curr_b"] = "UAH";
  d["currency_sh"] = "0";
  d["btc_VAL"] = "EUR";
  d["bitcoin_sh"] = "0";
  d["MQ"] = "0";
  d["host"] = "";
  d["port"] = "";
  d["user"] = "";
  d["mpass"] = "";
  d["ha_en"] = "0";
  d["ha_name"] = "Clock Informer";
  d["m_text_1"] = "";
  d["m_text_2"] = "";
  d["m_text_3"] = "";
  d["m_text_4"] = "";
  d["txMQ"] = "";
  d["txMQ2"] = "";
  d["txMQ3"] = "";
  d["txMQ4"] = "";
  d["MQ_ds"] = "";
  d["MQ_BM"] = "";
  d["MQ_DHT"] = "";
  d["MQ_O"] = "";
  d["MQZ"] = "";
  d["M1"] = "10";
  d["M2"] = "10";
  d["M3"] = "10";
  d["M4"] = "10";
  d["nr"] = "0";
  d["nApi"] = "";
  d["nId"] = "";
  d["tSW"] = "0";
  d["Se"] = "0";
  d["Sl"] = "0";
  d["citato"] = "0";
  d["tLangc"] = "en";
  d["tempds"] = "0";
  d["tempdht"] = "0";
  d["tbmp"] = "0";
  d["bebp"] = "0";
  d["tAir"] = "0";
  d["txO"] = "Temp: ";
  d["Ai"] = "0";
  d["tcorr"] = "0.00";
  d["tcorrds"] = "0.00";
  d["tcbmp"] = "0.00";
  d["tcO"] = "0.00";
  d["textds"] = "Temp: ";
  d["textdht"] = "Temp: ";
  d["txbmp"] = "Temp: ";
  d["txO"] = "Temp: ";
  d["txA"] = "Air: ";
  d["news_show"] = "0";
  d["cntr_news"] = "ru";
  d["Cat_news"] = "general";
  d["apinews"] = "";
  d["descr_n"] = "1";
  d["DFvol"] = "25";
  d["DFgol"] = "1";
  d["onoffv"] = "5";
  d["lt_snd"] = "0";
  d["lt_rep"] = "1";
  d["n_vc"] = "1";
  d["z"] = "2";
  d["secwn"] = "Clock ESP";
  d["secwp"] = "";
  d["bud"] = "0";
  d["hero_order"] = "";
  d["AuN"] = "";
  d["AuP"] = "";
  g_c2ef0 = "";
  serializeJson(d, g_c2ef0);
}

// @400e9764: тексти MQTT із документа конфігурації
void config_apply_mqtt_texts(JsonDocument &d) {
  g_c25c4[0] = d["m_text_1"].as<String>();
  g_c25d0 = d["m_text_2"].as<String>();
  g_c25dc = d["m_text_3"].as<String>();
  g_c25e8 = d["m_text_4"].as<String>();
  g_c2594[0] = d["txMQ"].as<String>();
  g_c25a0 = d["txMQ2"].as<String>();
  g_c25ac = d["txMQ3"].as<String>();
  g_c25b8 = d["txMQ4"].as<String>();
}

// @40100348: міграція старих конфігурацій при старті
void config_migrate() {
  JsonDocument d;
  deserializeJson(d, g_c2ef0);
  bool a = d["ApiCUR"].as<String>() != "null";
  if (a) config_remove_key("ApiCUR");
  bool b = d["R"].as<String>() != "4";
  if (b) { config_set("adw", "0"); config_set("R", "4"); }
  bool c = d["An"].as<String>() == "";
  if (c) config_set("An", "1");
  bool e = d["px"].as<String>() == "";
  if (!e) {
    if (!c && !b && !a) return;
  } else {
    config_set("px", "22");
    g_bf043 = 0x16;
  }
  g_c2d28 = 1;
  config_save_atomic();
  config_load_file();
}
// @400f4af8: видалити ключ із конфігурації
void config_remove_key(const String &key) {
  JsonDocument d;
  if (config_doc_load(d)) {
    d.remove(key);
    config_doc_store(d);
  }
}
