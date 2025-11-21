#ifndef SNAPLINK_ROUTINE_H
#define SNAPLINK_ROUTINE_H

#include <Arduino.h>
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "Config.h"

class SnapLinkRoutine {
public:
    enum State {
        IDLE,
        MOVE_RAIL_FORWARD,
        MOVE_LINEAR_DOWN,
        CLOSE_CLAW,
        PULL_UP,
        MOVE_RAIL_BACK,
        DONE
    };

    SnapLinkRoutine();

    void begin();
    void start();
    void update();

    bool isActive() const { return active; }

private:
    State state;
    bool active;

    unsigned long phaseStart;

    // references provided at start()
    LinearActuator* lin;
    RailStepper* rail;
    ClawStepper* claw;
    ServoClaw* servo;
};

#endif
