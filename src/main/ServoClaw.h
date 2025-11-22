#ifndef SERVO_CLAW_H
#define SERVO_CLAW_H

#include <Arduino.h>
#include <Servo.h>
#include "Pins.h"
#include "Config.h"

class ServoClaw {
public:
    ServoClaw();

    void begin();
    void update();

    // Actions
    void open();
    void close();
    void closeStrong();
    void setCustomTarget(int angle);

    // Status
    bool isBusy() const { return currentAngle != targetAngle; }
    int  getAngle() const { return currentAngle; }

private:
    Servo servo;

    int currentAngle = 90;
    int targetAngle  = 90;

    unsigned long lastStepTime = 0;

    const int moveInterval = SERVO_STEP_INTERVAL_MS;   // from Config.h
    const int stepSize     = SERVO_STEP_SIZE;          // angle increment per update
};

#endif
