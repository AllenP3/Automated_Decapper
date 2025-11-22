#ifndef HOME_ROUTINE_H
#define HOME_ROUTINE_H

#include <Arduino.h>
#include "BaseRoutine.h"
#include "UI_OLED.h"

// Forward declarations of your motor classes:
class LinearActuator;
class RailStepper;
class ClawStepper;

class HomeRoutine : public BaseRoutine {
public:
    HomeRoutine(LinearActuator &linRef,
                RailStepper &railRef,
                ClawStepper &clawRef);

    void begin() override {}
    void update() override {}
    bool isFinished() override { return finished; }
    const char* getName() override { return "HOME"; }

    static bool run(UI_OLED &ui,
                    LinearActuator &linear,
                    RailStepper &rail,
                    ClawStepper &claw);

private:
    bool finished = false;

    LinearActuator* lin;
    RailStepper* rail;
    ClawStepper* claw;
};


#endif
