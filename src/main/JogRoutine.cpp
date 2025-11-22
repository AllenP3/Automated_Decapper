#include "JogRoutine.h"
#include "UI_OLED.h"

#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "Config.h"

JogRoutine::JogRoutine(LinearActuator &linRef,
                       RailStepper &railRef,
                       ClawStepper &clawRef,
                       ServoClaw &servoRef)
{
    lin = &linRef;
    rail = &railRef;
    claw = &clawRef;
    servo = &servoRef;
}

void JogRoutine::run(UI_OLED &ui,
                     LinearActuator &linear,
                     RailStepper &rail,
                     ClawStepper &claw,
                     ServoClaw &servo)
{
    ui.showMessage("JOG MODE", "Start/Stop to exit", 700);

    while (true) {

        if (ui.startRequested() || ui.stopRequested()) {
            ui.showMessage("JOG EXIT", "Homing Required", 800);
            return;
        }

        int x = analogRead(JOY_X_PIN);
        int y = analogRead(JOY_Y_PIN);

        if (x < 400) {
            claw.rotateSteps(-JOG_CLAW_SPEED_STEPS);
        }
        else if (x > 600) {
            claw.rotateSteps(+JOG_CLAW_SPEED_STEPS);
        }
        else if (y < 400) {
            rail.moveSteps(+1);
        }
        else if (y > 600) {
            rail.moveSteps(-1);
        }
        else {
            ui.showMessage("JOG MODE", "Idle", 60);
        }
    }
}
