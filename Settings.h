#pragma once
#include "config.h"

enum Mode : uint8_t { MODE_HEAT = 0, MODE_COOL = 1 };

struct Settings {
  int16_t setpoint;  // 0.1 C
  uint8_t mode;      // Mode
  uint8_t hyst;      // 0.1 C
  uint8_t step;      // 0.1 C
};

static const int SETTINGS_WORDS = 6;  // magic, setpoint, mode, hyst, step, checksum

static inline int clampInt(int v, int lo, int hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

static inline void settingsClamp(Settings &s) {
  s.setpoint = clampInt(s.setpoint, SET_MIN, SET_MAX);
  s.mode     = (s.mode == MODE_COOL) ? MODE_COOL : MODE_HEAT;
  s.hyst     = clampInt(s.hyst, HYST_MIN, HYST_MAX);
  s.step     = clampInt(s.step, STEP_MIN, STEP_MAX);
}

static inline Settings settingsDefault() {
  Settings s = { DEFAULT_SETPOINT, DEFAULT_MODE, DEFAULT_HYST, DEFAULT_STEP };
  return s;
}

static inline bool settingsEqual(const Settings &a, const Settings &b) {
  return a.setpoint == b.setpoint && a.mode == b.mode &&
         a.hyst == b.hyst && a.step == b.step;
}

static uint16_t settingsChecksum(const uint16_t *w) {
  uint16_t c = 0xA5A5;
  for (int i = 0; i < SETTINGS_WORDS - 1; i++) c ^= (uint16_t)(w[i] + i);
  return c;
}

// Doc tu flash. Hop le thi tra true, nguoc lai tra mac dinh.
static bool settingsLoad(Settings &out) {
  const volatile uint16_t *p = (const volatile uint16_t *)SETTINGS_FLASH_ADDR;
  uint16_t w[SETTINGS_WORDS];
  for (int i = 0; i < SETTINGS_WORDS; i++) w[i] = p[i];

  if (w[0] == SETTINGS_MAGIC && w[5] == settingsChecksum(w)) {
    out.setpoint = (int16_t)w[1];
    out.mode = (uint8_t)w[2];
    out.hyst = (uint8_t)w[3];
    out.step = (uint8_t)w[4];
    settingsClamp(out);
    return true;
  }
  out = settingsDefault();
  return false;
}

// Xoa trang cuoi roi ghi lai (chi goi khi nguoi dung luu va gia tri thay doi).
static bool settingsSave(const Settings &s) {
  uint16_t w[SETTINGS_WORDS];
  w[0] = SETTINGS_MAGIC;
  w[1] = (uint16_t)s.setpoint;
  w[2] = s.mode;
  w[3] = s.hyst;
  w[4] = s.step;
  w[5] = settingsChecksum(w);

  bool ok = false;
  HAL_FLASH_Unlock();
  FLASH_EraseInitTypeDef e;
  memset(&e, 0, sizeof(e));
  e.TypeErase = FLASH_TYPEERASE_PAGES;
  e.PageAddress = SETTINGS_FLASH_ADDR;
  e.NbPages = 1;
  uint32_t pageErr = 0;
  if (HAL_FLASHEx_Erase(&e, &pageErr) == HAL_OK) {
    ok = true;
    for (int i = 0; i < SETTINGS_WORDS && ok; i++) {
      ok = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,
                             SETTINGS_FLASH_ADDR + 2 * i, w[i]) == HAL_OK;
    }
  }
  HAL_FLASH_Lock();
  return ok;
}
