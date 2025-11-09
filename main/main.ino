#include <Servo.h>
#include <AccelStepper.h>

// ====== Pin Definitions ======
#define SERVO_PIN 9
#define ROT_STEP_PIN 2
#define ROT_DIR_PIN 3
#define LIN_STEP_PIN 4
#define LIN_DIR_PIN 5
#define HALL_PIN 6
#define MODE_PIN 7   // Toggle button (Snaplink/Screwcap)
#define ENABLE_PIN 8 // Optional enable pin for both steppers

// ====== Motor Settings ======
#define ROT_MAX_SPEED 600
#define ROT_ACCEL 400
#define LIN_MAX_SPEED 800
#define LIN_ACCEL 400

// ====== Heights and Distances ======
long linear_home = 0;            // Home reference
long screwcap_height = -1000;    // Calibrated screw cap engagement height
long snaplink_height = -1500;    // Calibrated snap cap engagement height
long unscrew_lift_distance = 1500; // Distance to lift while unscrewing
long final_lift_distance = 3000; // Final lift after opening

// ====== Servo Angles ======
int CLAW_OPEN = 70;
int CLAW_CLOSE = 120;

// ====== State Variables ======
Servo claw;
AccelStepper rotStepper(AccelStepper::DRIVER, ROT_STEP_PIN, ROT_DIR_PIN);
AccelStepper linStepper(AccelStepper::DRIVER, LIN_STEP_PIN, LIN_DIR_PIN);

bool screwMode = false;

// ============================================================
// ===================== SETUP ================================
// ============================================================
void setup() {
  Serial.begin(115200);

  pinMode(HALL_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);
  pinMode(ENABLE_PIN, OUTPUT);
  digitalWrite(ENABLE_PIN, LOW); // Enable motors (active low depending on driver)

  claw.attach(SERVO_PIN);
  claw.write(CLAW_OPEN);

  rotStepper.setMaxSpeed(ROT_MAX_SPEED);
  rotStepper.setAcceleration(ROT_ACCEL);

  linStepper.setMaxSpeed(LIN_MAX_SPEED);
  linStepper.setAcceleration(LIN_ACCEL);

  Serial.println("=== Decapper System Initializing ===");

  // Perform initial homing
  homeAll();

  Serial.println("System Ready. Waiting for toggle or command...");
}

// ============================================================
// ===================== LOOP ================================
// ============================================================
void loop() {
  // Read mode from toggle
  screwMode = digitalRead(MODE_PIN) == HIGH;

  if (screwMode) {
    Serial.println("Mode: Screwcap");
    performScrewcapRoutine();
  } else {
    Serial.println("Mode: Snaplink");
    performSnaplinkRoutine();
  }

  delay(5000); // Wait before repeating for testing
}

// ============================================================
// ===================== FUNCTIONS ============================
// ============================================================

void homeAll() {
  Serial.println("Homing linear actuator...");
  linStepper.moveTo(10000); // Move up until limit (simulate)
  while (digitalRead(HALL_PIN) == HIGH) {
    linStepper.runSpeed();
    linStepper.setSpeed(400);
  }
  linStepper.stop();
  linStepper.setCurrentPosition(linear_home);

  Serial.println("Homing rotation...");
  while (digitalRead(HALL_PIN) == HIGH) {
    rotStepper.setSpeed(-300);
    rotStepper.runSpeed();
  }
  rotStepper.stop();
  rotStepper.setCurrentPosition(0);

  Serial.println("Homing complete.");
  claw.write(CLAW_OPEN);
}

// ========== Screw Cap Routine ==========
void performScrewcapRoutine() {
  Serial.println("Starting Screwcap Routine...");

  // 1. Go down to screwcap height
  moveLinearTo(screwcap_height);

  // 2. Close claw
  claw.write(CLAW_CLOSE);
  delay(1000);

  // 3. Unscrew while lifting
  long liftTarget = linStepper.currentPosition() + unscrew_lift_distance;
  for (int i = 0; i < 400; i++) { // number of steps for unscrewing
    rotStepper.moveTo(rotStepper.currentPosition() + 200);
    rotStepper.run();
    linStepper.moveTo(liftTarget);
    linStepper.run();
  }

  // 4. Final lift up
  moveLinearTo(linear_home + final_lift_distance);

  Serial.println("Screwcap unscrewing complete.");
  claw.write(CLAW_OPEN);
  delay(1000);
  homeAll();
}

// ========== Snaplink Routine ==========
void performSnaplinkRoutine() {
  Serial.println("Starting Snaplink Routine...");

  // 1. Move down
  moveLinearTo(snaplink_height);

  // 2. Close claw
  claw.write(CLAW_CLOSE);
  delay(1000);

  // 3. Rotate full turn to snap open
  long snapRotation = rotStepper.currentPosition() + 800; // one full rev (adjust)
  rotStepper.moveTo(snapRotation);
  while (rotStepper.distanceToGo() != 0) {
    rotStepper.run();
  }

  // 4. Lift back up
  moveLinearTo(linear_home + final_lift_distance);

  Serial.println("Snaplink decap complete.");
  claw.write(CLAW_OPEN);
  delay(1000);
  homeAll();
}

// ========== Helper Function ==========
void moveLinearTo(long targetPos) {
  linStepper.moveTo(targetPos);
  while (linStepper.distanceToGo() != 0) {
    linStepper.run();
  }
}
