#pragma once
#include <Arduino.h>

class PanelIO {
public:
  enum Mode { MODE_OFF, MODE_SNAP, MODE_SCREW };
  enum Function { FUNC_CALIBRATE, FUNC_EXECUTE };

  void init(uint8_t mA, uint8_t mB, uint8_t funcPin, uint8_t startPin, uint8_t stopPin);
  void update();
  
  Mode mode();
  Function function();

  bool startPressed();
  bool stopPressed();

private:
  uint8_t modeA_pin, modeB_pin, func_pin, start_pin, stop_pin;
  bool lastStart = HIGH;
  bool lastStop = HIGH;
};
