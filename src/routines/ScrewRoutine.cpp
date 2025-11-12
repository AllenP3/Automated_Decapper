void ScrewRoutine::run(LinearActuator& lin, RailStepper& rail, ClawStepper& claw, ServoClaw& servo) {
  servo.open(120);                         // 1. open claw
  lin.moveTo(20);                          // 2. lower to cap height
  servo.close(60);                         // 3. grip cap
  claw.rotateDegrees(360);                 // 4. begin unscrewing
  lin.moveRelative(-15);                   // 5. simultaneously lift up 15 mm
  // future: add timed coordination & torque feedback
}
