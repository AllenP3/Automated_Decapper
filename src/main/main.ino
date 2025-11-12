#include <Arduino.h>
#include "../io/PanelIO.h"
#include "../motors/LinearActuator.h"
#include "../motors/RailStepper.h"
#include "../motors/ClawStepper.h"
#include "../motors/ServoClaw.h"
#include "../routines/Calibration.h"
#include "../routines/ScrewRoutine.h"
#include "../routines/SnaplinkRoutine.h"

// Global objects
PanelIO panel;
LinearActuator lin;
RailStepper rail;
ClawStepper claw;
ServoClaw servo;

bool running = false;

void setup() {
  Serial.begin(115200);

  // UI panel
  panel.init(24, 25, 26, 27, 28);  // modeA, modeB, func, start, stop

  // Motor initialisation
  lin.init(11, 12, 10);            // step, dir, en
  rail.init(2, 3, 4, 5, 22);       // in1–4, limit switch
  claw.init(6, 7, 8, 9, 23);       // in1–4, hall sensor
  servo.init(13);                  // servo pin
}

void loop() {
  panel.update();

  if (panel.stopPressed()) {
    lin.stop();
    rail.stop();
    claw.stop();
    running = false;
  }

  if (panel.startPressed() && !running) {
    running = true;

    if (panel.mode() == PanelIO::MODE_OFF) {
      Serial.println("System is OFF. Ignoring START.");
      running = false;
    }
    else if (panel.function() == PanelIO::FUNC_CALIBRATE) {
      Calibration::runAll(lin, rail, claw);
    }
    else if (panel.function() == PanelIO::FUNC_EXECUTE) {
      if (panel.mode() == PanelIO::MODE_SCREW)
        ScrewRoutine::run(lin, rail, claw, servo);
      else if (panel.mode() == PanelIO::MODE_SNAP)
        SnaplinkRoutine::run(lin, rail, claw, servo);
    }

    running = false;
  }
}
