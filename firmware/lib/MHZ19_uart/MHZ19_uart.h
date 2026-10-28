// MHZ19_uart (nara256, варіант для ESP32 з номером апаратного UART).
// У реєстрі PlatformIO цієї бібліотеки немає — відтворено з машинного коду прошивки 2.7.1
// (конструктор @4013e4bc, begin @401da070, writeCommand @4013e518, setAutoCalibration @4013e5b0, getSerialData @4013e5cc).
#pragma once
#include <Arduino.h>

enum MHZ19_UART_DATA { PPM, TEMPERATURE, STAT };

class MHZ19_uart {
 public:
  MHZ19_uart() {}
  virtual ~MHZ19_uart() {}
  void begin(int rx, int tx, int s) {
    _rx_pin = rx;
    _tx_pin = tx;
    _serialno = s;
  }
  void setAutoCalibration(boolean autocalib);
  void calibrateZero();
  void calibrateSpan(int ppm);
  int getPPM();
  int getTemperature();
  int getStatus();
  boolean isWarming();

 protected:
  void writeCommand(uint8_t com[]);
  void writeCommand(uint8_t com[], uint8_t response[]);

 private:
  uint8_t mhz19_checksum(uint8_t com[]) {
    uint8_t sum = 0x00;
    for (int i = 1; i < REQUEST_CNT; i++) sum += com[i];
    sum = 0xff - sum + 0x01;
    return sum;
  }
  int getSerialData(MHZ19_UART_DATA flg);

  static const int REQUEST_CNT = 8;
  static const int RESPONSE_CNT = 9;
  uint8_t getppm[REQUEST_CNT] = { 0xff, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00 };
  uint8_t zerocalib[REQUEST_CNT] = { 0xff, 0x01, 0x87, 0x00, 0x00, 0x00, 0x00, 0x00 };
  uint8_t spancalib[REQUEST_CNT] = { 0xff, 0x01, 0x88, 0x00, 0x00, 0x00, 0x00, 0x00 };
  uint8_t autocalib_on[REQUEST_CNT] = { 0xff, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00 };
  uint8_t autocalib_off[REQUEST_CNT] = { 0xff, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00 };
  int _serialno = 1;
  int _rx_pin = -1;
  int _tx_pin = -1;
};
