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
    homed = false;
}

bool RailStepper::limitTriggered() {
    return digitalRead(PIN_RAIL_LIMIT) == LOW; // active LOW
}

bool RailStepper::isBusy() const {
    return currentPos != targetPos;
}

void RailStepper::update() {
    if (!isBusy()) return;

    // If we are moving backwards (negative direction) AND limit is hit → stop instantly
    if (targetPos < currentPos && limitTriggered()) {
        currentPos = 0;
        targetPos = 0;
        homed = true;
        return;
    }

    unsigned long now = micros();
    if (now - lastStepTime < RAIL_STEP_DELAY_US) return;
    lastStepTime = now;

    int dir = (targetPos > currentPos) ? 1 : -1;
    currentPos += dir;
    stepMotor(dir);
}

void RailStepper::moveTo(long pos) {
    targetPos = pos;
}

void RailStepper::moveSteps(long rel) {
    moveTo(currentPos + rel);
}

void RailStepper::home() {
    // drive backwards until limit triggers
    while (!limitTriggered()) {
        stepMotor(-1);
        delayMicroseconds(RAIL_STEP_DELAY_US);
    }

    currentPos = 0;
    targetPos = 0;
    homed = true;
}

void RailStepper::stepMotor(int dir) {
    seqIndex = (seqIndex + dir) & 7;

    digitalWrite(PIN_RAIL_IN1, seq[seqIndex][0]);
    digitalWrite(PIN_RAIL_IN2, seq[seqIndex][1]);
    digitalWrite(PIN_RAIL_IN3, seq[seqIndex][2]);
    digitalWrite(PIN_RAIL_IN4, seq[seqIndex][3]);
}
