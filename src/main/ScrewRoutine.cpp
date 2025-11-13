#include "ScrewRoutine.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"

#include "PanelIO.h"
#include "MachineState.h"

extern PanelIO panel;   // declared in main.ino

void ScrewRoutine::run(LinearActuator& lin,
                       RailStepper& rail,
                       ClawStepper& claw,
                       ServoClaw& servo) {
  Serial.println("=== SCREW ROUTINE START ===");
  MachineState::running = true;

  // --- STEP 1: open claw fully ---
  panel.showStatus("SCREW: Opening", "Open claw");
  delay(150);  // short so text appears before movement
  Serial.println("Opening claw...");
  servo.open(140);     // tune angle

  // --- STEP 2: lower to cap position ---
  panel.showStatus("SCREW: Move down", "To cap");
  delay(150);
  Serial.println("Lowering to cap...");
  lin.moveTo(20);      // mm from top (tune)

  // --- STEP 3: close claw to grip cap ---
  panel.showStatus("SCREW: Grip", "Gripping cap");
  delay(150);
  Serial.println("Gripping cap...");
  servo.close(60);     // tune grip angle
  delay(300);

  // --- STEP 4: break initial torque ---
  panel.showStatus("SCREW: Break", "Breaking torque");
  delay(150);
  Serial.println("Breaking torque...");
  claw.rotateDegrees(30);   // small wiggle
  claw.rotateDegrees(-30);  // backwards
  delay(100);

  // --- STEP 5: unscrew while lifting ---
  panel.showStatus("SCREW: Unscrew", "Lift & turn");
  delay(150);
  Serial.println("Unscrewing while lifting...");

  float liftDistance = -12;    // mm upward while unscrewing (not used directly here)
  float perTurnLift  = -1.5;   // mm to lift per 90° turn
  int turns = 8;               // 8 × 90° = 720°, usually enough

  for (int i = 0; i < turns; i++) {
    claw.rotateDegrees(90);             // rotate 90°
    lin.moveRelative(perTurnLift);      // lift up simultaneously
    delay(10);
  }

  // --- STEP 6: final lift to clear threads ---
  panel.showStatus("SCREW: Lift", "Final lift");
  delay(150);
  Serial.println("Final lift...");
  lin.moveRelative(-8);

  // --- STEP 7: hold cap ---
  panel.showStatus("SCREW: Done", "Holding cap");
  delay(150);
  Serial.println("Cap removed. Holding cap safely.");

  Serial.println("=== SCREW ROUTINE COMPLETE ===");

  // mark successful SCREW cycle
  MachineState::screwCount++;
  MachineState::needsHoming = false;

  panel.showDone("SCREW Done", "Process OK");
  delay(1000);

  MachineState::running = false;
  panel.showIdleScreen();
}
