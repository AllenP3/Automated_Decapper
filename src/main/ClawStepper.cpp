#include "ClawStepper.h"
#include <Arduino.h>

ClawStepper::ClawStepper() {}

void ClawStepper::begin() {
    pinMode(CLAW_IN1, OUTPUT);
    pinMode(CLAW_IN2, OUTPUT);
    pinMode(CLAW_IN3, OUTPUT);
    pinMode(CLAW_IN4, OUTPUT);

    pinMode(CLAW_HALL_PIN, INPUT_PULLUP);

    currentStep = 0;
    targetStep  = 0;
    seqIndex = 0;
    homed = false;
}

void ClawStepper::update() {
    if (currentStep == targetStep) return;

    unsigned long now = micros();
    if (now - lastStepTime < CLAW_STEP_DELAY_US) return;

    lastStepTime = now;

    int dir = (targetStep > currentStep) ? 1 : -1;

    stepOnce(dir);
    currentStep += dir;
}

void ClawStepper::stepOnce(int dir) {
    seqIndex = (seqIndex + dir) & 7;

    digitalWrite(CLAW_IN1, seq[seqIndex][0]);
    digitalWrite(CLAW_IN2, seq[seqIndex][1]);
    digitalWrite(CLAW_IN3, seq[seqIndex][2]);
    digitalWrite(CLAW_IN4, seq[seqIndex][3]);
}

void ClawStepper::rotateDegrees(float deg) {
    long steps = deg / CLAW_DEGREES_PER_STEP;
    moveToSteps(currentStep + steps);
}

void ClawStepper::rotateToDegrees(float deg) {
    long steps = deg / CLAW_DEGREES_PER_STEP;
    moveToSteps(steps);
}

void ClawStepper::rotateSteps(long steps) {
    moveToSteps(currentStep + steps);
}

void ClawStepper::moveToSteps(long targetSteps) {
    targetStep = targetSteps;
}

void ClawStepper::home() {

    // Rotate until hall sensor is LOW (magnet present)
    while (digitalRead(CLAW_HALL_PIN) == HIGH) {
        stepOnce(-1);
        delayMicroseconds(CLAW_STEP_DELAY_US);
    }

    currentStep = 0;
    targetStep  = 0;
    homed = true;
}
