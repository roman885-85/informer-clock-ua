// Світлодіодна стрічка WS2812B (GPIO27, до 30 шт.): 28 ефектів і 18 варіантів мерехтіння, режим CO2, будильник.
// Ефекти автора зібрані з прикладів FastLED (DemoReel100, Fire2012, ColorTwinkles, TwinkleFOX, Pride2015, ColorWaves),
// tweaking4all (м'ячики, вогонь) та Andrew Tuline (three_sin, two_sin, ripples, plasma, matrix, beatwave).
// Перенесено з 2.7.1 (адреси оригіналу в коментарях).
#include "fw.h"

#define LEDS g_c2f5a          // CRGB[30] @3ffc2f5a
#define NUM_LEDS g_bf043      // кількість світлодіодів із конфігурації
#define qsuba(x, b) ((x > b) ? x - b : 0)   // Andrew Tuline
#define qsubd(x, b) ((x > b) ? b : 0)

// ---------- палітри TwinkleFOX (Mark Kriegsman) @3f4063a0…3f406560 ----------
#define Holly_Green 0x00580c
#define Holly_Red 0xB00402
#define HALFFAIRY ((CRGB::FairyLight & 0xFEFEFE) / 2)
#define QUARTERFAIRY ((CRGB::FairyLight & 0xFCFCFC) / 4)
#define C9_Red 0xB80400
#define C9_Orange 0x902C02
#define C9_Green 0x046002
#define C9_Blue 0x070758
#define C9_White 0x606820
#define Ice_Blue1 0x0C1040
#define Ice_Blue2 0x182080
#define Ice_Blue3 0x5080C0
const TProgmemRGBPalette16 RedGreenWhite_p FL_PROGMEM = {
  CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red, CRGB::Red,
  CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, CRGB::Green, CRGB::Green, CRGB::Green, CRGB::Green };
const TProgmemRGBPalette16 Holly_p FL_PROGMEM = {
  Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green,
  Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Green, Holly_Red };
const TProgmemRGBPalette16 RedWhite_p FL_PROGMEM = {
  CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray,
  CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray, CRGB::Red, CRGB::Red, CRGB::Gray, CRGB::Gray };
const TProgmemRGBPalette16 BlueWhite_p FL_PROGMEM = {
  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue,
  CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Blue, CRGB::Gray, CRGB::Gray, CRGB::Gray };
const TProgmemRGBPalette16 FairyLight_p FL_PROGMEM = {
  CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, HALFFAIRY, HALFFAIRY, CRGB::FairyLight, CRGB::FairyLight,
  QUARTERFAIRY, QUARTERFAIRY, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight, CRGB::FairyLight };
const TProgmemRGBPalette16 Snow_p FL_PROGMEM = {
  0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048,
  0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0x304048, 0xE0F0FF };
const TProgmemRGBPalette16 RetroC9_p FL_PROGMEM = {
  C9_Red, C9_Orange, C9_Red, C9_Orange, C9_Orange, C9_Red, C9_Orange, C9_Red,
  C9_Green, C9_Green, C9_Green, C9_Green, C9_Blue, C9_Blue, C9_Blue, C9_White };
const TProgmemRGBPalette16 Ice_p FL_PROGMEM = {
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1,
  Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue1, Ice_Blue2, Ice_Blue2, Ice_Blue2, Ice_Blue3 };

// ---------- стан ефектів (у .bss оригіналу — нулі; палітри — з глобального конструктора @401008b8) ----------
CRGBPalette16 gPalettes[8] = { RainbowColors_p, RainbowStripeColors_p, CloudColors_p, LavaColors_p,   // @3ffc1f1a
                               OceanColors_p, ForestColors_p, PartyColors_p, HeatColors_p };
CRGBPalette16 gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);  // @3ffc1ecc вогонь з палітрою
CRGBPalette16 twCurrentPalette(CRGB::Black);   // @3ffc1e9c ColorTwinkles
CRGBPalette16 tfCurrentPalette;                // @3ffc1e5c TwinkleFOX
CRGBPalette16 currentPalette(CRGB::Black);     // @3ffc1e20 Andrew Tuline
CRGBPalette16 targetPalette(PartyColors_p);    // @3ffc1df0
CRGBPalette16 thisPalette;                     // @3ffc1d9c two_sin
CRGBPalette16 thatPalette;                     // @3ffc1d6c
TBlendType currentBlending;                    // @3ffc1d68
uint8_t directionFlags[(30 + 7) / 8];          // @3ffc1e98 ColorTwinkles
CRGB gBackgroundColor = CRGB::Black;           // @3ffc1e8c TwinkleFOX
int ledsX[30][3];                              // @3ffc2104 копія кадру для «маршу»
uint8_t heatFire[30];                          // @3ffc1874 Fire (tweaking4all)
uint8_t heatFirePal[30];                       // @3ffc1836 Fire2012WithPalette
int wave1, wave2, wave3;                       // @3ffc1e58/1e54/1e50 three_sin
// two_sin / matrix (Andrew Tuline) @3ffc1dcc…1de1
uint8_t thathue;                               // 1de1
uint8_t thisrot, thatrot, allsat;              // 1de0, 1ddf, 1dde
uint8_t thisdir, thatdir, alldir;              // 1ddd, 1ddc, 1ddb
int8_t thisspeed, thatspeed;                   // 1dda, 1dd9
uint8_t allfreq;                               // 1dd8
int thisphase, thatphase;                      // 1dd4, 1dd0
uint8_t thiscutoff, thatcutoff, fadeval;       // 1dce, 1dcd, 1dcc
uint8_t huerot, bgclr, bgbri;                  // 1cb2, 1cb1, 1cb0 matrix
// ripples (Andrew Tuline) @3ffc1d20: 6 × 10 байтів
struct ripple {
  uint8_t bright;
  uint8_t color;
  int16_t pos;
  int8_t velocity;
  uint8_t life;
  uint8_t maxLife;
  uint8_t fade;
  bool exist;
  void Move() {
    pos += velocity;
    life++;
    if (pos > NUM_LEDS - 1) {
      velocity *= -1;
      pos = NUM_LEDS - 1;
    }
    if (pos < 0) {
      pos = 0;
      velocity *= -1;
    }
    bright = scale8(bright, fade);
    if (life > maxLife) exist = false;
  }
  void Init(uint8_t Fade, uint8_t MaxLife) {
    pos = random8(NUM_LEDS / 8, NUM_LEDS - NUM_LEDS / 8);
    velocity = 1;
    life = 0;
    maxLife = MaxLife;
    exist = true;
    bright = 255;
    color = millis();
    fade = Fade;
  }
};
ripple ripples[6];

