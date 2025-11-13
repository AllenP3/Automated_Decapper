#include "SnaplinkRoutine.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"

#include "PanelIO.h"
#include "MachineState.h"

extern PanelIO panel;   // defined in main.ino

void SnaplinkRoutine::run(LinearActuator& lin,
                          RailStepper& rail,
                          ClawStepper& claw,
                          ServoClaw& servo) {
  Serial.println("=== SNAPLINK ROUTINE START ===");
  MachineState::running = true;

  // --- STEP 1: open claw wide ---
  panel.showStatus("SNAP: Opening", "Open claw");
  delay(150);  // short so you can see text before motion
  Serial.println("Opening claw...");
  servo.open(135);   // adjust to your linkage width

  // --- STEP 2: move down to snaplink height ---
  panel.showStatus("SNAP: Move down", "To snaplink");
  delay(150);
  Serial.println("Lowering into position...");
  lin.moveTo(14);    // tune exact height

  // --- STEP 3: close claw partially ---
  panel.showStatus("SNAP: Grip", "Light grip");
  delay(150);
  Serial.println("Gripping snaplink lightly...");
  servo.close(80);   // partial grip, tune as needed
  delay(300);

  // --- STEP 4: rotate to engage snaplink ---
  panel.showStatus("SNAP: Rotate", "Engage link");
  delay(150);
  Serial.println("Rotating to engage snaplink...");
  claw.rotateDegrees(45);   // gentle rotation
  delay(50);

  // --- STEP 5: pull up to release snaplink ---
  panel.showStatus("SNAP: Pull", "Release link");
  delay(150);
  Serial.println("Pulling to release...");
  lin.moveRelative(-5);     // upward snap
  delay(100);

  // --- STEP 6: rotate back to neutral ---
  panel.showStatus("SNAP: Rotate back", "Neutral");
  delay(150);
  Serial.println("Rotating back to neutral...");
  claw.rotateDegrees(-45);

  // --- STEP 7: move up to safe height ---
  panel.showStatus("SNAP: Move up", "Safe height");
  delay(150);
  Serial.println("Returning to safe height...");
  lin.moveTo(0);

  // --- STEP 8: open claw to release ---
  panel.showStatus("SNAP: Release", "Open claw");
  delay(150);
  Serial.println("Releasing snaplink...");
  servo.open(140);

  Serial.println("=== SNAPLINK ROUTINE COMPLETE ===");

  // mark as successful cycle
  MachineState::snapCount++;
  MachineState::needsHoming = false;

  panel.showDone("SNAP Done", "Process OK");
  delay(1000);

  MachineState::running = false;
  panel.showIdleScreen();
}
