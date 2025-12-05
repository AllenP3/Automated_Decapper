#pragma once
#include <Arduino.h>
#include <U8g2lib.h>
#include "Pins.h"

enum Mode {
    MODE_SNAP = 0,
    MODE_SCREW = 1,
    MODE_HOME = 2,
    MODE_JOG = 3,
    MODE_INFO = 4
};

class UI_OLED {
public:
    UI_OLED();

    void begin();
    void update();

    Mode getMode() const;
    bool startRequested();
    bool stopRequested();

    void showScreen(const char* t, const char* s);

private:
    U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2;

    Mode currentMode;
    unsigned long lastMove;
    unsigned long lastButton;

    void draw();
    void handleJoystick();
    int smoothRead(int pin);
};
