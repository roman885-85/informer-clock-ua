/*
 * Clock Informer — 10 ефектів зміни циферблата автора прошивки 2.7.1 (коди 29–38), відновлені з машинного
 * коду прошивки (MD_PZone::zoneAnimate → 0x4013ad3c … 0x4013bdb4). В оригіналі — окремий файл бібліотеки після
 * MD_Parola_Wipe.cpp. Принцип у всіх: commonPrint() виводить текст зони в буфер, далі за кроком анімації _nextPos
 * маска/перенесення пікселів через getColumn/setColumn.
 * Перевірка: поведінка кожного ефекту збігається з оригіналом на випадкових сценаріях (під час реконструкції машинний
 * код оригіналу й копії виконувався в емуляторі Xtensa й порівнювався кадр за кадром); машинний код ciNoise і ефектів
 * Melt/Rain/Spiral/Pixelate — побайтно, решти — відрізняється розподілом регістрів і порядком блоків (не поведінкою).
 *
 * EN: the 10 clock-face transition effects of Clock Informer 2.7.1 by Semen_p (codes 29–38: Neon, Melt, Rain, Shatter,
 * Spiral, Pixelate, Jitter, Cascade, Comb, Iris), reconstructed from the original machine code and verified in an Xtensa
 * emulator frame by frame. Part of the MD_Parola fork (LGPL-2.1); effect designs credited to Semen_p.
 */
#include <MD_Parola.h>
#include <MD_Parola_lib.h>

