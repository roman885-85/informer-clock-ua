// itoa/utoa для String на комп'ютері (tools/sim/newstail): у прошивці їх дає newlib, у бібліотеці macOS/Linux їх немає
#include <stdlib.h>
#include <string.h>

char *utoa(unsigned int v, char *s, int radix) {
  char t[33];
  int n = 0;
  do { int d = v % radix; t[n++] = d < 10 ? '0' + d : 'a' + d - 10; v /= radix; } while (v);
  for (int i = 0; i < n; i++) s[i] = t[n - 1 - i];
  s[n] = 0;
  return s;
}
char *itoa(int v, char *s, int radix) {
  if (v < 0 && radix == 10) { s[0] = '-'; utoa(-(unsigned int)v, s + 1, radix); return s; }
  return utoa((unsigned int)v, s, radix);
}
