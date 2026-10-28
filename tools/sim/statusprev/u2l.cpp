// Перекодування UTF-8 → коди шрифту табло справжньою utf8_to_led() із firmware/src/text_led.cpp
#include <cstdio>
#include <cstdint>
#include <string>
struct String {
  std::string s;
  String() {}
  String(const char *c) : s(c) {}
  int length() const { return (int)s.size(); }
  char operator[](int i) const { return i < (int)s.size() ? s[i] : 0; }
  String &operator+=(char c) { s += c; return *this; }
  String &operator+=(const String &o) { s += o.s; return *this; }
  String &operator+=(const char *o) { s += o; return *this; }
};
void utf8_to_led(const String &src, char *dst, int cap);
int main(int argc, char **argv) {
  for (int a = 1; a < argc; a++) {
    char out[256] = {0};
    utf8_to_led(String(argv[a]), out, 255);
    for (int i = 0; out[i]; i++) printf("%u ", (unsigned char)out[i]);
    printf("\n");
  }
}
