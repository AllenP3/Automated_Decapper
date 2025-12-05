#include "UI_OLED.h"

static const char* MODE_NAMES[] = {
    "SNAP", "SCREW", "HOME", "JOG", "INFO"
};

UI_OLED::UI_OLED()
: u8g2(U8G2_R0, U8X8_PIN_NONE)
{
    currentMode = MODE_SNAP;
    lastMove = 0;
    lastButton = 0;
}

void UI_OLED::begin() {
    pinMode(PIN_JOY_SW, INPUT);
    pinMode(PIN_BTN_STOP, INPUT_PULLUP);

    u8g2.begin();
    showScreen("Decapper UI", "Ready");
    delay(600);
}

void UI_OLED::update() {
    handleJoystick();
    draw();
}

Mode UI_OLED::getMode() const {
    return currentMode;
}

bool UI_OLED::startRequested() {
    int v = digitalRead(PIN_JOY_SW);
    if (v == HIGH) {
        unsigned long now = millis();
        if (now - lastButton > 300) {
            lastButton = now;
            return true;
        }
    }
    return false;
}

bool UI_OLED::stopRequested() {
    return digitalRead(PIN_BTN_STOP) == LOW;
}

void UI_OLED::handleJoystick() {
    int x = smoothRead(PIN_JOY_X);
    unsigned long now = millis();

    if (now - lastMove < 220) return;

    if (x < 350) {
        if (currentMode == MODE_SNAP) currentMode = MODE_INFO;
        else currentMode = (Mode)(currentMode - 1);
        lastMove = now;
    } 
    else if (x > 700) {
        if (currentMode == MODE_INFO) currentMode = MODE_SNAP;
        else currentMode = (Mode)(currentMode + 1);
        lastMove = now;
    }
}

int UI_OLED::smoothRead(int pin) {
    long sum = 0;
    for (int i = 0; i < 6; i++) {
        sum += analogRead(pin);
        delayMicroseconds(150);
    }
    return sum / 6;
}

void UI_OLED::draw() {
    u8g2.clearBuffer();

    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.drawStr(2, 10, "DECAP MENU");

    int x = 2;
    int y = 40;

    for (int i = 0; i < 5; i++) {
        const char* label = MODE_NAMES[i];
        int w = u8g2.getStrWidth(label) + 10;

        if (i == currentMode) {
            u8g2.drawBox(x - 2, y - 12, w, 14);
            u8g2.setDrawColor(0);
            u8g2.drawStr(x, y, label);
            u8g2.setDrawColor(1);
        } else {
            u8g2.drawStr(x, y, label);
        }

        x += w + 4;
    }

    u8g2.sendBuffer();
}

void UI_OLED::showScreen(const char* t, const char* s) {
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);

    int w = u8g2.getStrWidth(t);
    u8g2.drawStr((128 - w)/2, 28, t);

    u8g2.setFont(u8g2_font_6x10_tf);
    w = u8g2.getStrWidth(s);
    u8g2.drawStr((128 - w)/2, 48, s);

    u8g2.sendBuffer();
}
