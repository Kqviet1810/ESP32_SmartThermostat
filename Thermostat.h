#pragma once
#include "config.h"
#include "Settings.h"

// HEAT: bat khi T <= Set - H, tat khi T >= Set
// COOL: bat khi T >= Set + H, tat khi T <= Set
// Vung giua: giu nguyen. Loi cam bien: tat relay ngay.
class Thermostat {
 public:
  bool relayOn;

  void begin() {
    relayOn = false;
    lastSwitch = 0;
    switched = false;
    apply();
  }

  void update(const Settings &s, bool valid, int16_t t, uint32_t now) {
    if (!valid) {               // an toan: tat ngay, bo qua thoi gian toi thieu
      if (relayOn) setRelay(false, now);
      return;
    }

    bool want = relayOn;
    if (s.mode == MODE_HEAT) {
      if (t <= s.setpoint - (int)s.hyst) want = true;
      else if (t >= s.setpoint)          want = false;
    } else {
      if (t >= s.setpoint + (int)s.hyst) want = true;
      else if (t <= s.setpoint)          want = false;
    }

    if (want != relayOn && (!switched || now - lastSwitch >= RELAY_MIN_SWITCH_MS)) {
      setRelay(want, now);
    }
  }

 private:
  uint32_t lastSwitch;
  bool switched;

  void setRelay(bool on, uint32_t now) {
    relayOn = on;
    lastSwitch = now;
    switched = true;
    apply();
  }

  void apply() {
    digitalWrite(PIN_RELAY, relayOn ? RELAY_ON_LEVEL : RELAY_OFF_LEVEL);
    digitalWrite(PIN_LED, relayOn ? HIGH : LOW);
  }
};
