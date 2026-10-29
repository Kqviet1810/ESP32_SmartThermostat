// Smart Thermostat - STM32F030C8T6 (Arduino core STM32duino)
// DS18B20 (PB4) + OLED SSD1306 I2C (PB6/PB7) + 3 nut (PA5/PA4/PA3) + relay (PB0, active LOW)
//
// Dieu khien:
//   Man 1: giu ENTER 2s            -> vao cai dat
//   Man 2..5: UP/DOWN              -> chinh gia tri
//             ENTER (nhan ngan)    -> man ke tiep; o man 5 thi luu va ve man 1
//             giu ENTER 2s         -> luu va ve man 1
//             giu UP+DOWN 1.5s     -> huy, bo thay doi
//             60s khong bam        -> tu huy, bo thay doi
#include "config.h"
#include "Settings.h"
#include "Buttons.h"
#include "Sensor.h"
#include "Thermostat.h"
#include "Ui.h"

static Button btnUp, btnDown, btnEnter;
static TempSensor sensor;
static Thermostat thermostat;

static Settings cfg;      // cai dat dang dung
static Settings edit;     // ban tam khi dang chinh
static Screen screen = S_MAIN;
static uint32_t lastActivity = 0;
static uint32_t lastDraw = 0;
static uint32_t bothSince = 0;
static bool dirty = true;

static void enterSettings(uint32_t now) {
  edit = cfg;
  screen = S_TEMP;
  lastActivity = now;
  dirty = true;
}

static void leaveSettings(bool save) {
  if (save) {
    settingsClamp(edit);
    if (!settingsEqual(edit, cfg)) {
      cfg = edit;
      settingsSave(cfg);
    }
  }
  screen = S_MAIN;
  bothSince = 0;
  dirty = true;
}

// delta = +1 (UP) hoac -1 (DOWN)
static void adjust(int delta) {
  switch (screen) {
    case S_TEMP: edit.setpoint = clampInt(edit.setpoint + delta * (int)edit.step, SET_MIN, SET_MAX); break;
    case S_MODE: edit.mode = (edit.mode == MODE_HEAT) ? MODE_COOL : MODE_HEAT; break;
    case S_HYST: edit.hyst = clampInt(edit.hyst + delta, HYST_MIN, HYST_MAX); break;
    case S_STEP: edit.step = clampInt(edit.step + delta, STEP_MIN, STEP_MAX); break;
    default: return;
  }
  dirty = true;
}

static void handleUi(uint32_t now) {
  bool anyEvent = btnUp.pressed || btnDown.pressed || btnEnter.pressed ||
                  btnUp.repeat || btnDown.repeat || btnEnter.clicked || btnEnter.longHit;

  if (screen == S_MAIN) {
    if (btnEnter.longHit) enterSettings(now);
    return;
  }

  if (anyEvent) lastActivity = now;
  if (now - lastActivity >= UI_TIMEOUT_MS) { leaveSettings(false); return; }

  // Giu UP+DOWN cung luc de huy
  if (btnUp.down() && btnDown.down()) {
    if (bothSince == 0) bothSince = now;
    if (now - bothSince >= BTN_CANCEL_MS) { leaveSettings(false); return; }
    return;  // dang giu 2 nut: khong chinh gia tri
  }
  bothSince = 0;

  if (btnEnter.longHit) { leaveSettings(true); return; }

  if (btnUp.pressed   || btnUp.repeat)   adjust(+1);
  if (btnDown.pressed || btnDown.repeat) adjust(-1);

  if (btnEnter.clicked) {
    if (screen == S_STEP) { leaveSettings(true); return; }
    screen = (Screen)(screen + 1);
    dirty = true;
  }
}

void setup() {
  // Relay tat ngay lap tuc (muc cao = tat)
  pinMode(PIN_RELAY, OUTPUT);
  digitalWrite(PIN_RELAY, RELAY_OFF_LEVEL);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  btnUp.begin(PIN_BTN_UP);
  btnDown.begin(PIN_BTN_DOWN);
  btnEnter.begin(PIN_BTN_ENTER);

  settingsLoad(cfg);
  uiBegin();
  sensor.begin();
  thermostat.begin();
}

void loop() {
  uint32_t now = millis();

  btnUp.update(now);
  btnDown.update(now);
  btnEnter.update(now);

  sensor.update(now);
  thermostat.update(cfg, sensor.valid, sensor.tenths, now);

  handleUi(now);

  if (dirty || now - lastDraw >= UI_REFRESH_MS) {
    lastDraw = now;
    dirty = false;
    if (screen == S_MAIN) uiDrawMain(sensor.valid, sensor.tenths, cfg, thermostat.relayOn);
    else                  uiDrawSetting(screen, edit);
  }
}
