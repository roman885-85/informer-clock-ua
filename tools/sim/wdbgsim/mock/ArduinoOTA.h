#pragma once
#include <functional>
typedef int ota_error_t;
struct ArduinoOTAClass { ArduinoOTAClass &setMdnsEnabled(bool) { return *this; } ArduinoOTAClass &setHostname(const char *) { return *this; }
  ArduinoOTAClass &setPassword(const char *) { return *this; } ArduinoOTAClass &onStart(std::function<void()>) { return *this; }
  ArduinoOTAClass &onProgress(std::function<void(unsigned, unsigned)>) { return *this; } ArduinoOTAClass &onEnd(std::function<void()>) { return *this; }
  ArduinoOTAClass &onError(std::function<void(ota_error_t)>) { return *this; } void begin() {} void end() {} void handle() {} };
inline ArduinoOTAClass ArduinoOTA;
