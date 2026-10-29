#pragma once
#include <U8g2lib.h>
#include <Wire.h>
#include "config.h"
#include "Settings.h"

enum Screen : uint8_t { S_MAIN = 0, S_TEMP, S_MODE, S_HYST, S_STEP };

static U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

static void uiBegin() {
  Wire.setSDA(PIN_SDA);
  Wire.setSCL(PIN_SCL);
  u8g2.begin();
  u8g2.setBusClock(400000);
}

// 305 -> "30.5", -53 -> "-5.3"
static void fmtTenths(char *b, size_t n, int v) {
  const char *sign = v < 0 ? "-" : "";
  int a = v < 0 ? -v : v;
  snprintf(b, n, "%s%d.%d", sign, a / 10, a % 10);
}

// Ve so lon + ky hieu do C, can giua man hinh.
static void drawValueUnit(const char *val, const uint8_t *font, int baseline) {
  u8g2.setFont(font);
  int w = u8g2.getStrWidth(val);
  int ascent = u8g2.getAscent();
  const int unitW = 24;  // vong tron + chu C
  int x = (128 - (w + unitW)) / 2;
  if (x < 0) x = 0;
  u8g2.drawStr(x, baseline, val);
  u8g2.drawCircle(x + w + 6, baseline - ascent + 5, 3);
  u8g2.setFont(u8g2_font_helvB14_tr);
  u8g2.drawStr(x + w + 10, baseline, "C");
}

static void drawCentered(const char *s, const uint8_t *font, int baseline) {
  u8g2.setFont(font);
  u8g2.drawStr((128 - u8g2.getStrWidth(s)) / 2, baseline, s);
}

// ---------- Man 1 ----------
static void uiDrawMain(bool valid, int16_t temp, const Settings &s, bool relayOn) {
  char tbuf[10], sbuf[16];
  fmtTenths(tbuf, sizeof(tbuf), temp);
  fmtTenths(sbuf, sizeof(sbuf), s.setpoint);
  char line[24];
  snprintf(line, sizeof(line), "SET %s", sbuf);
  const char *modeStr = (s.mode == MODE_HEAT) ? "HEAT" : "COOL";

  u8g2.firstPage();
  do {
    if (valid) {
      drawValueUnit(tbuf, u8g2_font_logisoso38_tn, 42);
    } else {
      drawCentered("ERR", u8g2_font_helvB24_tr, 36);
      drawCentered("Kiem tra cam bien", u8g2_font_6x10_tf, 48);
    }
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 62, line);
    u8g2.drawStr(64, 62, modeStr);
    if (relayOn) {
      u8g2.drawBox(100, 52, 28, 12);
      u8g2.setDrawColor(0);
      u8g2.drawStr(106, 62, "ON");
      u8g2.setDrawColor(1);
    } else {
      u8g2.drawStr(106, 62, "OFF");
    }
  } while (u8g2.nextPage());
}

// ---------- Man 2..5 ----------
static void uiDrawSetting(Screen scr, const Settings &t) {
  const char *title = "";
  char val[12] = "";
  const char *hint = "UP/DN chinh ENT tiep";

  switch (scr) {
    case S_TEMP: title = "NHIET CAI DAT"; fmtTenths(val, sizeof(val), t.setpoint); break;
    case S_MODE: title = "CHE DO";        strcpy(val, t.mode == MODE_HEAT ? "HEAT" : "COOL"); break;
    case S_HYST: title = "DO TRE";        fmtTenths(val, sizeof(val), t.hyst); break;
    case S_STEP: title = "BUOC CHINH";    fmtTenths(val, sizeof(val), t.step);
                 hint = "UP/DN chinh ENT luu"; break;
    default: return;
  }
  char idx[8];
  snprintf(idx, sizeof(idx), "%d/4", (int)scr);

  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 9, title);
    u8g2.drawStr(128 - u8g2.getStrWidth(idx), 9, idx);
    u8g2.drawHLine(0, 12, 128);

    if (scr == S_MODE) drawCentered(val, u8g2_font_helvB24_tr, 46);
    else               drawValueUnit(val, u8g2_font_logisoso32_tn, 48);

    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(0, 62, hint);
  } while (u8g2.nextPage());
}
