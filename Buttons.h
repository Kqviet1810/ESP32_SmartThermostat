#pragma once
#include "config.h"

// Nut noi GND, dung INPUT_PULLUP (nhan = LOW). Co C 100nF chong rung phan cung,
// them debounce phan mem 30ms.
class Button {
 public:
  // Co su kien: dat lai moi lan update()
  bool pressed;   // vua nhan xuong
  bool repeat;    // lap khi giu (UP/DOWN)
  bool clicked;   // nha ra truoc khi dat nguong giu lau
  bool longHit;   // vua giu lau du BTN_LONG_MS (chi 1 lan moi lan giu)

  void begin(uint8_t p) {
    pin = p;
    pinMode(pin, INPUT_PULLUP);
    raw = stable = (digitalRead(pin) == LOW);
    lastChange = pressedAt = lastRepeat = 0;
    longFired = false;
    pressed = repeat = clicked = longHit = false;
  }

  bool down() const { return stable; }

  void update(uint32_t now) {
    pressed = repeat = clicked = longHit = false;

    bool r = (digitalRead(pin) == LOW);
    if (r != raw) { raw = r; lastChange = now; }
    if (raw != stable && now - lastChange >= BTN_DEBOUNCE_MS) {
      stable = raw;
      if (stable) {
        pressed = true;
        pressedAt = lastRepeat = now;
        longFired = false;
      } else if (!longFired) {
        clicked = true;
      }
    }

    if (stable) {
      uint32_t held = now - pressedAt;
      if (!longFired && held >= BTN_LONG_MS) { longFired = true; longHit = true; }
      uint32_t first = BTN_REPEAT_FIRST_MS;
      uint32_t every = (held >= BTN_LONG_MS) ? BTN_REPEAT_FAST_MS : BTN_REPEAT_MS;
      if (held >= first && now - lastRepeat >= every) { lastRepeat = now; repeat = true; }
    }
  }

 private:
  uint8_t pin;
  bool raw, stable, longFired;
  uint32_t lastChange, pressedAt, lastRepeat;
};
