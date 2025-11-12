#pragma once
#include <Arduino.h>
#include <Servo.h>

class ServoClaw {
public:
  void init(uint8_t pwmPin);
  void open(float deg);
  void close(float deg);

private:
  Servo servo;
  uint8_t pin;
};