// ---------- службові ----------
// @400dec14: розклад стрічки — увімкнена в заданий проміжок часу
void led_schedule_check() {
  int8_t v = g_c2f1f;
  if (v == 1) {
    g_c2f1f = 0;
    g_c2274 = v;
    g_c2274 = time_in_range(g_c2e54, g_c2e50) != 0;
  }
}
// @400dec40: випадковий ефект кожні ms
void led_random_tick(uint32_t ms) {
  if (millis() - g_c1894 >= ms) {
    g_c1894 = millis();
    g_c34ac = random(0, 27);
    g_c34aa = random(1, 19);
  }
}
// @400dec74: яскравість (ручна або за фоторезистором) і показ кадру
void led_show() {
  uint8_t *b;
  if (g_c2f25 == 0) b = &g_c2f24;
  else {
    if (g_c2f25 != 1) goto show;
    b = &g_c2ee3;
  }
  if (g_c2ee2 != *b) {
    g_c2ee2 = *b;
    FastLED.setBrightness(*b);
  }
show:
  FastLED.show();
}
// @400ded10: залити всю стрічку одним кольором
void led_fill_all(CRGB c) {
  for (int i = 0; i < NUM_LEDS; i++) LEDS[i] = c;
}

// ---------- ефекти 0…15 ----------
// @400deca8: 0 — веселка з кроком 255/N
void fx_rainbow_step() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  g_c2100 += 2;
  for (uint8_t i = 0; i < NUM_LEDS; i++) LEDS[i] = CHSV(g_c2100 + i * (float)(255 / NUM_LEDS), 255, 255);
}
// @400ded40: 1 — уся стрічка одним кольором, що змінюється
void fx_hue_cycle() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  g_c2100 += 2;
  CRGB c = CHSV(g_c2100, 255, 255);
  led_fill_all(c);
}
// @400ded90: 2 — «дихання» випадковим кольором
void fx_breathe() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  if (g_c20f8 == 0) {
    g_c20f4 += 4;
    g_c20fc += 2;
    if (g_c20f4 > 254) g_c20f4 = 255;
    if (g_c20fc > 249) {
      g_c20f8 = 1;
      g_c20fc = 125;
    }
    CRGB c = CHSV(g_bec24, g_bec20, g_c20f4);
    led_fill_all(c);
  } else if (g_c20f8 == 1) {
    g_c20f4 -= 4;
    g_c20fc -= 2;
    if (g_c20f4 < 1) g_c20f4 = 0;
    if (g_c20fc < 1) {
      g_c20f8 = 0;
      g_bec24 = random(0, 255);
      g_bec20 = random(0, 255);
    }
    CRGB c = CHSV(g_bec24, g_bec20, g_c20f4);
    led_fill_all(c);
  }
}
// @400dee84: 25 — статичний колір
void fx_static_color() {
  CRGB c = CHSV(g_c2f1e, g_bf042, 255);
  led_fill_all(c);
}
// @400deec4: 3 — три м'ячики (tweaking4all, g = -10)
void fx_bouncing_balls() {
  float Gravity = -10;
  int StartHeight = 1;
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  for (int i = 0; i < 3; i++) {
    g_c20c4[i] = millis() - g_c20ac[i];
    g_c20dc[i] = 0.5 * Gravity * pow(g_c20c4[i] / 1000, 2.0) + g_c20d0[i] * g_c20c4[i] / 1000;
    if (g_c20dc[i] < 0) {
      g_c20dc[i] = 0;
      g_c20d0[i] = g_c20a0[i] * g_c20d0[i];
      g_c20ac[i] = millis();
      if (g_c20d0[i] < 0.01) g_c20d0[i] = g_bec1c;
    }
    g_c20b8[i] = round(g_c20dc[i] * (NUM_LEDS - 1) / StartHeight);
  }
  for (int i = 0; i < 3; i++) LEDS[g_c20b8[i]] = CHSV(i * 40, 255, 255);
  FastLED.show();
  for (int i = 0; i < 3; i++) LEDS[g_c20b8[i]] = CRGB(0, 0, 0);
}
// рух точки туди й назад (ефекти 4 і 5)
static void dot_move() {
  if (g_c209c == 0) {
    if (g_c20e8 + 1 == NUM_LEDS) g_c209c = 1;
    else g_c20e8++;
  }
  if (g_c209c == 1) {
    g_c20e8--;
    if (g_c20e8 == 0) g_c209c = 0;
  }
}
// @400df03c: 4 — точка, що бігає
void fx_dot() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  g_c226c = 0;
  dot_move();
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == g_c20e8) LEDS[i] = CHSV(g_c226c, g_bec28, 255);
    else LEDS[i] = CHSV(0, 0, 0);
  }
}
// @400df0cc / @400df0e4: сусідні індекси по колу
int led_next(int i) { return (i < NUM_LEDS - 1) ? i + 1 : 0; }
int led_prev(int i) {
  if (i < 1) i = NUM_LEDS;
  return i - 1;
}
// @400df0f8: 5 — точка зі згасаючими хвостами з обох боків
void fx_dot_tails() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  g_c226c = 0;
  dot_move();
  int pos = g_c20e8;
  int n1 = led_next(pos), n2 = led_next(n1), n3 = led_next(n2);
  int p1 = led_prev(pos), p2 = led_prev(p1), p3 = led_prev(p2);
  for (int i = 0; i < NUM_LEDS; i++) {
    if (i == g_c20e8) LEDS[i] = CHSV(g_c226c, g_bec28, 255);
    else if (i == n1) LEDS[i] = CHSV(g_c226c, g_bec28, 150);
    else if (i == n2) LEDS[i] = CHSV(g_c226c, g_bec28, 80);
    else if (i == n3) LEDS[i] = CHSV(g_c226c, g_bec28, 20);
    else if (i == p1) LEDS[i] = CHSV(g_c226c, g_bec28, 150);
    else if (i == p2) LEDS[i] = CHSV(g_c226c, g_bec28, 80);
    else if (i == p3) LEDS[i] = CHSV(g_c226c, g_bec28, 20);
    else LEDS[i] = CHSV(0, 0, 0);
  }
}
// @400df210: зберегти кадр у ledsX
void led_save_frame() {
  for (int i = 0; i < NUM_LEDS; i++) {
    ledsX[i][0] = LEDS[i].r;
    ledsX[i][1] = LEDS[i].g;
    ledsX[i][2] = LEDS[i].b;
  }
}
// @400df240: 6 — випадкові кольори біжать стрічкою
void fx_random_march() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  led_save_frame();
  LEDS[0] = CHSV(random(0, 255), 255, 255);
  for (int i = 1; i < NUM_LEDS; i++) {
    int p = led_prev(i);
    LEDS[i].r = ledsX[p][0];
    LEDS[i].g = ledsX[p][1];
    LEDS[i].b = ledsX[p][2];
  }
}
// @400df2a4: 7 — веселка
void fx_rainbow() {
  g_c2270--;
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  fill_rainbow(LEDS, NUM_LEDS, g_c2270, 5);
}
// @400df2d4: колір «температури» (tweaking4all)
void setPixelHeatColor(int Pixel, byte temperature) {
  byte t192 = round((temperature / 255.0) * 191);
  byte heatramp = t192 & 0x3F;
  heatramp <<= 2;
  if (t192 > 0x80) {
    LEDS[Pixel].r = 255;
    LEDS[Pixel].g = 255;
    LEDS[Pixel].b = heatramp;
  } else if (t192 > 0x40) {
    LEDS[Pixel].r = 255;
    LEDS[Pixel].g = heatramp;
    LEDS[Pixel].b = 0;
  } else {
    LEDS[Pixel].r = heatramp;
    LEDS[Pixel].g = 0;
    LEDS[Pixel].b = 0;
  }
}
// @400df350: 8 — вогонь (tweaking4all), виклик Fire(55, 120)
void Fire(int Cooling, int Sparking) {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  int cooldown;
  for (int i = 0; i < NUM_LEDS; i++) {
    cooldown = random(0, ((Cooling * 10) / NUM_LEDS) + 2);
    if (cooldown > heatFire[i]) heatFire[i] = 0;
    else heatFire[i] = heatFire[i] - cooldown;
  }
  for (int k = NUM_LEDS - 1; k >= 2; k--) heatFire[k] = (heatFire[k - 1] + heatFire[k - 2] + heatFire[k - 2]) / 3;
  if (random(255) < Sparking) {
    int y = random(7);
    heatFire[y] = heatFire[y] + random(160, 255);
  }
  for (int j = 0; j < NUM_LEDS; j++) setPixelHeatColor(j, heatFire[j]);
}
// @400df820: Fire2012WithPalette (COOLING = g_bec1a, SPARKING = g_bec1b); forward = 0 — знизу догори з кінця
void Fire2012WithPalette(CRGBPalette16 pal, bool forward) {
  fill_solid(LEDS, NUM_LEDS, CRGB(0, 0, 0));
  random16_add_entropy(random(256));
  for (uint16_t i = 0; i < NUM_LEDS; i++) heatFirePal[i] = qsub8(heatFirePal[i], random8(0, ((g_bec1a * 10) / NUM_LEDS) + 2));
  for (uint16_t k = NUM_LEDS - 1; k >= 2; k--) heatFirePal[k] = (heatFirePal[k - 1] + heatFirePal[k - 2] + heatFirePal[k - 2]) / 3;
  if (random8() < g_bec1b) {
    int y = random8(7);
    heatFirePal[y] = qadd8(heatFirePal[y], random8(160, 255));
  }
  for (uint16_t j = 0; j < NUM_LEDS; j++) {
    uint8_t colorindex = scale8(heatFirePal[j], 190);
    CRGB color = ColorFromPalette(pal, colorindex);
    int pixelnumber;
    if (forward) pixelnumber = j;
    else pixelnumber = (NUM_LEDS - 1) - j;
    LEDS[pixelnumber] = color;
  }
}
// @400df968: 9 — вогонь з палітрою
void fx_fire_palette() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  Fire2012WithPalette(gPal, false);
}
// @400df40c: 10 — конфеті з палітри
void fx_confetti() {
  EVERY_N_MILLISECONDS(20) { g_c1f18++; }
  fadeToBlackBy(LEDS, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  LEDS[pos] += ColorFromPalette(gPalettes[g_c1f19], g_c1f18 + random8(64), 255, LINEARBLEND);
}
// @400df4e0: 11 — «bpm» з палітри (темп g_c2f29)
void fx_bpm() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  EVERY_N_MILLISECONDS(20) { g_c1f18++; }
  uint8_t beat = beatsin8(g_c2f29, 64, 255);
  CRGBPalette16 palette = gPalettes[g_c1f19];
  for (int i = 0; i < NUM_LEDS; i++) LEDS[i] = ColorFromPalette(palette, g_c1f18 + (i * 2), beat - g_c1f18 + (i * 10));
}
// @400df5c0: 14 — sinelon
void fx_sinelon() {
  EVERY_N_MILLISECONDS(20) { g_c1f18++; }
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  LEDS[pos] += CHSV(g_c1f18, 255, 192);
}
// @400df678: 15 — juggle
void fx_juggle() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  byte dothue = 0;
  for (int i = 0; i < 8; i++) {
    LEDS[beatsin16(i + 7, 0, NUM_LEDS - 1)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}
// @400df70c: 12 — Pride2015
void fx_pride() {
  static uint16_t sPseudotime = 0;   // @3ffc1854
  static uint16_t sLastMillis = 0;   // @3ffc1856
  static uint16_t sHue16 = 0;        // @3ffc1858
  uint8_t sat8 = beatsin88(87, 220, 250);
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);
  uint16_t hue16 = sHue16;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;
    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    CRGB newcolor = CHSV(hue8, sat8, bri8);
    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;
    nblend(LEDS[pixelnumber], newcolor, 64);
  }
}
// @400df98c: ColorWaves
void colorwaves(CRGB *ledarray, uint16_t numleds, CRGBPalette16 &palette) {
  static uint16_t sPseudotime = 0;   // @3ffc1830
  static uint16_t sLastMillis = 0;   // @3ffc1832
  static uint16_t sHue16 = 0;        // @3ffc1834
  uint8_t brightdepth = beatsin88(341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88(203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);
  uint16_t hue16 = sHue16;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);
  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis;
  sLastMillis = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88(400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;
  for (uint16_t i = 0; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if (h16_128 & 0x100) hue8 = 255 - (h16_128 >> 1);
    else hue8 = h16_128 >> 1;
    brightnesstheta16 += brightnessthetainc16;
    uint16_t b16 = sin16(brightnesstheta16) + 32768;
    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);
    uint8_t index = hue8;
    index = scale8(index, 240);
    CRGB newcolor = ColorFromPalette(palette, index, bri8);
    uint16_t pixelnumber = i;
    pixelnumber = (numleds - 1) - pixelnumber;
    nblend(ledarray[pixelnumber], newcolor, 128);
  }
}
// @400dfac4: 13 — кольорові хвилі (чорний–синій–блакитний–білий)
void fx_colorwaves() {
  CRGBPalette16 p(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);
  colorwaves(LEDS, NUM_LEDS, p);
}

