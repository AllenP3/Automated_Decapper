#include "ScrewRoutine.h"

void ScrewRoutine::run(LinearActuator& lin, RailStepper& rail, ClawStepper& claw, ServoClaw& servo) {
  Serial.println("Screw routine start.");

  servo.open(140);            // Fully open claw
  lin.moveTo(20);             // Lower to screw cap
  servo.close(60);            // Grip cap

  // Begin unscrewing
  claw.rotateDegrees(360);    // Rotation motion
  lin.moveRelative(-10);      // Raise while unscrewing

  // Cap is still held in claw: do NOT open claw yet
  Serial.println("Screw routine finished.");
}
