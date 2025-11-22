#ifndef RAIL_STEPPER_H
#define RAIL_STEPPER_H

#include <Arduino.h>
#include "Pins.h"
#include "Config.h"

class RailStepper {
public:
    RailStepper();

    void begin();
    void update();

    // Motion control
    void moveTo(long positionSteps);      // absolute
    void moveSteps(long relSteps);        // relative
    bool isBusy() const;

    // Homing
    void home();
    bool isHomed() const { return homed; }

    long getPosition() const { return currentPos; }

private:
    // Step sequence for 28BYJ-48
    const uint8_t seq[8][4] = {
        {1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,1,1,0},
        {0,0,1,0}, {0,0,1,1}, {0,0,0,1}, {1,0,0,1}
    };

    long currentPos = 0;
    long targetPos = 0;

    uint8_t seqIndex = 0;

    unsigned long lastStepTime = 0;

    bool homed = false;

    void stepMotor(int dir);
    bool limitTriggered();

};

#endif
