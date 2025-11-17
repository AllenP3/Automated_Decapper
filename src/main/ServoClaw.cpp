#include "ServoClaw.h"

ServoClaw::ServoClaw() {}

void ServoClaw::begin() {
    servo.attach(PIN_SERVO_CLAW);
    currentAngle = SERVO_INITIAL_ANGLE;   // from Config.h
    targetAngle  = currentAngle;
    servo.write(currentAngle);
}

void ServoClaw::update() {
    if (currentAngle == targetAngle) return;

    unsigned long now = millis();
    if (now - lastStepTime < moveInterval) return;

    lastStepTime = now;

    // Smooth movement towards target
    if (currentAngle < targetAngle)
        currentAngle += stepSize;
    else
        currentAngle -= stepSize;

    // Clamp so we never overshoot
    if (abs(currentAngle - targetAngle) < stepSize)
        currentAngle = targetAngle;

    servo.write(currentAngle);
}

/* ======================
   SERVO ACTION COMMANDS
   ====================== */

void ServoClaw::open() {
    targetAngle = SERVO_OPEN_ANGLE;
}

void ServoClaw::close() {
    targetAngle = SERVO_CLOSE_ANGLE;
}

void ServoClaw::closeStrong() {
    targetAngle = SERVO_STRONG_CLOSE_ANGLE;
}

void ServoClaw::setCustomTarget(int angle) {
    targetAngle = constrain(angle, 0, 180);
}
