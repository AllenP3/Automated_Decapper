#include "JogRoutine.h"
#include "UI_OLED.h"
#include "Arduino.h"

// JOG mode uses joystick for manual motion.
// Exits on START or STOP button.

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

        const int low = 400, high = 600;

        if (x < low) {
            // Rotate Claw - direction
            claw.step(-1);
            ui.showMessage("JOG", "Rotate -", 50);
        }
        else if (x > high) {
            // Rotate Claw + direction
            claw.step(+1);
            ui.showMessage("JOG", "Rotate +", 50);
        }
        else if (y < low) {
            // Fine up/down for rail stepper?
            rail.step(+1);
            ui.showMessage("JOG", "Rail +", 50);
        }
        else if (y > high) {
            rail.step(-1);
            ui.showMessage("JOG", "Rail -", 50);
        }
        else {
            // idle update (draw idle UI in jog format)
            ui.drawMessage("JOG MODE", "Idle");
            delay(60);
        }
    }
}