// ---------- ColorTwinkles (Mark Kriegsman) — варіанти мерехтіння 1, 3, 4, 6 ----------
// @400dfb10
CRGB makeBrighter(const CRGB &color, fract8 howMuchBrighter) {
  CRGB incrementalColor = color;
  incrementalColor.nscale8(howMuchBrighter);
  return color + incrementalColor;
}
// @400dfb74
CRGB makeDarker(const CRGB &color, fract8 howMuchDarker) {
  CRGB newcolor = color;
  newcolor.nscale8(255 - howMuchDarker);
  return newcolor;
}
// @400dfbb0
bool getPixelDirection(uint16_t i) {
  uint16_t index = i / 8;
  uint8_t bitNum = i & 0x07;
  return bitRead(directionFlags[index], bitNum);
}
// @400dfbd8
void setPixelDirection(uint16_t i, bool dir) {
  uint16_t index = i / 8;
  uint8_t bitNum = i & 0x07;
  bitWrite(directionFlags[index], bitNum, dir);
}
// @400dfc10
void brightenOrDarkenEachPixel(fract8 fadeUpAmount, fract8 fadeDownAmount) {
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    if (getPixelDirection(i) == 0) {
      LEDS[i] = makeDarker(LEDS[i], fadeDownAmount);
    } else {
      LEDS[i] = makeBrighter(LEDS[i], fadeUpAmount);
      if (LEDS[i].r == 255 || LEDS[i].g == 255 || LEDS[i].b == 255) setPixelDirection(i, 0);
    }
  }
}
// @400dfc90
void colortwinkles() {
  EVERY_N_MILLIS(30) {
    brightenOrDarkenEachPixel(32, 20);
    if (random8() < 255) {
      int pos = random16(NUM_LEDS);
      if (!LEDS[pos]) {
        LEDS[pos] = ColorFromPalette(twCurrentPalette, random8(), 64, NOBLEND);
        setPixelDirection(pos, 1);
      }
    }
  }
}
// @400dfd5c / @400dfd70 / @400dfd84 / @400dfdec
void tw_cloud() { twCurrentPalette = CloudColors_p; colortwinkles(); }
void tw_rainbow() { twCurrentPalette = RainbowColors_p; colortwinkles(); }
void tw_snow() {
  CRGB W(CRGB::White), w(85, 85, 85);
  twCurrentPalette = CRGBPalette16(W, W, W, W, w, w, w, w, w, w, w, w, w, w, w, w);
  colortwinkles();
}
void tw_fairy() {
  CRGB l(0xE1A024);
  twCurrentPalette = CRGBPalette16(l, l, l, l, l, l, l, l, l, l, l, l, l, l, l, l);
  colortwinkles();
}