// шум автора (0x4013ad1c)
static uint8_t ciNoise(int16_t x) {
  uint16_t m = (uint16_t)((int32_t)x * 0x79b1);
  uint16_t v = (uint16_t)((m >> 7) ^ m);
  return (uint8_t)(((v >> 3) ^ v) & 0xff);
}
// порядковий номер пікселя (x,y) при обході спіраллю сітки W×8 (0x401d9ec4; в оригіналі — inline-функція з заголовка)
static int16_t ciSpiralIdx(int16_t x, int16_t y, int16_t W)
{
  int16_t ring = (x < y) ? x : y;
  if (W - 1 - x < ring) ring = W - 1 - x;
  if (7 - y < ring) ring = 7 - y;
  x -= ring;
  y -= ring;
  if (y != 0)
  {
    int16_t m2 = -2 * ring;
    int16_t iw = W + m2;
    if (x == iw - 1) x = iw - 1 + y;
    else
    {
      int16_t ih = 8 + m2;
      if (y == ih - 1) x = (iw - 1) + (ih - 1) + (iw - 1 - x);
      else x = 2 * (iw - 1) + (ih - 1) + (ih - 1 - y);
    }
  }
  return (W + 8) * 2 * ring + x;
}
static const int8_t CI_JDX[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
static const int8_t CI_JDY[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };

// --- спільний каркас «A»: старт для входу (INITIALISE) / виходу (INITIALISE|PAUSE), робота в станах 1..3
#define CI_START_A(bIn) \
  if ((bIn) ? (_fsmState == INITIALISE) : (_fsmState == PAUSE || _fsmState == INITIALISE)) { _nextPos = 0; _fsmState = PUT_CHAR; }
#define CI_ACTIVE (_fsmState == GET_FIRST_CHAR || _fsmState == GET_NEXT_CHAR || _fsmState == PUT_CHAR)
#define CI_FINISH(bIn) { _fsmState = (bIn) ? PAUSE : END; }

static inline uint8_t ciCol(MD_MAX72XX *mx, uint16_t c, bool inv) { uint8_t v = mx->getColumn(c); return inv ? (uint8_t)~v : v; }

void MD_PZone::effectCiNeon(bool bIn)
{
  uint8_t zs = _zoneStart;
  uint8_t ze = _zoneEnd;

  if (bIn ? (_fsmState == INITIALISE) : (_fsmState == PAUSE || _fsmState == INITIALISE))
  {
    _nextPos = 0;
    _fsmState = PUT_CHAR;
  }

  if (_fsmState == GET_FIRST_CHAR || _fsmState == GET_NEXT_CHAR || _fsmState == PUT_CHAR)
  {
    commonPrint();
    int16_t step = _nextPos;
    int16_t prob;
    if (bIn) prob = (step * 100) / 12;
    else prob = 100 - (step * 100) / 12;
    bool full = (step == 1) || (step == 4) || (bIn ? (step == 8) : (step == 2));
    for (int16_t c = zs * 8; c <= ze * 8 + 7; c++)
    {
      uint8_t col = DATA_BAR(_MX->getColumn(c));
      uint8_t out = 0;
      for (uint8_t r = 0; r < 8; r++)
      {
        if (col & (1 << r))
        {
          if (full) out |= (1 << r);
          else
          {
            uint8_t n = ciNoise((c * 8 + r) * 31 + step * 7) % 100;
            if (n < prob) out |= (1 << r);
          }
        }
      }
      _MX->setColumn(c, DATA_BAR(out));
    }
    if (_nextPos < 12) { _nextPos++; return; }
  }
  else if (_fsmState == END) return;
  _fsmState = bIn ? PAUSE : END;
}

// --- спільний каркас «B» (Melt/Rain/Spiral/Pixelate) — як у автора
#define CI_B_BEGIN(bIn) \
  bool ciRun = false; \
  if (!(bIn)) { \
    if (_fsmState <= PUT_FILLER || _fsmState == PAUSE) { \
      if (_fsmState == PUT_FILLER) { _fsmState = END; return; } \
      if (_fsmState == INITIALISE || _fsmState == PAUSE) { _nextPos = 0; _fsmState = PUT_CHAR; } \
      ciRun = true; \
    } else { _fsmState = END; return; } \
  } else { \
    if (_fsmState == INITIALISE) { _nextPos = 0; _fsmState = PUT_CHAR; ciRun = true; } \
    else if (_fsmState < PUT_FILLER) ciRun = true; \
    else { _fsmState = PAUSE; return; } \
  }

void MD_PZone::effectCiMelt(bool bIn)
{
  int16_t startCol = _zoneStart * 8;
  int16_t endCol = _zoneEnd * 8 + 7;

  if (bIn)
  {
    switch (_fsmState)
    {
    case INITIALISE:
      _nextPos = 0;
      _fsmState = PUT_CHAR;
      // fall through
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      commonPrint();
      for (int16_t c = startCol; c <= endCol; c++)
      {
        int16_t v = _nextPos - (ciNoise(c) & 3);
        if (v < 0) v = 0;
        if (v > 8) v = 8;
        uint8_t mask = v ? (0xff << (8 - v)) : 0;
        uint8_t col = DATA_BAR(_MX->getColumn(c)) & mask;
        _MX->setColumn(c, DATA_BAR(col));
      }
      if (_nextPos > 10) _fsmState = PAUSE;
      else _nextPos++;
      break;

    default:
      _fsmState = PAUSE;
    }
  }
  else
  {
    switch (_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      _nextPos = 0;
      _fsmState = PUT_CHAR;
      // fall through
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      commonPrint();
      for (int16_t c = startCol; c <= endCol; c++)
      {
        int16_t v = _nextPos - (ciNoise(c) & 3);
        if (v < 0) v = 0;
        if (v > 8) v = 8;
        uint8_t mask = (v < 8) ? (0xff << v) : 0;
        uint8_t col = DATA_BAR(_MX->getColumn(c)) & mask;
        _MX->setColumn(c, DATA_BAR(col));
      }
      if (_nextPos > 10) _fsmState = END;
      else _nextPos++;
      break;

    default:
      _fsmState = END;
    }
  }
}

void MD_PZone::effectCiRain(bool bIn)
{
  int16_t startCol = _zoneStart * 8;
  int16_t endCol = _zoneEnd * 8 + 7;

  if (bIn)
  {
    switch (_fsmState)
    {
    case INITIALISE:
      _nextPos = 0;
      _fsmState = PUT_CHAR;
      // fall through
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      commonPrint();
      for (int16_t c = startCol; c <= endCol; c++)
      {
        int16_t v = _nextPos - (ciNoise(c + 7) & 3);
        uint8_t mask;
        if (v < 0) mask = 0;
        else if (v >= 7) mask = 0xff;
        else mask = (1 << (v + 1)) - 1;
        uint8_t col = DATA_BAR(_MX->getColumn(c)) & mask;
        if (v >= 0 && v < 8) col |= (1 << v);
        _MX->setColumn(c, DATA_BAR(col));
      }
      if (_nextPos > 10) _fsmState = PAUSE;
      else _nextPos++;
      break;

    default:
      _fsmState = PAUSE;
    }
  }
  else
  {
    switch (_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      _nextPos = 0;
      _fsmState = PUT_CHAR;
      // fall through
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
      commonPrint();
      for (int16_t c = startCol; c <= endCol; c++)
      {
        int16_t v = _nextPos - (ciNoise(c + 7) & 3);
        uint8_t mask;
        if (v >= 7) mask = 0;
        else if (v < 0) mask = 0xff;
        else mask = 0xff << (v + 1);
        uint8_t col = DATA_BAR(_MX->getColumn(c)) & mask;
        if (v >= 0 && v < 8) col |= (1 << v);
        _MX->setColumn(c, DATA_BAR(col));
      }
      if (_nextPos > 10) _fsmState = END;
      else _nextPos++;
      break;

    default:
      _fsmState = END;
    }
  }
}

void MD_PZone::effectCiShatter(bool bIn) {
  CI_START_A(bIn);
  if (CI_ACTIVE) {
    commonPrint();
    int16_t start = _zoneStart * 8, endc = _zoneEnd * 8 + 7;
    int16_t center = (int16_t)((uint16_t)(endc + start) >> 1);
    for (int16_t c = start; c <= endc; c++) {
      uint8_t col = ciCol(_MX, c, _inverted);
      int16_t dist = c - center; if (dist < 0) dist = -dist;
      uint8_t out = 0;
      for (uint8_t r = 0; r < 8; r++) {
        int16_t ay = (int16_t)r - 3; if (ay < 0) ay = -ay;
        int16_t d = dist + ay + (ciNoise((int16_t)(((uint16_t)(c << 3)) ^ r)) & 3);
        bool lit = bIn ? (d <= _nextPos) : (d > _nextPos);
        if (lit) out |= (1 << r);
      }
      if (_inverted) col = ~col;          // у автора — повторна інверсія
      _MX->setColumn(c, out & col);
    }
    if (_nextPos <= (int16_t)((endc - start) / 2 + 8)) { _nextPos++; return; }
  } else if (_fsmState == END) return;
  CI_FINISH(bIn);
}

void MD_PZone::effectCiSpiral(bool bIn)
{
  int16_t startCol = _zoneStart * 8;
  int16_t endCol = _zoneEnd * 8 + 7;
  int16_t W = endCol - startCol + 1;
  uint16_t rings = (W < 8) ? (W / 2 + 1) : 5;
  int16_t total = (W + 8) * 2 * rings;
  int16_t per = total / 48;
  if (per < 1) per = 1;
  int16_t limit = total / per + 1;

  if (bIn)
  {
    switch (_fsmState)
    {
    case INITIALISE:
      _nextPos = 0;
      _fsmState = PUT_CHAR;
      // fall through
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    {
      commonPrint();
      int16_t thr = _nextPos * per;
      for (int16_t c = startCol; c <= endCol; c++)
      {
        uint8_t col = DATA_BAR(_MX->getColumn(c));
        uint8_t out = 0;
        for (uint8_t r = 0; r < 8; r++)
        {
          int16_t idx = ciSpiralIdx(c - startCol, r, W);
          if (idx > thr - per && idx <= thr) out |= (1 << r);
          else if (idx <= thr && (col & (1 << r))) out |= (1 << r);
        }
        _MX->setColumn(c, DATA_BAR(out));
      }
      if (_nextPos >= limit) _fsmState = PAUSE;
      else _nextPos++;
      break;
    }
    default:
      _fsmState = PAUSE;
    }
  }
  else
  {
    switch (_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      _nextPos = 0;
      _fsmState = PUT_CHAR;
      // fall through
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    {
      commonPrint();
      int16_t thr = _nextPos * per;
      for (int16_t c = startCol; c <= endCol; c++)
      {
        uint8_t col = DATA_BAR(_MX->getColumn(c));
        uint8_t out = 0;
        for (uint8_t r = 0; r < 8; r++)
        {
          int16_t idx = ciSpiralIdx(c - startCol, r, W);
          if (idx > thr - per && idx <= thr) out |= (1 << r);
          else if (idx > thr && (col & (1 << r))) out |= (1 << r);
        }
        _MX->setColumn(c, DATA_BAR(out));
      }
      if (_nextPos >= limit) _fsmState = END;
      else _nextPos++;
      break;
    }
    default:
      _fsmState = END;
    }
  }
}

void MD_PZone::effectCiPixelate(bool bIn)
{
  int16_t startCol = _zoneStart * 8;
  int16_t endCol = _zoneEnd * 8 + 7;

  if (bIn)
  {
    switch (_fsmState)
    {
    case INITIALISE:
      _nextPos = 0;
      _fsmState = PUT_CHAR;
      // fall through
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    {
      commonPrint();
      int16_t step = _nextPos;
      for (int16_t c = startCol; c <= endCol; c++)
      {
        int16_t bx = (c - startCol) >> 1;
        uint8_t col = DATA_BAR(_MX->getColumn(c));
        uint8_t out = 0;
        for (uint8_t r = 0; r < 8; r++)
        {
          int16_t n = ciNoise(bx * 97 + (r >> 1)) & 7;
          if (n <= step && (col & (1 << r))) out |= (1 << r);
        }
        _MX->setColumn(c, DATA_BAR(out));
      }
      if (_nextPos > 7) _fsmState = PAUSE;
      else _nextPos++;
      break;
    }
    default:
      _fsmState = PAUSE;
    }
  }
  else
  {
    switch (_fsmState)
    {
    case PAUSE:
    case INITIALISE:
      _nextPos = 0;
      _fsmState = PUT_CHAR;
      // fall through
    case GET_FIRST_CHAR:
    case GET_NEXT_CHAR:
    case PUT_CHAR:
    {
      commonPrint();
      int16_t step = _nextPos;
      for (int16_t c = startCol; c <= endCol; c++)
      {
        int16_t bx = (c - startCol) >> 1;
        uint8_t col = DATA_BAR(_MX->getColumn(c));
        uint8_t out = 0;
        for (uint8_t r = 0; r < 8; r++)
        {
          int16_t n = ciNoise(bx * 97 + (r >> 1)) & 7;
          if (n > step && (col & (1 << r))) out |= (1 << r);
        }
        _MX->setColumn(c, DATA_BAR(out));
      }
      if (_nextPos > 7) _fsmState = END;
      else _nextPos++;
      break;
    }
    default:
      _fsmState = END;
    }
  }
}

void MD_PZone::effectCiJitter(bool bIn) {
  CI_START_A(bIn);
  if (CI_ACTIVE) {
    int16_t start = _zoneStart * 8;
    int16_t W = (_zoneEnd + 1) * 8 - start;
    commonPrint();
    if (W <= 128) {
      uint8_t src[128], dst[128];
      for (int16_t i = 0; i < W; i++) { src[i] = ciCol(_MX, start + i, _inverted); dst[i] = 0; }
      int16_t s = _nextPos, t = bIn ? (12 - s) : s;
      int16_t amp = (t * 5) / 12;
      for (int16_t i = 0; i < W; i++)
        for (uint8_t k = 0; k < 8; k++) {
          if (!(src[i] & (1 << k))) continue;
          uint8_t n = ciNoise((int16_t)(((uint16_t)(i << 3)) ^ k));
          if (!bIn && s > (int16_t)(n % 12)) continue;
          int16_t nx = (int16_t)(CI_JDX[n & 7] * amp + i);
          int16_t ny = (int16_t)(CI_JDY[n & 7] * amp + k);
          if (nx >= 0 && nx < W && ny >= 0 && ny < 8) dst[nx] |= (uint8_t)(1 << ny);
        }
      for (int16_t i = 0; i < W; i++) _MX->setColumn(start + i, _inverted ? (uint8_t)~dst[i] : dst[i]);
    }
    if (_nextPos < 12) { _nextPos++; return; }
  } else if (_fsmState == END) return;
  CI_FINISH(bIn);
}

void MD_PZone::effectCiCascade(bool bIn) {
  CI_START_A(bIn);
  if (CI_ACTIVE) {
    int16_t start = _zoneStart * 8;
    int16_t W = (_zoneEnd + 1) * 8 - start;
    commonPrint();
    if (W <= 128) {
      uint8_t src[128], dst[128];
      for (int16_t i = 0; i < W; i++) { src[i] = ciCol(_MX, start + i, _inverted); dst[i] = 0; }
      int16_t s = _nextPos, denom = W + 14;
      for (int16_t i = 0; i < W; i++) {
        uint16_t a3 = (uint16_t)(i + 14);
        for (uint8_t k = 0; k < 8; k++, a3 -= 2) {
          if (!(src[i] & (1 << k))) continue;
          int16_t g = (int16_t)((3 * (int16_t)a3) / denom); if (g < 0) g = 0; if (g > 2) g = 2;
          int16_t val = (int16_t)((g - 2) * 8 + s);
          int16_t d = !bIn ? (val < 1 ? 0 : (val > 16 ? 16 : val)) : (val < 1 ? 16 : (val > 15 ? 0 : 16 - val));
          int16_t half = d >> 1;
          int16_t ny = bIn ? (k - half) : (k + half);
          int16_t nx = bIn ? (i - d) : (i + d);
          if (nx >= 0 && nx < W && ny >= 0 && ny < 8) dst[nx] |= (uint8_t)(1 << ny);
        }
      }
      for (int16_t i = 0; i < W; i++) _MX->setColumn(start + i, _inverted ? (uint8_t)~dst[i] : dst[i]);
    }
    if (_nextPos < 32) { _nextPos++; return; }
  } else if (_fsmState == END) return;
  CI_FINISH(bIn);
}

void MD_PZone::effectCiComb(bool bIn) {
  CI_START_A(bIn);
  int16_t start = _zoneStart * 8;
  int16_t W = (_zoneEnd + 1) * 8 - start;
  int16_t stag = W >> 1;
  if (CI_ACTIVE) {
    commonPrint();
    if (W <= 128) {
      uint8_t src[128], dst[128];
      for (int16_t i = 0; i < W; i++) { src[i] = ciCol(_MX, start + i, _inverted); dst[i] = 0; }
      for (int16_t i = 0; i < W; i++)
        for (uint8_t k = 0; k < 8; k++) {
          if (!(src[i] & (1 << k))) continue;
          int16_t eff = (int16_t)(_nextPos - (int16_t)((uint16_t)(k >> 1) * (uint16_t)stag));
          int16_t sh = bIn ? (eff < 1 ? W : (eff >= W ? 0 : W - eff)) : (eff < 1 ? 0 : (eff < W ? eff : W));
          if (!(k & 1)) sh = -sh;                           // парні/непарні рядки — в різні боки
          int16_t nx = i + sh;
          if (nx >= 0 && nx < W) dst[nx] |= (uint8_t)(1 << k);
        }
      for (int16_t i = 0; i < W; i++) _MX->setColumn(start + i, _inverted ? (uint8_t)~dst[i] : dst[i]);
    }
    if (_nextPos < (int16_t)(W + stag * 3)) { _nextPos++; return; }
  } else if (_fsmState == END) return;
  CI_FINISH(bIn);
}

void MD_PZone::effectCiIris(bool bIn) {
  CI_START_A(bIn);
  if (CI_ACTIVE) {
    int16_t start = _zoneStart * 8, endc = _zoneEnd * 8 + 7;
    commonPrint();
    int16_t maxR = (int16_t)((endc - start) / 2 + 3);
    int16_t r = bIn ? _nextPos : (int16_t)(maxR - _nextPos);
    int16_t center = (int16_t)((uint16_t)(endc + start) >> 1);
    int32_t r2 = (int32_t)r * r, rm2 = (int32_t)(r - 1) * (r - 1);
    bool ring = (r > 0) && (r < maxR);
    for (int16_t c = start; c <= endc; c++) {
      uint8_t col = ciCol(_MX, c, _inverted);
      int32_t dx2 = (int32_t)(c - center) * (c - center);
      uint8_t out = 0;
      for (uint8_t k = 0; k < 8; k++) {
        int32_t d2 = (int32_t)((int)k - 3) * ((int)k - 3) + dx2;
        if (d2 > r2) continue;
        if (col & (1 << k)) out |= (1 << k);
        if (ring && rm2 < d2) out |= (1 << k);             // кільце «діафрагми», що світиться
      }
      if (_inverted) out ^= 0xff;
      _MX->setColumn(c, out);
    }
    if (_nextPos < maxR) { _nextPos++; return; }
  } else if (_fsmState == END) return;
  CI_FINISH(bIn);
}
