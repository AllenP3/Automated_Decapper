#include "LinearActuator.h"

void LinearActuator::init(uint8_t stepPin, uint8_t dirPin, uint8_t enPin) {
  STEP = stepPin;
  DIR = dirPin;
  EN = enPin;

  pinMode(STEP, OUTPUT);
  pinMode(DIR, OUTPUT);
  pinMode(EN, OUTPUT);

  digitalWrite(EN, LOW);
}

void LinearActuator::setSpeed(int microDelay) {
  delayMicros = microDelay;
}

void LinearActuator::stepOnce(bool direction) {
  digitalWrite(DIR, direction);
  digitalWrite(STEP, HIGH);
  delayMicroseconds(delayMicros);
  digitalWrite(STEP, LOW);
  delayMicroseconds(delayMicros);
}

void LinearActuator::home() {
  // Move upward until a LIMIT SWITCH is connected to pin 22
  while (digitalRead(22) == HIGH) { // active LOW
    stepOnce(false);
  }
  positionMM = 0;
}

void LinearActuator::moveTo(float mm) {
  float delta = mm - positionMM;
  moveRelative(delta);
}

void LinearActuator::moveRelative(float mm) {
  long steps = mm * 80; // adjust based on your leadscrew pitch
  bool dir = (steps >= 0);

  for (long i = 0; i < abs(steps); i++) {
    stepOnce(dir);
  }
  positionMM += mm;
}

void LinearActuator::stop() {
  digitalWrite(EN, HIGH);
}

float LinearActuator::getPosition() {
  return positionMM;
}
