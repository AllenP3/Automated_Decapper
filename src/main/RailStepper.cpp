#include "RailStepper.h"

void RailStepper::init(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t limitPin) {
  IN1=a; IN2=b; IN3=c; IN4=d;
  LIMIT_PIN = limitPin;

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(LIMIT_PIN, INPUT_PULLUP);
}

void RailStepper::setStepDelay(int ms) {
  stepDelay = ms;
}

void RailStepper::writeStep(int a, int b, int c, int d) {
  digitalWrite(IN1,a);
  digitalWrite(IN2,b);
  digitalWrite(IN3,c);
  digitalWrite(IN4,d);
}

void RailStepper::home() {
  Serial.println("[RailStepper] Homing...");

  while (digitalRead(LIMIT_PIN) == HIGH) {
    moveSteps(-1);
  }

  stop();
  Serial.println("[RailStepper] Homed.");
}

void RailStepper::moveSteps(long steps) {
  bool rev = (steps < 0);
  steps = abs(steps);

  for (long i = 0; i < steps; i++) {
    int idx = i % 8;
    if (rev) idx = 7 - idx;

    writeStep(seq[idx][0], seq[idx][1], seq[idx][2], seq[idx][3]);
    delay(stepDelay);
  }
}

void RailStepper::fastMoveSteps(long steps) {
  int old = stepDelay;
  stepDelay = 1;
  moveSteps(steps);
  stepDelay = old;
}

void RailStepper::stop() {
  writeStep(0,0,0,0);
}
