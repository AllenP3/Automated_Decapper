// PanelIO.h
#pragma once
#include <Arduino.h>

class PanelIO {
public:
  // UI modes
  enum Mode { MODE_SNAP, MODE_SCREW, MODE_HOME, MODE_INFO };

  // joyX, joyY, joySW, startPin, stopPin
  void init(uint8_t joyX, uint8_t joyY, uint8_t joySW,
            uint8_t startPin, uint8_t stopPin);

  void update();  // read joystick & refresh idle screen if mode changed

  Mode mode() const { return currentMode; }

  bool startPressed();  // edge-detected START (HIGH->LOW)
  bool stopPressed();   // edge-detected STOP  (HIGH->LOW)

  // basic UI helpers
  void showIdleScreen();
  void showNeedsHoming();
  void showInfoScreen();

  // generic helpers for routines
  void showStatus(const char* line1, const char* line2);

  // blocking phase with spinner + STOP check
  // returns false if STOP pressed -> routine should abort
  bool runPhase(const char* line1,
                const char* baseText,
                unsigned long durationMs);

  void showDone(const char* title, const char* msg);

private:
  uint8_t joyX_pin, joyY_pin, joySW_pin;
  uint8_t start_pin, stop_pin;

  bool lastStart = HIGH;
  bool lastStop  = HIGH;

  Mode currentMode   = MODE_SNAP;
  unsigned long lastJoyMove = 0;
};
