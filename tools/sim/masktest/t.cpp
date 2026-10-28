// перевірка mask_passwords (ws.cpp) на рядку з /networks.json
#include <cstdio>
#include <string>
struct String { std::string s; String() {} String(const char *c) : s(c) {} String(const std::string &x) : s(x) {}
  int indexOf(const char *t, int from = 0) const { auto p = s.find(t, from); return p == std::string::npos ? -1 : (int)p; }
  int indexOf(char c, int from = 0) const { auto p = s.find(c, from); return p == std::string::npos ? -1 : (int)p; }
  String substring(int a, int b = -1) const { return String(b < 0 ? s.substr(a) : s.substr(a, b - a)); }
  String &operator+=(const String &o) { s += o.s; return *this; } String &operator+=(const char *o) { s += o; return *this; }
  int length() const { return (int)s.size(); } char operator[](int i) const { return s[i]; } };
#include "fn.inc"
int main() {
  const char *t[] = { "Saved Networks Config File: [{\"network\":\"HomeWiFi\",\"password\":\"secret123\",\"auto\":true},{\"network\":\"Office5G\",\"password\":\"p\\\"q\",\"auto\":true}]",
                      "Config networks loaded: []", "Wifi : link OK" };
  for (auto x : t) printf("%s\n", mask_passwords(String(x)).s.c_str());
}
