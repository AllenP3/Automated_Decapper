#include "SnaplinkRoutine.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"

void SnaplinkRoutine::run(LinearActuator& lin, RailStepper& rail, ClawStepper& claw, ServoClaw& servo) {
  Serial.println("=== SNAPLINK ROUTINE START ===");

  // OPEN CLAW wide
  Serial.println("Opening claw...");
  servo.open(135);   // adjust to your linkage width

  // MOVE DOWN to snaplink height
  Serial.println("Lowering into position...");
  lin.moveTo(14);    // tune exact height

  // CLOSE CLAW PARTIALLY — not too tight
  Serial.println("Gripping snaplink lightly...");
  servo.close(80);   // partial grip, tune as needed
  delay(300);

  // ROTATE to engage snaplink
  Serial.println("Rotating to engage snaplink...");
  claw.rotateDegrees(45);   // gentle rotation
  delay(50);

  // PULL UP sharply to release snaplink
  Serial.println("Pulling to release...");
  lin.moveRelative(-5);     // upward snap
  delay(100);

  // SMALL ROTATION BACK to neutral
  claw.rotateDegrees(-45);

  // MOVE UP to safe height
  Serial.println("Returning to safe height...");
  lin.moveTo(0);

  // OPEN CLAW to release link
  Serial.println("Releasing snaplink...");
  servo.open(140);

  Serial.println("=== SNAPLINK ROUTINE COMPLETE ===");
}
