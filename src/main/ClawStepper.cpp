#include "ClawStepper.h"

ClawStepper::ClawStepper() {}

void ClawStepper::begin() {
    pinMode(PIN_CLAW_IN1, OUTPUT);
    pinMode(PIN_CLAW_IN2, OUTPUT);
    pinMode(PIN_CLAW_IN3, OUTPUT);
    pinMode(PIN_CLAW_IN4, OUTPUT);

    pinMode(PIN_CLAW_HALL, INPUT_PULLUP);

    homed = false;
}

void ClawStepper::update() {
    if (!isBusy()) return;

    if (millis() - lastStepTime < CLAW_STEP_DELAY_US) return;
    lastStepTime = millis();

    int dir = (targetStep > currentStep) ? +1 : -1;
    currentStep += dir;

    stepOnce(dir);
}

void ClawStepper::stepOnce(int dir) {
    seqIndex = (seqIndex + dir) & 7;

    digitalWrite(PIN_CLAW_IN1, seq[seqIndex][0]);
    digitalWrite(PIN_CLAW_IN2, seq[seqIndex][1]);
    digitalWrite(PIN_CLAW_IN3, seq[seqIndex][2]);
    digitalWrite(PIN_CLAW_IN4, seq[seqIndex][3]);
}

void ClawStepper::rotateSteps(long steps) {
    targetStep = currentStep + steps;
}

void ClawStepper::moveToSteps(long tgt) {
    targetStep = tgt;
}

void ClawStepper::rotateDegrees(float deg) {
    long steps = deg * CLAW_STEPS_PER_DEG;  
    rotateSteps(steps);
}

void ClawStepper::rotateToDegrees(float deg) {
    long tgt = deg * CLAW_STEPS_PER_DEG;
    moveToSteps(tgt);
}

void ClawStepper::home() {
    // Move slowly clockwise until hall sensor triggers
    while (true) {
        bool hallTriggered = CLAW_HALL_ACTIVE_LOW
                             ? (digitalRead(PIN_CLAW_HALL) == LOW)
                             : (digitalRead(PIN_CLAW_HALL) == HIGH);

        if (hallTriggered) break;

        stepOnce(+1);
        delayMicroseconds(CLAW_HOMING_STEP_DELAY_US);
    }

    // Small retreat for clean-zero
    for (int i = 0; i < 30; i++) {
        stepOnce(-1);
        delayMicroseconds(CLAW_HOMING_STEP_DELAY_US);
    }

    currentStep = 0;
    targetStep  = 0;

    homed = true;
}
