#include "RailStepper.h"

RailStepper::RailStepper() {}

void RailStepper::begin() {
    pinMode(PIN_RAIL_IN1, OUTPUT);
    pinMode(PIN_RAIL_IN2, OUTPUT);
    pinMode(PIN_RAIL_IN3, OUTPUT);
    pinMode(PIN_RAIL_IN4, OUTPUT);

    pinMode(PIN_RAIL_LIMIT, INPUT_PULLUP);

    currentPos = 0;
    targetPos  = 0;
}

bool RailStepper::isBusy() const {
    return currentPos != targetPos;
}

void RailStepper::update() {
    if (!isBusy()) return;

    unsigned long now = millis();
    if (now - lastStepTime < RAIL_STEP_DELAY_US / 1000) return;
    lastStepTime = now;

    int dir = (targetPos > currentPos) ? 1 : -1;

    currentPos += dir;
    stepMotor(dir);
}

void RailStepper::moveTo(long steps) {
    targetPos = steps;
}

void RailStepper::moveSteps(long rel) {
    targetPos = currentPos + rel;
}

bool RailStepper::limitTriggered() {
    if (RAIL_LIMIT_ACTIVE_LOW)
        return digitalRead(PIN_RAIL_LIMIT) == LOW;
    else
        return digitalRead(PIN_RAIL_LIMIT) == HIGH;
}

void RailStepper::home() {
    // move backwards until trigger
    while (!limitTriggered()) {
        stepMotor(-1);
        delayMicroseconds(RAIL_STEP_DELAY_US);
    }

    // zero reference
    currentPos = 0;
    targetPos  = 0;
    homed = true;
}

void RailStepper::stepMotor(int dir) {
    seqIndex = (seqIndex + dir) & 7;

    digitalWrite(PIN_RAIL_IN1, seq[seqIndex][0]);
    digitalWrite(PIN_RAIL_IN2, seq[seqIndex][1]);
    digitalWrite(PIN_RAIL_IN3, seq[seqIndex][2]);
    digitalWrite(PIN_RAIL_IN4, seq[seqIndex][3]);
}
