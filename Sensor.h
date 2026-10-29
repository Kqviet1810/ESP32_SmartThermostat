#pragma once
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"

// DS18B20 doc khong chan: gui lenh convert, SENSOR_PERIOD_MS sau moi doc ket qua.
class TempSensor {
 public:
  TempSensor() : ow(PIN_ONEWIRE), ds(&ow) {}

  bool valid;       // false = ERR
  int16_t tenths;   // nhiet do theo 0.1 C (chi dung khi valid)

  void begin() {
    valid = false;
    tenths = 0;
    fails = SENSOR_FAIL_LIMIT;
    ds.begin();
    ds.setWaitForConversion(false);
    ds.setResolution(12);
    ds.requestTemperatures();
    reqAt = millis();
  }

  void update(uint32_t now) {
    if (now - reqAt < SENSOR_PERIOD_MS) return;

    if (ds.getDeviceCount() == 0) ds.begin();  // cam lai cam bien sau khi mat ket noi

    float t = ds.getTempCByIndex(0);
    // -127 = mat ket noi, 85.0 = gia tri reset khi chua convert xong
    if (t == DEVICE_DISCONNECTED_C || t == 85.0f) {
      if (fails < SENSOR_FAIL_LIMIT) fails++;
      if (fails >= SENSOR_FAIL_LIMIT) valid = false;
    } else {
      fails = 0;
      valid = true;
      tenths = (int16_t)lroundf(t * 10.0f);
    }

    ds.requestTemperatures();
    reqAt = now;
  }

 private:
  OneWire ow;
  DallasTemperature ds;
  uint32_t reqAt;
  uint8_t fails;
};