// ---------- TwinkleFOX (Mark Kriegsman) — решта варіантів мерехтіння ----------
// @401d8164
uint8_t attackDecayWave8(uint8_t i) {
  if (i < 86) {
    return i * 3;
  } else {
    i -= 86;
    return 255 - (i + (i / 2));
  }
}
// @401d818c
void coolLikeIncandescent(CRGB &c, uint8_t phase) {
  if (phase < 128) return;
  uint8_t cooling = (phase - 128) >> 4;
  c.g = qsub8(c.g, cooling);
  c.b = qsub8(c.b, cooling * 2);
}
// @400dfe3c (TWINKLE_SPEED = g_bec19, TWINKLE_DENSITY = g_bec18)
CRGB computeOneTwinkle(uint32_t ms, uint8_t salt) {
  uint16_t ticks = ms >> (8 - g_bec19);
  uint8_t fastcycle8 = ticks;
  uint16_t slowcycle16 = (ticks >> 8) + salt;
  slowcycle16 += sin8(slowcycle16);
  slowcycle16 = (slowcycle16 * 2053) + 1384;
  uint8_t slowcycle8 = (slowcycle16 & 0xFF) + (slowcycle16 >> 8);
  uint8_t bright = 0;
  if (((slowcycle8 & 0x0E) / 2) < g_bec18) bright = attackDecayWave8(fastcycle8);
  uint8_t hue = slowcycle8 - salt;
  CRGB c;
  if (bright > 0) {
    c = ColorFromPalette(tfCurrentPalette, hue, bright, NOBLEND);
    coolLikeIncandescent(c, fastcycle8);
  } else {
    c = CRGB::Black;
  }
  return c;
}
// @400dfee4
void drawTwinkles() {
  uint16_t PRNG16 = 11337;
  uint32_t clock32 = millis();
  CRGB bg;
  bg = gBackgroundColor;
  uint8_t backgroundBrightness = bg.getAverageLight();
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    CRGB &pixel = LEDS[i];
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;
    uint16_t myclockoffset16 = PRNG16;
    PRNG16 = (uint16_t)(PRNG16 * 2053) + 1384;
    uint8_t myspeedmultiplierQ5_3 = ((((PRNG16 & 0xFF) >> 4) + (PRNG16 & 0x0F)) & 0x0F) + 0x08;
    uint32_t myclock30 = (uint32_t)((clock32 * myspeedmultiplierQ5_3) >> 3) + myclockoffset16;
    uint8_t myunique8 = PRNG16 >> 8;
    CRGB c = computeOneTwinkle(myclock30, myunique8);
    uint8_t cbright = c.getAverageLight();
    int16_t deltabright = cbright - backgroundBrightness;
    if (deltabright >= 32 || (!bg)) {
      pixel = c;
    } else if (deltabright > 0) {
      pixel = blend(bg, c, deltabright * 8);
    } else {
      pixel = bg;
    }
  }
}
// @400e0044…@400e0148
void tf_red_green_white() { tfCurrentPalette = RedGreenWhite_p; drawTwinkles(); }
void tf_holly() { tfCurrentPalette = Holly_p; drawTwinkles(); }
void tf_red_white() { tfCurrentPalette = RedWhite_p; drawTwinkles(); }
void tf_blue_white() { tfCurrentPalette = BlueWhite_p; drawTwinkles(); }
void tf_fairy_light() { tfCurrentPalette = FairyLight_p; drawTwinkles(); }
void tf_snow() { tfCurrentPalette = Snow_p; drawTwinkles(); }
void tf_ice() { tfCurrentPalette = Ice_p; drawTwinkles(); }
void tf_retro_c9() { tfCurrentPalette = RetroC9_p; drawTwinkles(); }
void tf_party() { tfCurrentPalette = PartyColors_p; drawTwinkles(); }
void tf_forest() { tfCurrentPalette = ForestColors_p; drawTwinkles(); }
void tf_lava() { tfCurrentPalette = LavaColors_p; drawTwinkles(); }
void tf_heat() { tfCurrentPalette = HeatColors_p; drawTwinkles(); }
void tf_cloud() { tfCurrentPalette = CloudColors_p; drawTwinkles(); }
void tf_ocean() { tfCurrentPalette = OceanColors_p; drawTwinkles(); }

