#include "PanelIO.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "Calibration.h"
#include "ScrewRoutine.h"
#include "SnaplinkRoutine.h"
#include "MachineState.h"

// Global objects
PanelIO      panel;
LinearActuator lin;
RailStepper    rail;
ClawStepper    claw;
ServoClaw      servo;

void setup() {
  Serial.begin(115200);
  MachineState::init();

  // UI: joystick X=A0, Y=A1, SW=4, START=2, STOP=3
  panel.init(A0, A1, 4, 2, 3);

  // Motor initialisation (same as before)
  lin.init(11, 12, 10);            // step, dir, en
  rail.init(2, 3, 4, 5, 22);       // in1–4, limit switch
  claw.init(6, 7, 8, 9, 23);       // in1–4, hall sensor
  servo.init(13);                  // servo pin
}

void loop() {
  panel.update();   // read joystick, refresh OLED idle screen

  // STOP button -> stop motors, mark homing required
  if (panel.stopPressed()) {
    lin.stop();
    rail.stop();
    claw.stop();
    MachineState::running     = false;
    MachineState::needsHoming = true;
    panel.showIdleScreen();
  }

  // START button
  if (panel.startPressed() && !MachineState::running) {
    PanelIO::Mode m = panel.mode();

    // INFO mode: just show counters
    if (m == PanelIO::MODE_INFO) {
      panel.showInfoScreen();
      delay(2000);
      panel.showIdleScreen();
      return;
    }

    // SNAP / SCREW blocked when homing is required
    if (MachineState::needsHoming &&
        (m == PanelIO::MODE_SNAP || m == PanelIO::MODE_SCREW)) {
      panel.showNeedsHoming();
      delay(1500);
      panel.showIdleScreen();
      return;
    }

    MachineState::running = true;

    if (m == PanelIO::MODE_HOME) {
      // homing / calibration
      Calibration::runAll(lin, rail, claw);
      MachineState::needsHoming = false;
    }
    else if (m == PanelIO::MODE_SNAP) {
      SnaplinkRoutine::run(lin, rail, claw, servo);
      MachineState::snapCount++;      // count successful SNAP
      MachineState::needsHoming = false;  // assumes routine includes homing
    }
    else if (m == PanelIO::MODE_SCREW) {
      ScrewRoutine::run(lin, rail, claw, servo);
      MachineState::screwCount++;     // count successful SCREW
      MachineState::needsHoming = false;
    }

    MachineState::running = false;
    panel.showIdleScreen();
  }
}
