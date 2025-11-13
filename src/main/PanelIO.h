#pragma once
#include <Arduino.h>

class PanelIO {
public:
  // UI modes
  enum Mode {
    MODE_SNAP,
    MODE_SCREW,
    MODE_HOME,
    MODE_INFO
  };

  void init(uint8_t joyX, uint8_t joyY, uint8_t joySW,
            uint8_t startPin, uint8_t stopPin);

  void update();                  // read joystick & refresh idle screen

  Mode mode() const { return currentMode; }

  bool startPressed();            // edge-detected START
  bool stopPressed();             // edge-detected STOP

  // helper screens used from main or routines
  void showIdleScreen();
  void showNeedsHoming();
  void showInfoScreen();

private:
  // pins
  uint8_t joyX_pin, joyY_pin, joySW_pin;
  uint8_t start_pin, stop_pin;

  // last button states
  bool lastStart = HIGH;
  bool lastStop  = HIGH;

  Mode currentMode   = MODE_SNAP;
  unsigned long lastJoyMove = 0;
};
