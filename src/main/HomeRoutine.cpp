#include "HomeRoutine.h"
#include "UI_OLED.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "Config.h"

HomeRoutine::HomeRoutine(UI_OLED &uiRef,
                         LinearActuator &linRef,
                         RailStepper &railRef,
                         ClawStepper &clawRef)
{
    ui = &uiRef;
    lin = &linRef;
    rail = &railRef;
    claw = &clawRef;
}


void HomeRoutine::begin()
{
    finished = false;
    state = START;
}

bool HomeRoutine::run(UI_OLED &ui,
                      LinearActuator &linear,
                      RailStepper &rail,
                      ClawStepper &claw)
{
    if (!ui.runPhase("HOME", "Linear Up", 1500)) return false;
    linear.home();
    delay(200);

    if (!ui.runPhase("HOME", "Rail to Limit", 1500)) return false;
    rail.home();
    delay(200);

    if (!ui.runPhase("HOME", "Claw Align", 1500)) return false;
    claw.home();
    delay(200);

    ui.clearHomingFlag();
    ui.showMessage("HOME DONE", "System Ready");
    return true;
}