// ---------- Andrew Tuline ----------
// @400e015c: зміна цільової палітри для three_sin (цикл 60 с)
void ChangeMe_three_sin() {
  uint8_t secondHand = (millis() / 1000) % 60;
  if (g_bebff != secondHand) {    // static lastSecond = 99 @3ffbebff
    g_bebff = secondHand;
    CRGB p = CHSV(HUE_PURPLE, 255, 255);
    CRGB g = CHSV(HUE_GREEN, 255, 255);
    CRGB u = CHSV(HUE_BLUE, 255, 255);
    CRGB b = CRGB::Black;
    CRGB w = CRGB::White;
    switch (secondHand) {
      case 0: targetPalette = RainbowColors_p; break;
      case 5: targetPalette = CRGBPalette16(u, u, b, b, p, p, b, b, u, u, b, b, p, p, b, b); break;
      case 10: targetPalette = OceanColors_p; break;
      case 15: targetPalette = CloudColors_p; break;
      case 20: targetPalette = LavaColors_p; break;
      case 25: targetPalette = ForestColors_p; break;
      case 30: targetPalette = PartyColors_p; break;
      case 35: targetPalette = CRGBPalette16(b, b, b, w, b, b, b, w, b, b, b, w, b, b, b, w); break;
      case 40: targetPalette = CRGBPalette16(u, u, u, w, u, u, u, w, u, u, u, w, u, u, u, w); break;
      case 45: targetPalette = CRGBPalette16(u, p, u, w, p, u, u, w, u, g, u, w, u, p, u, w); break;
      case 50: targetPalette = CloudColors_p; break;
      case 55: targetPalette = CRGBPalette16(u, u, u, w, u, u, p, p, u, p, p, p, u, p, p, w); break;
      case 60: break;
    }
  }
}
// @400e033c
void three_sin() {
  wave1 += beatsin8(10, -4, 4);
  wave2 += beatsin8(15, -2, 2);
  wave3 += beatsin8(12, -3, 3);
  for (int k = 0; k < NUM_LEDS; k++) {
    uint8_t tmp = sin8(g_bec16 * k + wave1) + sin8(g_bec16 * k + wave2) + sin8(g_bec16 * k + wave3);
    LEDS[k] = ColorFromPalette(currentPalette, tmp, 255);
  }
}
// @400e03e8: 16
void fx_three_sin() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  ChangeMe_three_sin();
  EVERY_N_MILLISECONDS(g_bec17) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, 24);
    three_sin();
  }
}
// @400e0464: початкові значення two_sin (resetvars)
void led_defaults() {
  g_c226c = 0;
  thathue = 140;
  thisrot = 1;
  thatrot = 1;
  allsat = 255;
  thisdir = 0;
  thatdir = 0;
  alldir = 0;
  thisspeed = 4;
  thatspeed = 4;
  allfreq = 32;
  thisphase = 0;
  thatphase = 0;
  thiscutoff = 192;
  thatcutoff = 192;
  g_bec17 = 10;
  fadeval = 192;
}
// @400e04e0
void two_sin() {
  thisdir ? thisphase += beatsin8(thisspeed, 2, 10) : thisphase -= beatsin8(thisspeed, 2, 10);
  thatdir ? thatphase += beatsin8(thisspeed, 2, 10) : thatphase -= beatsin8(thatspeed, 2, 10);
  g_c226c += thisrot;
  thathue += thatrot;
  for (int k = 0; k < NUM_LEDS - 1; k++) {
    int thisbright = qsuba(cubicwave8((k * allfreq) + thisphase), thiscutoff);
    int thatbright = qsuba(cubicwave8((k * allfreq) + 128 + thatphase), thatcutoff);
    LEDS[k] = ColorFromPalette(thisPalette, g_c226c, thisbright, currentBlending);
    LEDS[k] += ColorFromPalette(thatPalette, thathue, thatbright, currentBlending);
  }
  nscale8(LEDS, NUM_LEDS, fadeval);
}
// @400e0644: зміни параметрів two_sin (цикл 60 с); палітри щоразу скидаються на веселку
void ChangeMe_two_sin() {
  thisPalette = RainbowColors_p;
  thatPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
  uint8_t secondHand = (millis() / 1000) % 60;
  if (g_bebfe != secondHand) {    // static lastSecond @3ffbebfe
    g_bebfe = secondHand;
    switch (secondHand) {
      case 0: thisrot = 1; thatrot = 1; thisPalette = PartyColors_p; thatPalette = PartyColors_p; break;
      case 5: thisrot = 0; thatdir = 1; thatspeed = -4; thisPalette = ForestColors_p; thatPalette = OceanColors_p; break;
      case 10: thatrot = 0; thisPalette = PartyColors_p; thatPalette = RainbowColors_p; break;
      case 15: allfreq = 16; thisdir = 1; thathue = 128; break;
      case 20: thiscutoff = 96; thatcutoff = 240; break;
      case 25: thiscutoff = 96; thatdir = 0; thatcutoff = 96; thisrot = 1; break;
      case 30: thisspeed = -4; thisdir = 0; thatspeed = -4; break;
      case 35: thiscutoff = 128; thatcutoff = 128; break;
      case 40: thisspeed = 3; break;
      case 45: thisspeed = 3; thatspeed = -3; break;
      case 50: thisspeed = 2; thatcutoff = 96; thiscutoff = 224; thatspeed = 3; break;
      case 55: led_defaults(); break;
      case 60: break;
    }
  }
}
// @400e0818: 17
void fx_two_sin() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  ChangeMe_two_sin();
  EVERY_N_MILLISECONDS(g_bec17) { two_sin(); }
}
// @400e0884
void ripples_draw() {
  for (int i = 0; i < 6; i += 2) {
    if (random8() > 224 && !ripples[i].exist) {
      ripples[i].Init(192, 10);
      ripples[i + 1] = ripples[i];
      ripples[i + 1].velocity *= -1;
    }
  }
  for (int i = 0; i < 6; i++) {
    if (ripples[i].exist) {
      LEDS[ripples[i].pos] = ColorFromPalette(currentPalette, ripples[i].color, ripples[i].bright, LINEARBLEND);
      ripples[i].Move();
    }
  }
  fadeToBlackBy(LEDS, NUM_LEDS, 160);
}
// @400e6830
void SetupRandomPalette() {
  uint8_t baseC = random8();
  targetPalette = CRGBPalette16(CHSV(baseC + random8(32), 255, random8(128, 255)),
                                CHSV(baseC + random8(32), 255, random8(192, 255)),
                                CHSV(baseC + random8(32), 192, random8(192, 255)),
                                CHSV(baseC + random8(32), 255, random8(128, 255)));
}
// @400e68c8: 18
void fx_ripples() {
  EVERY_N_MILLISECONDS(50) { nblendPaletteTowardPalette(currentPalette, targetPalette, 24); }
  EVERY_N_SECONDS(5) { SetupRandomPalette(); }
  EVERY_N_MILLISECONDS(50) { ripples_draw(); }
}
// @400e09b4
void plasma() {
  int thisPhase = beatsin8(6, -64, 64);
  int thatPhase = beatsin8(7, -64, 64);
  for (int k = 0; k < NUM_LEDS; k++) {
    int colorIndex = cubicwave8((k * 23) + thisPhase) / 2 + cos8((k * 15) + thatPhase) / 2;
    int thisBright = qsuba(colorIndex, beatsin8(7, 0, 96));
    LEDS[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);
  }
}
// @400e69a0: 19
void fx_plasma() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  EVERY_N_MILLISECONDS(50) { plasma(); }
  EVERY_N_MILLISECONDS(100) { nblendPaletteTowardPalette(currentPalette, targetPalette, 24); }
  EVERY_N_SECONDS(5) {
    uint8_t baseC = random8();
    targetPalette = CRGBPalette16(CHSV(baseC + random8(32), 192, random8(128, 255)),
                                  CHSV(baseC + random8(32), 255, random8(128, 255)),
                                  CHSV(baseC + random8(32), 192, random8(128, 255)),
                                  CHSV(baseC + random8(32), 255, random8(128, 255)));
  }
}
// @400e0a50
void palette_wave(uint8_t colorIndex) {
  uint8_t s = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    LEDS[i] = ColorFromPalette(currentPalette, colorIndex + sin8(s), 255, LINEARBLEND);
    colorIndex += 3;
    s += 16;
  }
}
// @400e0aa8
void ChangeMe_wave() {
  uint8_t secondHand = (millis() / 1000) % 60;
  if (g_bebfd != secondHand) {    // static lastSecond @3ffbebfd
    g_bebfd = secondHand;
    CRGB p = CHSV(HUE_PURPLE, 255, 255);
    CRGB g = CHSV(HUE_GREEN, 255, 255);
    CRGB b = CRGB::Black;
    CRGB w = CRGB::White;
    if (secondHand == 0) targetPalette = RainbowColors_p;
    if (secondHand == 10) targetPalette = CRGBPalette16(g, g, b, b, p, p, b, b, g, g, b, b, p, p, b, b);
    if (secondHand == 20) targetPalette = CRGBPalette16(b, b, b, w, b, b, b, w, b, b, b, w, b, b, b, w);
    if (secondHand == 30) targetPalette = LavaColors_p;
    if (secondHand == 40) targetPalette = CloudColors_p;
    if (secondHand == 50) targetPalette = PartyColors_p;
  }
}
// @400e0bdc: 20
void fx_palette_wave() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  ChangeMe_wave();
  EVERY_N_MILLISECONDS(100) { nblendPaletteTowardPalette(currentPalette, targetPalette, 24); }
  EVERY_N_MILLISECONDS(g_bec17) {
    static uint8_t startIndex = 0;   // @3ffc17dc
    startIndex++;
    palette_wave(startIndex);
  }
}
// @400e0c9c
void palette_comets() {
  uint8_t colorIndex = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    uint8_t c = colorIndex + millis() / 5;
    if (c > 128) c = 0;
    LEDS[i] = ColorFromPalette(currentPalette, c, c, currentBlending);
    colorIndex += 12;
  }
}
// @400e6b3c: 21
void fx_palette_comets() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  EVERY_N_MILLISECONDS(100) { nblendPaletteTowardPalette(currentPalette, targetPalette, 24); }
  EVERY_N_SECONDS(5) {
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)),
                                  CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }
  palette_comets();
}
// @400e0cfc
void beat_runner(uint8_t colorIndex) {
  static int sinBeat = 0;   // @3ffc17cc
  sinBeat += beatsin8(1, 20, 50);
  uint8_t cutoff = beatsin8(50, 164, 248);
  int thisbright = qsubd(cubicwave8(sinBeat), cutoff);
  LEDS[0] = ColorFromPalette(currentPalette, colorIndex, thisbright, currentBlending);
  memmove(LEDS + 1, LEDS, (NUM_LEDS - 1) * 3);
}
// @400e6c80: 22
void fx_beat_runner() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  EVERY_N_MILLIS_I(thisTimer, 100) {
    thisTimer.setPeriod(beatsin8(10, 20, 50));
    beat_runner(millis() >> 4);
  }
  EVERY_N_MILLISECONDS(100) { nblendPaletteTowardPalette(currentPalette, targetPalette, 24); }
  EVERY_N_SECONDS(5) {
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)),
                                  CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }
}
// @400e0d70: «матриця» (palIndex = g_bec15, thisbright = g_bec14)
void matrix() {
  if (huerot) g_bec15++;
  if (random8(90) > 80) {
    if (thisdir == 0) LEDS[0] = ColorFromPalette(currentPalette, g_bec15, g_bec14, currentBlending);
    else LEDS[NUM_LEDS - 1] = ColorFromPalette(currentPalette, g_bec15, g_bec14, currentBlending);
  } else {
    if (thisdir == 0) LEDS[0] = CHSV(bgclr, g_bec28, bgbri);
    else LEDS[NUM_LEDS - 1] = CHSV(bgclr, g_bec28, bgbri);
  }
  if (thisdir == 0) {
    for (int i = NUM_LEDS - 1; i > 0; i--) LEDS[i] = LEDS[i - 1];
  } else {
    for (int i = 0; i < NUM_LEDS - 1; i++) LEDS[i] = LEDS[i + 1];
  }
}
// @400e0e9c: зміни «матриці» (цикл 25 с)
void ChangeMe_matrix() {
  uint8_t secondHand = (millis() / 1000) % 25;
  if (g_bebfc != secondHand) {    // static lastSecond @3ffbebfc
    g_bebfc = secondHand;
    switch (secondHand) {
      case 0: g_bec17 = 50; g_bec15 = 95; bgclr = 140; bgbri = 4; huerot = 0; break;
      case 5: targetPalette = OceanColors_p; huerot = 1; thisdir = 1; bgbri = 0; break;
      case 10: targetPalette = LavaColors_p; g_bec17 = 30; huerot = 0; g_bec15 = 0; bgclr = 50; bgbri = 8; break;
      case 15: targetPalette = ForestColors_p; g_bec17 = 80; bgbri = 16; bgclr = 96; g_bec15 = random8(); break;
      case 20: g_bec15 = random8(); huerot = 1; break;
      case 25: break;
    }
  }
}
// @400e0fa4: 23
void fx_matrix() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  ChangeMe_matrix();
  EVERY_N_MILLISECONDS(100) { nblendPaletteTowardPalette(currentPalette, targetPalette, 24); }
  EVERY_N_MILLISECONDS(g_bec17) { matrix(); }
}
// @400e1058
void beatwave() {
  uint8_t wave1 = beatsin8(9, 0, 255);
  uint8_t wave2 = beatsin8(8, 0, 255);
  uint8_t wave3 = beatsin8(7, 0, 255);
  uint8_t wave4 = beatsin8(6, 0, 255);
  for (int i = 0; i < NUM_LEDS; i++) LEDS[i] = ColorFromPalette(currentPalette, i + wave1 + wave2 + wave3 + wave4, 255, currentBlending);
}
// @400e6e18: 24
void fx_beatwave() {
  fadeToBlackBy(LEDS, NUM_LEDS, 20);
  beatwave();
  EVERY_N_MILLISECONDS(100) { nblendPaletteTowardPalette(currentPalette, targetPalette, 24); }
  EVERY_N_SECONDS(5) {
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)),
                                  CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }
}

