#pragma once
#include <Arduino.h>

class ClawStepper {
public:
  void init(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t hallPin);

  void home();
  void rotateDegrees(float deg);
  void moveSteps(long steps);
  void stop();

  void setStepDelay(int ms);

private:
  void writeStep(int a, int b, int c, int d);

  uint8_t IN1, IN2, IN3, IN4, HALL_PIN;
  int stepDelay = 3;
  float degreesPerStep = 1.8;   // gear ratio adjusted externally

  int seq[8][4] = {
    {1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},
    {0,0,1,0},{0,0,1,1},{0,0,0,1},{1,0,0,1}
  };
};
