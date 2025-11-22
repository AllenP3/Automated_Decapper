#ifndef JOG_ROUTINE_H
#define JOG_ROUTINE_H

#include <Arduino.h>
#include "BaseRoutine.h"
#include "UI_OLED.h"

// Forward declarations
class LinearActuator;
class RailStepper;
class ClawStepper;
class ServoClaw;

class JogRoutine : public BaseRoutine {
public:
    JogRoutine(LinearActuator &linRef,
               RailStepper &railRef,
               ClawStepper &clawRef,
               ServoClaw &servoRef);

    void begin() override {}
    void update() override {}
    bool isFinished() override { return false; }
    const char* getName() override { return "JOG"; }

    static void run(UI_OLED &ui,
                    LinearActuator &linear,
                    RailStepper &rail,
                    ClawStepper &claw,
                    ServoClaw &servo);

private:
    LinearActuator* lin;
    RailStepper* rail;
    ClawStepper* claw;
    ServoClaw* servo;
};


#endif
