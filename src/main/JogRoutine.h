#ifndef JOG_ROUTINE_H
#define JOG_ROUTINE_H

#include <Arduino.h>
#include "UI_OLED.h"

// Forward declarations
class LinearActuator;
class RailStepper;
class ClawStepper;
class ServoClaw;

class JogRoutine {
public:
    static void run(UI_OLED &ui,
                    LinearActuator &linear,
                    RailStepper &rail,
                    ClawStepper &claw,
                    ServoClaw &servo);
};

#endif
