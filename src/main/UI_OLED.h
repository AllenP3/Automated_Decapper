#ifndef UI_OLED_H
#define UI_OLED_H

#include <U8g2lib.h>

// ======================= MODES =======================
enum Mode {
    MODE_SNAP = 0,
    MODE_SCREW,
    MODE_HOME,
    MODE_JOG,
    MODE_INFO
};

// ======================= UI CLASS =======================
class UI_OLED {
public:
    UI_OLED();

    void begin();
    void update();

    Mode getMode() const;
    bool startRequested();
    bool stopRequested();
    bool homingNeeded() const;

    void clearHomingFlag() { needsHoming = false; }

    bool runPhase(const char* title, const char* subtitle, unsigned long duration);
    void showMessage(const char* title, const char* subtitle, int delayMs = 800);

    void requireHoming(bool v) { needsHoming = v; }
    bool homingRequired() const { return needsHoming; }

private:
    void drawIdleScreen();
    void drawHeaderBar();
    void drawModeStrip();
    void drawCentered(const char* text, int y, const uint8_t* font);
    void drawProgressScreen(const char* title, const char* sub, float progress);
    void showScreen(const char* title, const char* subtitle);

    void handleJoystickModeSwitch();
    int smoothRead(int pin);
    bool buttonPressed(int pin);

private:
    Mode currentMode;
    bool isRunning;
    bool needsHoming;

    long snapCount;
    long screwCount;

    unsigned long lastButtonTime;
    unsigned long lastMoveTime;
    unsigned long lastAnimTick;
    bool animBlink;

    U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2;

    static const unsigned long debounceMs = 220;
};

// ======================= JOYSTICK PINS =======================
#define JOY_X_PIN A0
#define JOY_Y_PIN A1
#define JOY_SW_PIN 22   // ensure SW → GND when pressed + INPUT_PULLUP

// ======================= BUTTONS =======================
#define BTN_STOP 3

#endif