// ---------- індикатор CO2 (режим стрічки «CO2») ----------
// @400e10d4: «дихання» яскравості між g_bec0e і 255 з кроком g_bec0f
void co2_breathe(uint32_t ms) {
  if (millis() - g_c178c < ms) return;
  g_c178c = millis();
  if (g_bec10 <= g_bec0e) {
    g_bec10 = g_bec0e;
    g_c1c6c = 1;
  } else if (g_bec10 > 253) {
    g_bec10 = 255;
    g_c1c6c = 0;
  }
  g_bec10 += g_c1c6c ? g_bec0f : -g_bec0f;
}
// @400e113c: плавний перехід відтінку до рівня (1 — зелений … 6 — червоний)
void co2_hue(int8_t level, uint32_t ms) {
  if (millis() - g_c1788 < ms) return;
  g_c1788 = millis();
  switch (level) {
    case 1:
      if (g_bec11 < 97) { g_bec11++; return; }
      g_bec12 = 1;
      break;
    case 2:
      if (g_bec12 < 3) { if (g_bec12 != 2 && g_bec11 > 80) { g_bec11--; return; } }
      else if (g_bec11 < 80) { g_bec11++; return; }
      g_bec12 = 2;
      break;
    case 3:
      if (g_bec12 < 4) { if (g_bec12 != 3 && g_bec11 > 70) { g_bec11--; return; } }
      else if (g_bec11 < 70) { g_bec11++; return; }
      g_bec12 = 3;
      break;
    case 4:
      if (g_bec12 < 5) { if (g_bec12 != 4 && g_bec11 > 34) { g_bec11--; return; } }
      else if (g_bec11 < 34) { g_bec11++; return; }
      g_bec12 = 4;
      break;
    case 5:
      if (g_bec12 < 6) { if (g_bec12 != 5 && g_bec11 > 12) { g_bec11--; return; } }
      else if (g_bec11 < 12) { g_bec11++; return; }
      g_bec12 = 5;
      break;
    case 6:
      if (g_bec11 != 0) { g_bec11--; return; }
      g_bec12 = 6;
      break;
  }
}
// @400e125c: колір і «дихання» за рівнем eCO2 (g_c32f0)
void co2_indicator() {
  led_show();
  if (g_c32f0 < 2001) {
    if (g_c32f0 - 1500 < 500) { g_c1c6d = 60; g_bec0f = 5; g_bec13 = 5; g_bec0e = 50; }
    else if (g_c32f0 - 1000 < 499) { g_bec0f = 5; g_bec13 = 4; g_c1c6d = 80; g_bec0e = 80; }
    else if (g_c32f0 - 700 < 299) { g_c1c6d = 100; g_bec0f = 5; g_bec13 = 3; g_bec0e = 80; }
    else if (g_c32f0 - 550 < 149) { g_c1c6d = 150; g_bec0f = 2; g_bec13 = 2; g_bec0e = 100; }
    else if (g_c32f0 < 549) { g_c1c6d = 200; g_bec0f = 1; g_bec13 = 1; g_bec0e = 150; }
  } else {
    g_c1c6d = 40;
    g_bec0f = 5;
    g_bec13 = 6;
    g_bec0e = 0;
  }
  co2_breathe(g_c1c6d);
  co2_hue(g_bec13, 90);
  CRGB c = CHSV(g_bec11, 255, g_bec10);
  led_fill_all(c);
  FastLED.show();
  g_c349d = 0;
}
// @400e1394: найбільший вільний блок пам'яті
uint32_t heap_max_block() { return ESP.getMaxAllocHeap(); }

