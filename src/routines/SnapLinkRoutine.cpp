#include "SnaplinkRoutine.h"

void SnaplinkRoutine::run(LinearActuator& lin, RailStepper& rail, ClawStepper& claw, ServoClaw& servo) {
  Serial.println("Snaplink routine start.");

  // Placeholder – tune later
  servo.open(130);
  lin.moveTo(15);
  servo.close(50);
  claw.rotateDegrees(120);

  Serial.println("Snaplink routine finished.");
}
