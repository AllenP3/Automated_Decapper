#include "ServoClaw.h"

void ServoClaw::init(uint8_t pwmPin) {
  pin = pwmPin;
  servo.attach(pin);
}

void ServoClaw::slowMove(float start, float end, int speed) {
  if (start < end) {
    for (int pos = start; pos <= end; pos++) {
      servo.write(pos);
      delay(speed);
    }
  } else {
    for (int pos = start; pos >= end; pos--) {
      servo.write(pos);
      delay(speed);
    }
  }
}

void ServoClaw::open(float deg) {
  slowMove(servo.read(), deg, 5);
}

void ServoClaw::close(float deg) {
  slowMove(servo.read(), deg, 5);
}
