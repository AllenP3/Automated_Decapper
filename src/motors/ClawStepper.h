#pragma once
#include <Arduino.h>

class ClawStepper {
public:
  void init(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t hallPin);
  void home();
  void rotateDegrees(float deg);
  void moveSteps(long steps);
  void stop();

private:
  void writeStep(int a, int b, int c, int d);

  uint8_t IN1, IN2, IN3, IN4;
  uint8_t HALL_PIN;
  int seq[8][4] = {
    {1,0,0,0},{1,1,0,0},{0,1,0,0},{0,1,1,0},
    {0,0,1,0},{0,0,1,1},{0,0,0,1},{1,0,0,1}
  };
};
