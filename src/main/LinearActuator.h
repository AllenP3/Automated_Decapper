#pragma once
#include <Arduino.h>

class LinearActuator {
public:
  void init(uint8_t stepPin, uint8_t dirPin, uint8_t enPin);

  void setSpeed(int microDelay);
  void setStepsPerMM(float spmm);

  void home();
  void moveTo(float mm);
  void moveRelative(float mm);
  void stop();

  float getPosition();
  void setSoftLimits(float minMM, float maxMM);

private:
  void stepOnce(bool direction);

  uint8_t STEP, DIR, EN;
  float positionMM = 0;
  float stepsPerMM = 80;       // tune to your actuator
  float minPos = 0;            // soft limits
  float maxPos = 100;          // set in setup or calibration
  int delayMicros = 1200;
};
