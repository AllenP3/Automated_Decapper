#ifndef SNAPLINK_ROUTINE_H
#define SNAPLINK_ROUTINE_H

#include <Arduino.h>
#include "BaseRoutine.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "Config.h"

class SnapLinkRoutine : public BaseRoutine {
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

    SnapLinkRoutine(LinearActuator &linRef,
                    RailStepper &railRef,
                    ClawStepper &clawRef,
                    ServoClaw &servoRef);

    void begin() override;
    void update() override;
    void stop() override { active = false; }

    void start();   // <-- REQUIRED

    bool isFinished() override { return !active; }
    const char* getName() override { return "SNAP"; }

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
