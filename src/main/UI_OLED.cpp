#include "UI_OLED.h"
#include <Arduino.h>

static const char* MODE_NAMES[] = {"SNAP", "SCREW", "HOME", "JOG", "INFO"};

PanelIO::PanelIO()
: u8g2(U8G2_R0, U8X8_PIN_NONE)
{
    currentMode = MODE_SNAP;
    isRunning = false;
    needsHoming = false;
    snapCount = 0;
    screwCount = 0;
    lastButtonTime = 0;
    lastAnimTick = 0;
    animBlink = false;
}

void PanelIO::begin() {
    pinMode(JOY_SW_PIN, INPUT_PULLUP);
    pinMode(BTN_START, INPUT_PULLUP);
    pinMode(BTN_STOP, INPUT_PULLUP);

    u8g2.begin();
    showScreen("Decapper UI", "Initializing...");
    delay(800);
}

void PanelIO::update() {
    unsigned long now = millis();

    // animation tick
    if (now - lastAnimTick > 400) {
        lastAnimTick = now;
        animBlink = !animBlink;
    }

    if (!isRunning) {
        handleJoystickModeSwitch();
        drawIdleScreen();
    }
}

// ------------------------------------------------------------
// ACCESSORS
// ------------------------------------------------------------
Mode PanelIO::getMode() const {
    return currentMode;
}

bool PanelIO::startRequested() {
    return buttonPressed(BTN_START);
}

bool PanelIO::stopRequested() {
    return (digitalRead(BTN_STOP) == LOW);
}

bool PanelIO::homingNeeded() const {
    return needsHoming;
}

// ------------------------------------------------------------
// BUTTON + JOYSTICK UTILITIES
// ------------------------------------------------------------
bool PanelIO::buttonPressed(int pin) {
    if (digitalRead(pin) == LOW) {
        unsigned long now = millis();
        if (now - lastButtonTime > debounceMs) {
            lastButtonTime = now;
            return true;
        }
    }
    return false;
}

int PanelIO::smoothRead(int pin) {
    long sum = 0;
    for (int i = 0; i < 8; i++) {
        sum += analogRead(pin);
        delayMicroseconds(150);
    }
    return sum / 8;
}

// ------------------------------------------------------------
// MODE SWITCHING VIA JOYSTICK
// ------------------------------------------------------------
void PanelIO::handleJoystickModeSwitch() {
    int xVal = smoothRead(JOY_X_PIN);
    static unsigned long lastMove = 0;

    if (millis() - lastMove < 220) return;

    if (xVal < 350) {
        if (currentMode == MODE_SNAP) currentMode = MODE_INFO;
        else currentMode = (Mode)((int)currentMode - 1);
        lastMove = millis();
    }
    else if (xVal > 700) {
        if (currentMode == MODE_INFO) currentMode = MODE_SNAP;
        else currentMode = (Mode)((int)currentMode + 1);
        lastMove = millis();
    }
}

// ------------------------------------------------------------
// DRAWING HELPERS
// ------------------------------------------------------------
void PanelIO::drawHeaderBar() {
    u8g2.setFont(u8g2_font_t0_11b_tf);

    u8g2.drawStr(2, 10, "DECAP CTRL");

    int x = 96, y = 2, w = 30, h = 10;
    u8g2.drawFrame(x, y, w, h);
    u8g2.drawBox(x + w, y + 3, 3, 4);

    const char* status =
        isRunning ? "RUN" :
        needsHoming ? "HOME" : "IDLE";

    u8g2.setFont(u8g2_font_5x8_tr);
    int sw = u8g2.getStrWidth(status);
    u8g2.drawStr(x + (w - sw) / 2, y + 8, status);
}

