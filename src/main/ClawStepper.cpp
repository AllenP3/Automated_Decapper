#include "ClawStepper.h"

void ClawStepper::init(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t hallPin) {
  IN1=a; IN2=b; IN3=c; IN4=d;
  HALL_PIN = hallPin;

  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  pinMode(HALL_PIN, INPUT_PULLUP);
}

void ClawStepper::setStepDelay(int ms) {
  stepDelay = ms;
}

void ClawStepper::writeStep(int a, int b, int c, int d) {
  digitalWrite(IN1,a);
  digitalWrite(IN2,b);
  digitalWrite(IN3,c);
  digitalWrite(IN4,d);
}

void ClawStepper::home() {
  Serial.println("[ClawStepper] Homing...");

  while (digitalRead(HALL_PIN) == HIGH) {
    moveSteps(-1);
  }

  // Back off slightly
  moveSteps(20);

  Serial.println("[ClawStepper] Homing complete.");
}

void ClawStepper::moveSteps(long steps) {
  bool rev = (steps < 0);
  steps = abs(steps);

  for (long i = 0; i < steps; i++) {
    int idx = i % 8;
    if (rev) idx = 7 - idx;

    writeStep(seq[idx][0], seq[idx][1], seq[idx][2], seq[idx][3]);
    delay(stepDelay);
  }
}

void ClawStepper::rotateDegrees(float deg) {
  long steps = deg / (degreesPerStep);
  moveSteps(steps);
}

void ClawStepper::stop() {
  writeStep(0,0,0,0);
}
