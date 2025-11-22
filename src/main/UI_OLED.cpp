#include "UI_OLED.h"
#include <Arduino.h>

static const char* MODE_NAMES[] = {"SNAP", "SCREW", "HOME", "JOG", "INFO"};

UI_OLED::UI_OLED()
: u8g2(U8G2_R0)
{
    currentMode = MODE_SNAP;
    isRunning = false;
    needsHoming = false;

    snapCount = 0;
    screwCount = 0;

    lastPressTime = 0;
    lastMoveTime = 0;
    lastAnimTick = 0;
    animBlink = false;
}

// =========================================================
// INIT
// =========================================================
void UI_OLED::begin() {
    pinMode(PIN_BTN_START, INPUT);
    digitalWrite(PIN_BTN_START, LOW); // disables pull-up
        // joystick button (ACTIVE HIGH)
    pinMode(BTN_STOP, INPUT_PULLUP);  // stop button (ACTIVE LOW)

    u8g2.begin();
    showScreen("DECAP UI", "Starting...");
    delay(600);
}

// =========================================================
// UPDATE LOOP
// =========================================================
void UI_OLED::update() {
    unsigned long now = millis();

    if (now - lastAnimTick > 400) {
        animBlink = !animBlink;
        lastAnimTick = now;
    }

    if (!isRunning) {
        handleJoystickModeSwitch();
        drawIdleScreen();
    }
}

// =========================================================
// ACCESSORS
// =========================================================
Mode UI_OLED::getMode() const { return currentMode; }

bool UI_OLED::startRequested() {
    return buttonPressed(JOY_SW_PIN); // joystick button is start
}

bool UI_OLED::stopRequested() {
    return digitalRead(BTN_STOP) == LOW;
}

bool UI_OLED::homingNeeded() const { return needsHoming; }

// =========================================================
// BUTTON HANDLING W/ DEBOUNCE
// =========================================================

bool UI_OLED::buttonPressed(int pin) {
   
    Serial.print("[BPF] checking pin ");
    Serial.print(pin);
    Serial.print(" raw=");
    Serial.println(digitalRead(pin));


    static unsigned long lastPressTime = 0;
    unsigned long now = millis();

    bool pressed;

    if (pin == PIN_BTN_START) {
        // joystick: ACTIVE HIGH
        pressed = (digitalRead(PIN_BTN_START) == HIGH);
        Serial.print("Button Start Pressed");
    } 
    else if (pin == PIN_BTN_STOP) {
        // stop button: ACTIVE LOW
        pressed = (digitalRead(PIN_BTN_STOP) == LOW);
    } 
    else {
        return false;
    }

    if (pressed && (now - lastPressTime > debounceMs)) {
        lastPressTime = now;
        return true;
    }

    return false;
}




// =========================================================
// ANALOG FILTER
// =========================================================
int UI_OLED::smoothRead(int pin) {
    long sum = 0;
    for (int i = 0; i < 6; i++) {
        sum += analogRead(pin);
        delayMicroseconds(200);
    }
    return sum / 6;
}

// =========================================================
// MODE SWITCHING (WITH DEADZONE)
// =========================================================
void UI_OLED::handleJoystickModeSwitch() {
    int x = smoothRead(JOY_X_PIN);
     int xVal = smoothRead(JOY_X_PIN);

    Serial.print("ModeSwitch X=");
    Serial.println(xVal);
    unsigned long now = millis();

    if (now - lastMoveTime < 260) return;

    // DEAD ZONE
    if (x > 430 && x < 600) return;

    if (x < 430) {
        currentMode = (currentMode == MODE_SNAP) ? MODE_INFO : (Mode)(currentMode - 1);
        lastMoveTime = now;
    }
    else if (x > 600) {
        currentMode = (currentMode == MODE_INFO) ? MODE_SNAP : (Mode)(currentMode + 1);
        lastMoveTime = now;
    }
}

// =========================================================
// DRAWING
// =========================================================
void UI_OLED::drawHeaderBar() {
    u8g2.setFont(u8g2_font_t0_11b_tf);
    u8g2.drawStr(2, 10, "DECAP");

    const char* st =
        isRunning ? "RUN" :
        needsHoming ? "HOME" : "IDLE";

    u8g2.setFont(u8g2_font_5x8_tr);
    u8g2.drawStr(90, 10, st);
}

void UI_OLED::drawModeStrip() {
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
        } else {
            u8g2.drawStr(x + 3, y - 1, name);
        }

        x += w + 1;
    }
}

void UI_OLED::drawCentered(const char* t, int y, const uint8_t* font) {
    u8g2.setFont(font);
    int w = u8g2.getStrWidth(t);
    u8g2.drawStr((128 - w) / 2, y, t);
}

void UI_OLED::drawIdleScreen() {
    u8g2.clearBuffer();
    drawHeaderBar();

    char l1[24], l2[24];

    if (currentMode == MODE_SNAP)
        snprintf(l1, 24, "SNAP C:%ld", snapCount);
    else if (currentMode == MODE_SCREW)
        snprintf(l1, 24, "SCREW C:%ld", screwCount);
    else if (currentMode == MODE_INFO) {
        long t = snapCount + screwCount;
        snprintf(l1, 24, "TOTAL:%ld", t);
    }
    else snprintf(l1, 24, "%s", MODE_NAMES[currentMode]);

    snprintf(l2, 24, "START=JOY  STOP=D3");

    drawCentered(l1, 32, u8g2_font_ncenB08_tr);
    drawCentered(l2, 48, u8g2_font_6x10_tf);

    drawModeStrip();
    u8g2.sendBuffer();
}

void UI_OLED::drawProgressScreen(const char* t, const char* ptxt, float p) {
    u8g2.clearBuffer();
    drawHeaderBar();
    drawCentered(t, 26, u8g2_font_ncenB08_tr);

    int bw = 100, bh = 10;
    int bx = 14, by = 40;

    u8g2.drawFrame(bx, by, bw, bh);
    int fill = p * bw;
    if (fill < 0) fill = 0;
    if (fill > bw) fill = bw;

    u8g2.drawBox(bx + 1, by + 1, fill - 2, bh - 2);

    char buf[24];
    snprintf(buf, 24, "%s %d%%", ptxt, (int)(p * 100));
    drawCentered(buf, 60, u8g2_font_6x10_tf);

    u8g2.sendBuffer();
}

// =========================================================
// PHASE (USED BY ROUTINES)
// =========================================================
bool UI_OLED::runPhase(const char* t, const char* s, unsigned long dur) {
    isRunning = true;
    unsigned long start = millis();

    while (millis() - start < dur) {
        if (stopRequested()) {
            needsHoming = true;
            isRunning = false;
            showScreen("STOPPED", "Interrupted");
            delay(600);
            return false;
        }

        float p = float(millis() - start) / dur;
        drawProgressScreen(t, s, p);
        delay(50);
    }

    drawProgressScreen(t, s, 1.0f);
    delay(200);

    isRunning = false;
    return true;
}

void UI_OLED::showScreen(const char* title, const char* subtitle) {
    u8g2.clearBuffer();
    drawHeaderBar();
    drawCentered(title, 32, u8g2_font_ncenB08_tr);
    drawCentered(subtitle, 48, u8g2_font_6x10_tf);
    u8g2.sendBuffer();
}

void UI_OLED::showMessage(const char* title, const char* subtitle, int delayMs) {
    showScreen(title, subtitle);
    delay(delayMs);
}
