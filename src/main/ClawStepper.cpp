#include "CalibrationRoutine.h"

void CalibrationRoutine::run(
    UI_OLED &ui,
    LinearActuator &lin,
    RailStepper &rail,
    ClawStepper &claw,
    ServoClaw &servo
) {
    ui.showMessage("CALIBRATION", "Use joystick");
    delay(800);

    while (true) {

        if (ui.stopRequested()) {
            ui.showMessage("Cancelled", "No changes saved");
            delay(800);
            return;
        }

        if (ui.startRequested()) {
            ui.showMessage("Saved", "Calibration done");
            delay(800);
            return;
        }

        int x = analogRead(PIN_JOY_X);
        int y = analogRead(PIN_JOY_Y);

        if (x < 400) claw.rotateSteps(-JOG_CLAW_SPEED_STEPS);
        else if (x > 600) claw.rotateSteps(JOG_CLAW_SPEED_STEPS);

        if (y < 400) lin.moveSteps(JOG_LINEAR_SPEED_STEPS);
        else if (y > 600) lin.moveSteps(-JOG_LINEAR_SPEED_STEPS);

        if (digitalRead(PIN_JOY_SW) == LOW) servo.close();
        else servo.open();
    }
}
