#ifndef HOME_ROUTINE_H
#define HOME_ROUTINE_H

#include <Arduino.h>
#include "BaseRoutine.h"

// Forward declarations
class UI_OLED;
class LinearActuator;
class RailStepper;
class ClawStepper;

class HomeRoutine : public BaseRoutine {
public:
    enum State {
        START,
        MOVE_LINEAR_UP,
        WAIT_LINEAR,
        MOVE_RAIL_HOME,
        WAIT_RAIL,
        MOVE_CLAW_HOME,
        WAIT_CLAW,
        DONE
    };

    HomeRoutine(UI_OLED &uiRef,
                LinearActuator &linRef,
                RailStepper &railRef,
                ClawStepper &clawRef);

    void begin() override;
    void update() override;
    bool isFinished() override { return finished; }
    const char* getName() override { return "HOME"; }

private:
    LinearActuator* lin;
    RailStepper* rail;
    ClawStepper* claw;

    State state = START;
    bool finished = false;

    unsigned long phaseStart = 0;
    const unsigned long phaseDuration = 1200; // animation time
};

#endif
