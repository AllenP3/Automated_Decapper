// PanelIO.cpp
#include "PanelIO.h"
#include "MachineState.h"

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

// OLED, I2C addr 0x3C (change if your module uses something else)
static Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// small helper
static void drawTwoLines(const char* l1, const char* l2) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(l1);
  display.setCursor(0, 16);
  display.print(l2);
  display.display();
}

// ------------ init ------------
void PanelIO::init(uint8_t joyX, uint8_t joyY, uint8_t joySW,
                   uint8_t startPin, uint8_t stopPin) {

  joyX_pin   = joyX;
  joyY_pin   = joyY;
  joySW_pin  = joySW;
  start_pin  = startPin;
  stop_pin   = stopPin;

  pinMode(joyX_pin,   INPUT);
  pinMode(joyY_pin,   INPUT);
  pinMode(joySW_pin,  INPUT_PULLUP);

  pinMode(start_pin,  INPUT_PULLUP);
  pinMode(stop_pin,   INPUT_PULLUP);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    // OLED not found, block here
    for(;;) {}
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Decapping Panel");
  display.setCursor(0, 16);
  display.print("Initializing...");
  display.display();
  delay(1000);

  showIdleScreen();
}

// ------------ update ------------
void PanelIO::update() {
  int xVal = analogRead(joyX_pin);
  unsigned long now = millis();
  const unsigned long joyMoveDelay = 250;

  if (now - lastJoyMove < joyMoveDelay) return;

  if (xVal < 350) {
    // LEFT
    if (currentMode == MODE_SNAP) currentMode = MODE_INFO;
    else currentMode = (Mode)((int)currentMode - 1);
    lastJoyMove = now;
    showIdleScreen();
  } else if (xVal > 700) {
    // RIGHT
    if (currentMode == MODE_INFO) currentMode = MODE_SNAP;
    else currentMode = (Mode)((int)currentMode + 1);
    lastJoyMove = now;
    showIdleScreen();
  }
}

// ------------ button helpers ------------
bool PanelIO::startPressed() {
  bool now = digitalRead(start_pin);
  bool pressed = (lastStart == HIGH && now == LOW);
  lastStart = now;
  return pressed;
}

bool PanelIO::stopPressed() {
  bool now = digitalRead(stop_pin);
  bool pressed = (lastStop == HIGH && now == LOW);
  lastStop = now;
  return pressed;
}

// ------------ screen helpers ------------
void PanelIO::showIdleScreen() {
  const char* modeStr;
  switch (currentMode) {
    case MODE_SNAP:  modeStr = "SNAP";  break;
    case MODE_SCREW: modeStr = "SCREW"; break;
    case MODE_HOME:  modeStr = "HOME";  break;
    case MODE_INFO:  modeStr = "INFO";  break;
    default:         modeStr = "???";   break;
  }

  char line1[21];

  if (currentMode == MODE_SNAP) {
    snprintf(line1, sizeof(line1), "SNAP C:%3ld", MachineState::snapCount);
  } else if (currentMode == MODE_SCREW) {
    snprintf(line1, sizeof(line1), "SCREW C:%3ld", MachineState::screwCount);
  } else if (currentMode == MODE_INFO) {
    long total = MachineState::snapCount + MachineState::screwCount;
    snprintf(line1, sizeof(line1), "INFO T:%3ld", total);
  } else {
    snprintf(line1, sizeof(line1), "MODE: %s", modeStr);
  }

  char line2[21];
  if (MachineState::needsHoming) {
    snprintf(line2, sizeof(line2), "Home required");
  } else {
    snprintf(line2, sizeof(line2), "START D2/STOP D3");
  }

  drawTwoLines(line1, line2);
}

void PanelIO::showNeedsHoming() {
  drawTwoLines("Homing required", "Select HOME");
}

void PanelIO::showInfoScreen() {
  char line1[21];
  char line2[21];
  long total = MachineState::snapCount + MachineState::screwCount;

  snprintf(line1, sizeof(line1), "Tot:%3ld SNAP:%3ld",
           total, MachineState::snapCount);
  snprintf(line2, sizeof(line2), "SCREW:%3ld", MachineState::screwCount);

  drawTwoLines(line1, line2);
}

void PanelIO::showStatus(const char* line1, const char* line2) {
  drawTwoLines(line1, line2);
}

// --- phase with spinner + STOP check ---
// returns false if STOP pressed, true if finished
bool PanelIO::runPhase(const char* line1,
                       const char* baseText,
                       unsigned long durationMs) {
  const char spinner[4] = {'|', '/', '-', '\\'};
  int spinIndex = 0;
  unsigned long start = millis();

  while (millis() - start < durationMs) {
    // STOP check
    if (digitalRead(stop_pin) == LOW) {
      MachineState::needsHoming = true;
      MachineState::running     = false;
      drawTwoLines("Stopped", "STOP pressed");
      delay(800);
      return false;
    }

    char line2[21];
    snprintf(line2, sizeof(line2), "%-12s %c", baseText, spinner[spinIndex]);
    spinIndex = (spinIndex + 1) % 4;

    drawTwoLines(line1, line2);
    delay(150);
  }
  return true;
}

void PanelIO::showDone(const char* title, const char* msg) {
  drawTwoLines(title, msg);
}
