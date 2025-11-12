#include "Calibration.h"

void Calibration::runAll(LinearActuator& lin, RailStepper& rail, ClawStepper& claw) {
  Serial.println("Calibrating linear actuator...");
  lin.home();

  Serial.println("Calibrating rail stepper...");
  rail.home();

  Serial.println("Calibrating claw stepper...");
  claw.home();

  Serial.println("Calibration complete.");
}
