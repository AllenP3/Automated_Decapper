#include "RailStepper.h"

void RailStepper::init(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t limitPin) {
  IN1 = a; IN2 = b; IN3 = c; IN4 = d;
  LIMIT_PIN = limitPin;
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(LIMIT_PIN, INPUT_PULLUP);
}

void RailStepper::writeStep(int a, int b, int c, int d) {
  digitalWrite(IN1, a);
  digitalWrite(IN2, b);
  digitalWrite(IN3, c);
  digitalWrite(IN4, d);
}

void RailStepper::home() {
  while (digitalRead(LIMIT_PIN) == HIGH) {
    for (int i = 0; i < 8; i++) {
      writeStep(seq[i][0], seq[i][1], seq[i][2], seq[i][3]);
      delay(3);
    }
  }
  stop();
}

void RailStepper::moveSteps(long steps) {
  bool reverse = (steps < 0);
  long s = abs(steps);

  for (long i = 0; i < s; i++) {
    int idx = i % 8;
    if (reverse) idx = 7 - idx;
    writeStep(seq[idx][0], seq[idx][1], seq[idx][2], seq[idx][3]);
    delay(3);
  }
}

void RailStepper::stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}
