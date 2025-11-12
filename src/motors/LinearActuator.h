#pragma once
#include <Arduino.h>

class LinearActuator {
public:
  void init(uint8_t stepPin, uint8_t dirPin, uint8_t enPin);
  void setSpeed(int microDelay);
  void home();
  void moveTo(float mm);
  void moveRelative(float mm);
  void stop();
  float getPosition();

private:
  void stepOnce(bool direction);
  float positionMM = 0;
  int delayMicros = 1200;
  uint8_t STEP, DIR, EN;
};
