#include "Calibration.h"

void Calibration::runAll(LinearActuator& lin, RailStepper& rail, ClawStepper& claw) {
  Serial.println("=== CALIBRATION START ===");

  lin.setSoftLimits(0, 100);  // YOU TUNE THIS

  lin.home();
  rail.home();
  claw.home();

  Serial.println("=== CALIBRATION COMPLETE ===");
}
