#pragma once
#include <Arduino.h>
#include <Servo.h>

class ServoClaw {
public:
  void init(uint8_t pwmPin);

  void open(float deg);
  void close(float deg);

  void slowMove(float start, float end, int speed);

private:
  Servo servo;
  uint8_t pin;
};
