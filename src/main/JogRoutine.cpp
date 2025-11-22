#include "JogRoutine.h"
#include "UI_OLED.h"

#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "Config.h"

JogRoutine::JogRoutine(UI_OLED &uiRef,
                       LinearActuator &linRef,
                       RailStepper &railRef,
                       ClawStepper &clawRef,
                       ServoClaw &servoRef)
{
    ui = &uiRef;
    lin = &linRef;
    rail = &railRef;
    claw = &clawRef;
    servo = &servoRef;
}


void JogRoutine::begin()
{
    finished = false;
    ui->showMessage("JOG MODE", "Press to exit", 500);
}

void JogRoutine::update()
{
    // Exit condition
    if (ui->startRequested() || ui->stopRequested()) {
        ui->showMessage("JOG EXIT", "Homing Required", 800);
        finished = true;
        return;
    }

    int x = analogRead(PIN_JOY_X);
    int y = analogRead(PIN_JOY_Y);

    // Claw rotation
    if (x < 400) {
        claw->rotateSteps(-JOG_CLAW_SPEED_STEPS);
        return;
    }
    else if (x > 600) {
        claw->rotateSteps(+JOG_CLAW_SPEED_STEPS);
        return;
    }

    // Rail movement
    if (y < 400) {
        rail->moveSteps(+1);
        return;
    }
    else if (y > 600) {
        rail->moveSteps(-1);
        return;
    }

    // Neutral (idle but non-blocking)
    // do nothing
}
