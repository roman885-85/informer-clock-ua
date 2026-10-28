#include "Base64.h"

static inline void a4_to_a3(unsigned char *a3, unsigned char *a4) {
  a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
  a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
  a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
}
static inline unsigned char b64_lookup(char c) {
  if (c >= 'A' && c <= 'Z') return c - 'A';
  if (c >= 'a' && c <= 'z') return c - 71;
  if (c >= '0' && c <= '9') return c + 4;
  if (c == '+') return 62;
  if (c == '/') return 63;
  return -1;
}
int base64_decode(char *output, char *input, int inputLen) {
  int i = 0, j = 0;
  int decLen = 0;
  unsigned char a3[3];
  unsigned char a4[4];
  while (inputLen--) {
    if (*input == '=') break;
    a4[i++] = *(input++);
    if (i == 4) {
      for (i = 0; i < 4; i++) a4[i] = b64_lookup(a4[i]);
      a4_to_a3(a3, a4);
      for (i = 0; i < 3; i++) output[decLen++] = a3[i];
      i = 0;
    }
  }
  if (i) {
    for (j = i; j < 4; j++) a4[j] = '\0';
    for (j = 0; j < 4; j++) a4[j] = b64_lookup(a4[j]);
    a4_to_a3(a3, a4);
    for (j = 0; j < i - 1; j++) output[decLen++] = a3[j];
  }
  output[decLen] = '\0';
  return decLen;
}
