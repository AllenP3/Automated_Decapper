#include "CalibrationRoutine.h"

void Calibration::runAll(LinearActuator& lin, RailStepper& rail, ClawStepper& claw) {
  Serial.println("=== CALIBRATION START ===");

  lin.setSoftLimits(0, 100);  // TUNE THIS

  lin.home();
  rail.home();
  claw.home();

  Serial.println("=== CALIBRATION COMPLETE ===");
}
