#ifndef CLAW_STEPPER_H
#define CLAW_STEPPER_H

#include <Arduino.h>
#include "Pins.h"
#include "Config.h"

class ClawStepper {
public:
    ClawStepper();

    void begin();
    void update();

    // Movement
    void rotateDegrees(float deg);   // rotate relative
    void rotateToDegrees(float deg); // rotate absolute
    void rotateSteps(long steps);    // raw steps
    void moveToSteps(long targetSteps);

    // Homing
    void home();
    bool isHomed() const { return homed; }

    // State
    bool isBusy() const { return currentStep != targetStep; }

private:
    // hardware stepping
    void stepOnce(int dir);

    // Internal state
    long currentStep = 0;
    long targetStep  = 0;

    unsigned long lastStepTime = 0;

    bool homed = false;

    // 28BYJ half-step sequence
    const uint8_t seq[8][4] = {
        {1,0,0,0},
        {1,1,0,0},
        {0,1,0,0},
        {0,1,1,0},
        {0,0,1,0},
        {0,0,1,1},
        {0,0,0,1},
        {1,0,0,1}
    };

    uint8_t seqIndex = 0;
};

#endif