// ---------- вимкнення й будильник ----------
// @400e4224
void led_off() {
  FastLED.clear(true);
  FastLED.show();
}
// @400e423c: стрічка під час будильника (g_c2fc5: 1 — дихання, 2 — наростання білого з яскравістю табло, 3 — веселка, 4 — блимання)
void led_alarm_tick() {
  if (millis() - g_c1764 < g_c1a90) return;
  g_c1764 = millis();
  if (g_c2fc5 == 2) {
    g_c1a8a++;
    CRGB c = CHSV(255, 0, 255);
    led_fill_all(c);
    FastLED.setBrightness(g_c1a8a);
    FastLED.show();
    uint8_t in;
    switch (g_c1a8a) {
      case 0x00: in = 0; break;
      case 0x10: in = 1; break;
      case 0x20: in = 2; break;
      case 0x30: in = 3; break;
      case 0x40: in = 4; break;
      case 0x50: in = 5; break;
      case 0x60: in = 6; break;
      case 0x70: in = 7; break;
      case 0x80: in = 8; break;
      case 0x90: in = 9; break;
      case 0xa0: in = 10; break;
      case 0xb0: in = 11; break;
      case 0xc0: in = 12; break;
      case 0xd0: in = 13; break;
      case 0xe0: in = 14; break;
      case 0xfa: in = 15; break;
      default: return;
    }
    P.setIntensity(in);
    return;
  }
  if (g_c2fc5 == 1) {
    fx_breathe();
    FastLED.setBrightness(255);
  } else if (g_c2fc5 == 3) {
    fx_hue_cycle();
    FastLED.setBrightness(250);
  } else if (g_c2fc5 == 4) {
    g_c1760 = !g_c1760;
    if (g_c1760) {
      FastLED.setBrightness(254);
      FastLED.show();
    } else {
      led_off();
    }
    g_c1a94++;
    return;
  } else {
    return;
  }
  FastLED.show();
}

