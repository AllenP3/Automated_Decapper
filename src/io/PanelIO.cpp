#include "PanelIO.h"

void PanelIO::init(uint8_t mA, uint8_t mB, uint8_t funcPin, uint8_t startPin, uint8_t stopPin) {
  modeA_pin = mA;
  modeB_pin = mB;
  func_pin = funcPin;
  start_pin = startPin;
  stop_pin = stopPin;

  pinMode(modeA_pin, INPUT_PULLUP);
  pinMode(modeB_pin, INPUT_PULLUP);
  pinMode(func_pin, INPUT_PULLUP);
  pinMode(start_pin, INPUT_PULLUP);
  pinMode(stop_pin, INPUT_PULLUP);
}

void PanelIO::update() {
  // no debounce yet (can add later)
}

PanelIO::Mode PanelIO::mode() {
  bool a = digitalRead(modeA_pin);
  bool b = digitalRead(modeB_pin);

  if (!a && !b) return MODE_OFF;
  if (a && !b)  return MODE_SNAP;
  if (!a && b)  return MODE_SCREW;

  return MODE_OFF;
}

PanelIO::Function PanelIO::function() {
  return (digitalRead(func_pin) == HIGH) ? FUNC_CALIBRATE : FUNC_EXECUTE;
}

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
