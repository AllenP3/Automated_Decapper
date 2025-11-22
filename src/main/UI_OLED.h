#ifndef UI_OLED_H
#define UI_OLED_H

#include <U8g2lib.h>

// -----------------------------
// ENUMS
// -----------------------------
enum Mode {
    MODE_SNAP = 0,
    MODE_SCREW,
    MODE_HOME,
    MODE_JOG,
    MODE_INFO
};

// -----------------------------
// Main UI Class
// -----------------------------
class UI_OLED {
public:
    UI_OLED();

    // setup & loop
    void begin();
    void update();

    // accessors
    Mode getMode() const;
    bool startRequested();
    bool stopRequested();
    bool homingNeeded() const;

    // change homing state externally (after motors finish)
    void clearHomingFlag() { needsHoming = false; }

    // used by routines to display progress
    bool runPhase(const char* title, const char* subtitle, unsigned long duration);

    // used by routines after completing
    void showMessage(const char* title, const char* subtitle, int delayMs = 800);

    void requireHoming(bool v) { needsHoming = v; }
    bool homingRequired() const { return needsHoming; }

private:
    // drawing helpers
    void drawIdleScreen();
    void drawHeaderBar();
    void drawModeStrip();
    void drawCentered(const char* text, int y, const uint8_t* font);
    void drawProgressScreen(const char* title, const char* sub, float progress);
    void showScreen(const char* title, const char* subtitle);

    // joystick
    void handleJoystickModeSwitch();
    int smoothRead(int pin);
    bool buttonPressed(int pin);

private:
    // state
    Mode currentMode;
    bool isRunning;
    bool needsHoming;

    long snapCount;
    long screwCount;

    unsigned long lastButtonTime;
    unsigned long lastAnimTick;
    bool animBlink;

    // U8G2 instance
    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

    // constants
    static const unsigned long debounceMs = 200;
};

// Button & Joystick Pins
#define JOY_X_PIN A0
#define JOY_Y_PIN A1
#define JOY_SW_PIN 22

#define BTN_START 2
#define BTN_STOP  3

#endif
