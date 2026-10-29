#pragma once
#include <Arduino.h>

// ---------- Chan (theo so do STM32F030C8T6) ----------
#define PIN_SCL        PB6
#define PIN_SDA        PB7
#define PIN_BTN_UP     PA5
#define PIN_BTN_ENTER  PA4
#define PIN_BTN_DOWN   PA3
#define PIN_ONEWIRE    PB4   // DS18B20, R13 4.7k keo len 3.3V
#define PIN_RELAY      PB0   // tich cuc MUC THAP: LOW = relay dong
#define PIN_LED        PA1   // LED1, tich cuc muc cao

#define RELAY_ON_LEVEL   LOW
#define RELAY_OFF_LEVEL  HIGH

// ---------- Don vi: moi gia tri nhiet la so nguyen theo 0.1 do C ----------
#define SET_MIN          0      // 0.0 C
#define SET_MAX          999    // 99.9 C
#define HYST_MIN         1      // 0.1 C
#define HYST_MAX         100    // 10.0 C
#define STEP_MIN         1      // 0.1 C
#define STEP_MAX         10     // 1.0 C

#define DEFAULT_SETPOINT 300    // 30.0 C
#define DEFAULT_MODE     MODE_HEAT
#define DEFAULT_HYST     10     // 1.0 C
#define DEFAULT_STEP     5      // 0.5 C

// ---------- Thoi gian (ms) ----------
#define BTN_DEBOUNCE_MS      30
#define BTN_LONG_MS          2000   // giu ENTER de vao/luu
#define BTN_REPEAT_FIRST_MS  500
#define BTN_REPEAT_MS        150
#define BTN_REPEAT_FAST_MS   60     // sau khi giu > 2 giay
#define BTN_CANCEL_MS        1500   // giu UP+DOWN cung luc de huy
#define UI_TIMEOUT_MS        60000  // 1 phut khong bam: bo thay doi
#define UI_REFRESH_MS        150

#define SENSOR_PERIOD_MS     1000   // chu ky doc DS18B20 (12 bit can ~750ms)
#define SENSOR_FAIL_LIMIT    3      // so lan loi lien tiep truoc khi bao ERR
#define RELAY_MIN_SWITCH_MS  5000   // toi thieu giua 2 lan dong/ngat relay

// ---------- Luu cai dat vao trang flash cuoi (F030C8: 64KB, page 1KB) ----------
#define SETTINGS_FLASH_ADDR  0x0800FC00UL
#define SETTINGS_MAGIC       0x7A51