// ---------- вибір ефекту ----------
// @400e6f5c
void led_effect_run() {
  if (g_c2f27 == 27) {
    led_random_tick(g_c2f23 * 1000);
  } else {
    if (g_c34ac != g_c2f27) {
      g_c34ac = g_c2f27;
      g_c1f18 = 0;
    }
    if (g_c2f27 == 26 && g_c34aa != g_c2f26) g_c34aa = g_c2f26;
  }
  if (g_c34ac == 25 && g_c2f25 != 1) {
    // статичний колір: перемальовувати лише при зміні (або не частіше ніж раз на 250 мс після 1,5 с сталості)
    uint32_t bri = g_c2f24;
    bool same;
    if (g_c349d == 0 && g_c1798 != 0 && g_c2f1e == g_bebf8 && bri == g_bebf4 && g_bf042 == g_bebf0) same = true;
    else {
      g_c1794 = millis();
      same = false;
    }
    if (millis() - g_c1794 > 1500 && same && millis() - g_c1790 < 250) return;
    g_c1798 = 1;
    g_bebf8 = g_c2f1e;
    g_bebf0 = g_bf042;
    g_bebf4 = bri;
    g_c1790 = millis();
    fx_static_color();
    led_show();
  } else {
    g_c1798 = 0;
    led_show();
    switch (g_c34ac) {
      case 0: fx_rainbow_step(); break;
      case 1: fx_hue_cycle(); break;
      case 2: fx_breathe(); break;
      case 3: fx_bouncing_balls(); break;
      case 4: fx_dot(); break;
      case 5: fx_dot_tails(); break;
      case 6: fx_random_march(); break;
      case 7: fx_rainbow(); break;
      case 8: Fire(55, 120); break;
      case 9: fx_fire_palette(); break;
      case 10: fx_confetti(); break;
      case 11: fx_bpm(); break;
      case 12: fx_pride(); break;
      case 13: fx_colorwaves(); break;
      case 14: fx_sinelon(); break;
      case 15: fx_juggle(); break;
      case 16: fx_three_sin(); break;
      case 17: fx_two_sin(); break;
      case 18: fx_ripples(); break;
      case 19: fx_plasma(); break;
      case 20: fx_palette_wave(); break;
      case 21: fx_palette_comets(); break;
      case 22: fx_beat_runner(); break;
      case 23: fx_matrix(); break;
      case 24: fx_beatwave(); break;
      case 25: fx_static_color(); break;
      case 26:
        switch (g_c34aa) {
          case 1: tw_cloud(); break;
          case 2: tf_cloud(); break;
          case 3: tw_rainbow(); break;
          case 4: tw_snow(); break;
          case 5: tf_snow(); break;
          case 6: tw_fairy(); break;
          case 7: tf_red_green_white(); break;
          case 8: tf_holly(); break;
          case 9: tf_red_white(); break;
          case 10: tf_blue_white(); break;
          case 11: tf_fairy_light(); break;
          case 12: tf_ice(); break;
          case 13: tf_retro_c9(); break;
          case 14: tf_party(); break;
          case 15: tf_forest(); break;
          case 16: tf_lava(); break;
          case 17: tf_heat(); break;
          case 18: tf_ocean(); break;
        }
        break;
    }
  }
  g_c349d = 0;
}
// @400e7224: стрічка за режимом g_c2f28 (0 — вимкнена, 1 — завжди, 2 — за розкладом), кадр раз на ms
void led_fx_tick(uint32_t ms) {
  if (g_c2f28 == 1) {
    if (millis() - g_c2e90 < ms) return;
    g_c2e90 = millis();
  } else if (g_c2f28 == 0) {
    if (g_c349d == 0) {
      led_off();
      g_c349d = 1;
    }
    return;
  } else if (g_c2f28 == 2) {
    if (millis() - g_c2e90 < ms) return;
    g_c2e90 = millis();
    led_schedule_check();
    if (g_c2274 == 0) {
      if (g_c349d == 0) {
        led_off();
        g_c349d = 1;
      }
      return;
    }
    if (g_c2274 != 1) return;
  } else {
    return;
  }
  led_effect_run();
}
// @400f3e04: режим «CO2» (без датчика повітря — повернутися до ефектів)
void led_co2_mode() {
  if (g_c32e1 == 0) {
    g_c2f22 = g_c32e1;
    config_set(String("Sl"), String("0"));
  }
  if (g_c2f28 == 1) {
    co2_indicator();
  } else if (g_c2f28 == 0) {
    if (g_c349d == 0) {
      led_off();
      g_c349d = 1;
    }
  } else if (g_c2f28 == 2) {
    led_schedule_check();
    if (g_c2274 == 0) {
      if (g_c349d == 0) {
        led_off();
        g_c349d = 1;
      }
    } else if (g_c2274 == 1) {
      co2_indicator();
    }
  }
}
// @400f3ecc: такт стрічки з циклу (g_c2f22: 0 — ефекти, 1 — індикатор CO2)
void led_tick() {
  if (g_c2f22 == 0) {
    led_fx_tick(g_c2f29);
    return;
  }
  if (g_c2f22 == 1) led_co2_mode();
}
