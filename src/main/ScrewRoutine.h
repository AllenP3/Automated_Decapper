#ifndef SCREW_ROUTINE_H
#define SCREW_ROUTINE_H

#include <Arduino.h>
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "Config.h"

class ScrewRoutine {
public:
    enum State {
        IDLE,
        MOVE_RAIL_FORWARD,
        MOVE_LINEAR_DOWN,
        CLOSE_CLAW,
        BREAK_TORQUE,
        UNSCREW_ROTATE,
        LIFT_UP,
        MOVE_RAIL_BACK,
        DONE
    };

    ScrewRoutine();

    void begin();
    void start();
    void update();
    bool isActive() const { return active; }

private:
    State state;
    bool active;

    unsigned long phaseStart;

    LinearActuator* lin;
    RailStepper* rail;
    ClawStepper* claw;
    ServoClaw* servo;
};

#endif