void PanelIO::drawModeStrip() {
    u8g2.setFont(u8g2_font_5x8_tr);
    int y = 62;
    int x = 0;

    for (int i = 0; i < 5; i++) {
        const char* name = MODE_NAMES[i];
        int w = u8g2.getStrWidth(name) + 6;

        if (i == currentMode) {
            u8g2.drawBox(x, y - 9, w, 10);
            u8g2.setDrawColor(0);
            u8g2.drawStr(x + 3, y - 1, name);
            u8g2.setDrawColor(1);

            if (animBlink) {
                int tx = x + w / 2;
                u8g2.drawTriangle(tx - 3, y - 11, tx + 3, y - 11, tx, y - 7);
            }
        }
        else {
            u8g2.drawStr(x + 3, y - 1, name);
        }

        x += w + 1;
    }
}

void PanelIO::drawCentered(const char* text, int y, const uint8_t* font) {
    u8g2.setFont(font);
    int w = u8g2.getStrWidth(text);
    u8g2.drawStr((128 - w) / 2, y, text);
}

void PanelIO::drawIdleScreen() {
    u8g2.clearBuffer();
    drawHeaderBar();

    char line1[24];
    char line2[24];

    if (currentMode == MODE_SNAP)
        snprintf(line1, 24, "SNAP C:%ld", snapCount);
    else if (currentMode == MODE_SCREW)
        snprintf(line1, 24, "SCREW C:%ld", screwCount);
    else if (currentMode == MODE_INFO) {
        long total = snapCount + screwCount;
        snprintf(line1, 24, "TOTAL:%ld", total);
    }
    else snprintf(line1, 24, "%s MODE", MODE_NAMES[currentMode]);

    if (needsHoming)
        snprintf(line2, 24, "Homing required");
    else
        snprintf(line2, 24, "START=D2 STOP=D3");

    drawCentered(line1, 32, u8g2_font_ncenB08_tr);
    drawCentered(line2, 46, u8g2_font_6x10_tf);

    drawModeStrip();
    u8g2.sendBuffer();
}

void PanelIO::drawProgressScreen(const char* title,
                                 const char* phase,
                                 float p)
{
    if (p < 0) p = 0;
    if (p > 1) p = 1;

    u8g2.clearBuffer();
    drawHeaderBar();
    drawCentered(title, 28, u8g2_font_ncenB08_tr);

    int bx = 16, by = 38, bw = 96, bh = 10;
    u8g2.drawFrame(bx, by, bw, bh);

    int fill = p * bw;
    u8g2.drawBox(bx + 1, by + 1, fill - 2, bh - 2);

    char buf[24];
    snprintf(buf, 24, "%s %d%%", phase, (int)(p * 100));
    drawCentered(buf, 56, u8g2_font_6x10_tf);

    u8g2.sendBuffer();
}

// ------------------------------------------------------------
// MESSAGES FOR ROUTINES
// ------------------------------------------------------------
void PanelIO::showScreen(const char* title, const char* subtitle) {
    u8g2.clearBuffer();
    drawHeaderBar();
    drawCentered(title, 32, u8g2_font_ncenB08_tr);
    drawCentered(subtitle, 48, u8g2_font_6x10_tf);
    u8g2.sendBuffer();
}

void PanelIO::showMessage(const char* t, const char* s, int d) {
    showScreen(t, s);
    delay(d);
}

// ------------------------------------------------------------
// PROGRESS PHASE (USED BY SNAP, SCREW, HOME...)
// ------------------------------------------------------------
bool PanelIO::runPhase(const char* title,
                       const char* sub,
                       unsigned long duration)
{
    isRunning = true;
    unsigned long start = millis();

    while (millis() - start < duration) {
        if (stopRequested()) {
            needsHoming = true;
            isRunning = false;
            showScreen("STOPPED", "Cycle interrupted");
            delay(600);
            return false;
        }
        float p = float(millis() - start) / duration;
        drawProgressScreen(title, sub, p);
        delay(60);
    }

    drawProgressScreen(title, sub, 1.0f);
    delay(200);

    isRunning = false;
    return true;
}
