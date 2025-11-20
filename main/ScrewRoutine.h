#pragma once
#include <Arduino.h>
#include <Servo.h>
#include "LinearActuator.h"

class ScrewRoutine {
public:
    ScrewRoutine(Servo* claw,
                 int openAngle,
                 int closeAngle,
                 int stepPins[4]);

    void begin();

    // sequence
    void run_Sequence(LinearActuator& actuator);

private:

    // Servo
    Servo* claw;
    int openAngle;
    int closeAngle;

    // Stepper
    int motorPins[4];
    int stepIndex;

    const int seq[8][4] = {
        {1,0,0,0},
        {1,1,0,0},
        {0,1,0,0},
        {0,1,1,0},
        {0,0,1,0},
        {0,0,1,1},
        {0,0,0,1},
        {1,0,0,1}
    };

    // the three basic functions
    void openClaw();
    void closeClaw();
    void rotateOneStepCW();
};
