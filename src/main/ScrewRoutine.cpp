#include "ScrewRoutine.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"

void ScrewRoutine::run(LinearActuator& lin, RailStepper& rail, ClawStepper& claw, ServoClaw& servo) {
  Serial.println("=== SCREW ROUTINE START ===");

  // OPEN CLAW fully
  Serial.println("Opening claw...");
  servo.open(140);     // tune angle

  // LOWER to cap position
  Serial.println("Lowering to cap...");
  lin.moveTo(20);      // mm from top (tune)

  // CLOSE claw to grip cap
  Serial.println("Gripping cap...");
  servo.close(60);     // tune grip angle
  delay(300);

  // BREAK INITIAL TORQUE
  Serial.println("Breaking torque...");
  claw.rotateDegrees(30);   // small wiggle
  claw.rotateDegrees(-30);  // backwards
  delay(100);

  // BEGIN UNSCREWING WHILE LIFTING
  Serial.println("Unscrewing while lifting...");

  float liftDistance = -12;    // mm upward while unscrewing
  float perTurnLift  = -1.5;   // mm to lift per 90° turn
  int turns = 8;               // 8 × 90° = 720°, usually enough

  for (int i = 0; i < turns; i++) {
    claw.rotateDegrees(90);             // rotate 90°
    lin.moveRelative(perTurnLift);      // lift up simultaneously
    delay(10);
  }

  // FINAL LIFT TO CLEAR THREADS
  Serial.println("Final lift...");
  lin.moveRelative(-8);

  // KEEP CLAW CLOSED (cap held)
  Serial.println("Cap removed. Holding cap safely.");

  Serial.println("=== SCREW ROUTINE COMPLETE ===");
}
