#include "CalibrationRoutine.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "UI_OLED.h"
#include "Config.h"
#include <Arduino.h>

CalibrationRoutine::CalibrationRoutine(UI_OLED &uiRef,
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

void CalibrationRoutine::begin()
{
    finished = false;
    ui->showMessage("CALIBRATION", "Use joystick", 600);
}

void CalibrationRoutine::update()
{
    if (ui->startRequested()) {
        ui->showMessage("Saved", "Calibration done", 800);
        finished = true;
        return;
    }

    if (ui->stopRequested()) {
        ui->showMessage("Cancelled", "No changes saved", 800);
        finished = true;
        return;
    }

    int x = analogRead(PIN_JOY_X);
    int y = analogRead(PIN_JOY_Y);

    if (x < 400)  claw->rotateSteps(-JOG_CLAW_SPEED_STEPS);
    else if (x > 600) claw->rotateSteps(+JOG_CLAW_SPEED_STEPS);

    if (y < 400)  lin->moveSteps(+JOG_LINEAR_SPEED_STEPS);
    else if (y > 600) lin->moveSteps(-JOG_LINEAR_SPEED_STEPS);

    if (digitalRead(PIN_JOY_SW) == HIGH) servo->close();
    else servo->open();
}
