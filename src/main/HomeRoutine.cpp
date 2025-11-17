#include "HomeRoutine.h"
#include "UI_OLED.h"
#include "Arduino.h"

// -------------------------------------
// HOMING ROUTINE (linear, rail, claw)
// -------------------------------------

bool HomeRoutine::run(UI_OLED &ui,
                      LinearActuator &linear,
                      RailStepper &rail,
                      ClawStepper &claw)
{
    // 1) Home Linear Actuator
    if (!ui.runPhase("HOME", "Linear Up", 1500)) {
        return false;
    }
    linear.home();   
    delay(200);

    // 2) Home Rail Stepper
    if (!ui.runPhase("HOME", "Rail to Limit", 1500)) {
        return false;
    }
    rail.home();     
    delay(200);

    // 3) Home Claw Rotation (Hall Sensor)
    if (!ui.runPhase("HOME", "Claw Align", 1500)) {
        return false;
    }
    claw.home();     
    delay(200);

    // Clear homing requirement
    ui.clearHomingFlag();

    ui.showMessage("HOME DONE", "System Ready");
    return true;
}
