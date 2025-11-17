#ifndef HOME_ROUTINE_H
#define HOME_ROUTINE_H

#include <Arduino.h>
#include "UI_OLED.h"

// Forward declarations of your motor classes:
class LinearActuator;
class RailStepper;
class ClawStepper;

class HomeRoutine {
public:
    static bool run(UI_OLED &ui,
                    LinearActuator &linear,
                    RailStepper &rail,
                    ClawStepper &claw);
};

#endif
