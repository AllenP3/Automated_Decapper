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

void LinearActuator::setStepsPerMM(float spmm) {
  stepsPerMM = spmm;
}

void LinearActuator::setSoftLimits(float minMM, float maxMM) {
  minPos = minMM;
  maxPos = maxMM;
}

void LinearActuator::stepOnce(bool direction) {
  digitalWrite(DIR, direction);
  digitalWrite(STEP, HIGH);
  delayMicroseconds(delayMicros);
  digitalWrite(STEP, LOW);
  delayMicroseconds(delayMicros);
}

void LinearActuator::home() {
  Serial.println("[LinearActuator] Homing...");

  while (digitalRead(22) == HIGH) { // limit switch active LOW
    stepOnce(false);                // UP direction
  }

  // back off for sensor release
  for (int i = 0; i < 80; i++) stepOnce(true);

  positionMM = 0;
  Serial.println("[LinearActuator] Homing complete.");
}

void LinearActuator::moveTo(float mm) {
  moveRelative(mm - positionMM);
}

void LinearActuator::moveRelative(float mm) {
  float target = positionMM + mm;

  if (target < minPos) {
    Serial.println("[LinearActuator] BLOCKED: Under minPos");
    mm = minPos - positionMM;
  }

  if (target > maxPos) {
    Serial.println("[LinearActuator] BLOCKED: Exceeds maxPos");
    mm = maxPos - positionMM;
  }

  long steps = abs(mm) * stepsPerMM;
  bool dir = (mm > 0);

  for (long i = 0; i < steps; i++) {
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
