// =====================================
// TMC2209 Step/Dir Test
// Pins: EN=10, STEP=11, DIR=12
// Motor: 4-wire stepper (Blue-Black-Red-Green)
// =====================================

#define EN_PIN   10   // Enable pin (LOW = driver active)
#define STEP_PIN 11   // Step pulse pin
#define DIR_PIN  12   // Direction pin

// Motion parameters
int stepsPerMove = 20000;          // Number of steps per move
int stepDelayMicros = 50;      // Delay between step pulses (in µs) — smaller = faster

void setup() {
  pinMode(EN_PIN, OUTPUT);
  pinMode(STEP_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);

  // Enable the TMC2209 driver
  digitalWrite(EN_PIN, LOW);     // LOW = enabled
  delay(10);

  // Initial direction
  digitalWrite(DIR_PIN, HIGH);
}

void loop() {
  // Move forward
  digitalWrite(DIR_PIN, HIGH);
  moveMotor(stepsPerMove);
  delay(100);                   // Wait 1 second

  // Move backward
  digitalWrite(DIR_PIN, LOW);
  moveMotor(stepsPerMove);
  delay(100);                   // Wait 1 second
}

void moveMotor(int steps) {
  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(stepDelayMicros);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(stepDelayMicros);
  }
}
