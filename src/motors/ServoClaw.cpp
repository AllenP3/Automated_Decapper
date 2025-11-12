#include "ServoClaw.h"

void ServoClaw::init(uint8_t pwmPin) {
  pin = pwmPin;
  servo.attach(pin);
}

void ServoClaw::open(float deg) {
  servo.write(deg);
  delay(300);
}

void ServoClaw::close(float deg) {
  servo.write(deg);
  delay(300);
}
