<<<<<<< Updated upstream
// =======================================================
// Continuous Parallel Motion (Final - Stepper1 = Rotation)
// Servo + 2x 28BYJ + Linear Actuator (TMC2209)
// Allen | Nov 2025
// =======================================================

#include <Servo.h>

// ---------- Pin setup ----------
int rotPins[4] = {2, 3, 4, 5};     // Stepper 1 = Rotation
int finePins[4] = {6, 7, 8, 9};    // Stepper 2 = Fine adjustment
#define SERVO_PIN 13
#define EN_PIN   10
#define STEP_PIN 11
#define DIR_PIN  12

// ---------- Motion tuning ----------
int servoOpen = 150;        // Servo open angle
int servoClose = 80;        // Servo close angle
int servoInterval = 1500;   // ms between open/close

int rotLimit = 4048;        // Stepper1 rotation range (≈180°)
int fineLimit = 4000;        // Stepper2 fine oscillation range
int rotDelay = 1;           // smaller = faster (ms per microstep)
int fineDelay = 1;

float stepsPerMM = 600;     // actuator calibration
float distanceMM = 50;      // stroke each way
int actDelayMicros = 200;   // µs between actuator steps

// ---------- Internal state ----------
unsigned long lastServoTime = 0;
bool servoState = false;
Servo claw;

int seq[8][4] = {
  {1,0,0,0}, {1,1,0,0}, {0,1,0,0}, {0,1,1,0},
  {0,0,1,0}, {0,0,1,1}, {0,0,0,1}, {1,0,0,1}
};

int rotIdx=0, fineIdx=0;
int rotDir=1, fineDir=1;
int rotCount=0, fineCount=0;

bool actDir=true;
long actSteps=0, actTotal;
unsigned long lastRot=0, lastFine=0, lastAct=0;

// ---------- Setup ----------
void setup() {
  Serial.begin(9600);
  for (int i=0;i<4;i++){ pinMode(rotPins[i],OUTPUT); pinMode(finePins[i],OUTPUT); }

  claw.attach(SERVO_PIN);
  claw.write(servoClose);

  pinMode(EN_PIN,OUTPUT); pinMode(STEP_PIN,OUTPUT); pinMode(DIR_PIN,OUTPUT);
  digitalWrite(EN_PIN,LOW); // enable TMC2209
  actTotal = distanceMM * stepsPerMM;

  Serial.println("Parallel Motion Running ✅");
}

// ---------- Loop ----------
void loop() {
  unsigned long now = millis();

  // --- Servo ---
  if (now - lastServoTime > servoInterval) {
    servoState = !servoState;
    claw.write(servoState ? servoOpen : servoClose);
    lastServoTime = now;
  }

  // --- Stepper1: Rotation ---
  if (now - lastRot > rotDelay) {
    rotIdx += rotDir; if (rotIdx>=8) rotIdx=0; if (rotIdx<0) rotIdx=7;
    stepSeq(rotPins, rotIdx);
    rotCount += rotDir;
    if (abs(rotCount) >= rotLimit) { rotDir*=-1; rotCount=0; }
    lastRot = now;
  }

  // --- Stepper2: Fine adjustment ---
  if (now - lastFine > fineDelay) {
    fineIdx += fineDir; if (fineIdx>=8) fineIdx=0; if (fineIdx<0) fineIdx=7;
    stepSeq(finePins, fineIdx);
    fineCount += fineDir;
    if (abs(fineCount) >= fineLimit) { fineDir*=-1; fineCount=0; }
    lastFine = now;
  }

  // --- Linear actuator ---
  if (micros() - lastAct > actDelayMicros) {
    digitalWrite(DIR_PIN, actDir ? HIGH : LOW);
    digitalWrite(STEP_PIN, HIGH); delayMicroseconds(2); digitalWrite(STEP_PIN, LOW);
    actSteps++;
    if (actSteps >= actTotal) { actSteps=0; actDir=!actDir; }
    lastAct = micros();
  }
}

// ---------- Step driver ----------
void stepSeq(int p[4], int idx){
  for (int i=0;i<4;i++) digitalWrite(p[i], seq[idx][i]);
=======
#include <TMCStepper.h>
#include <AccelStepper.h>
#include <Servo.h>

// =====================================================
// TMC2209 LINEAR ACTUATOR
// =====================================================
#define EN_PIN     10
#define STEP_PIN   11
#define DIR_PIN    12

HardwareSerial & UART_PORT = Serial3;
#define R_SENSE        0.11
#define DRIVER_ADDRESS 0b00

TMC2209Stepper driver(&UART_PORT, R_SENSE, DRIVER_ADDRESS);
AccelStepper tmcStepper(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// =====================================================
// JOYSTICK
// =====================================================
#define JOY_Y A0      // controls servo + stepper A + actuator
#define JOY_X A1      // controls stepper B only
int deadZone = 80;

// =====================================================
// SERVO
// =====================================================
Servo clawServo;
#define SERVO_PIN 39
int servoPos = 90;

// =====================================================
// 28BYJ #1 (Y-axis)
// =====================================================
#define IN1_A 31
#define IN2_A 33
#define IN3_A 35
#define IN4_A 37

// =====================================================
// 28BYJ #2 (X-axis)
// =====================================================
#define IN1_B 23
#define IN2_B 25
#define IN3_B 27
#define IN4_B 29

// =====================================================
// NON-BLOCKING TIMERS FOR 28BYJ
// =====================================================
unsigned long lastStepTimeA = 0;
unsigned long lastStepTimeB = 0;
const unsigned long stepInterval = 600;

int seqIndexA = 0;
int seqIndexB = 0;

const int seq[8][4] = {
  {1,0,0,0},
  {1,1,0,0},
  {0,1,0,0},
  {0,1,1,0},
  {0,0,1,0},
  {0,0,1,1},
  {0,0,0,1},
  {1,0,0,1}
};

void applyStepA(int i) {
  digitalWrite(IN1_A, seq[i][0]);
  digitalWrite(IN2_A, seq[i][1]);
  digitalWrite(IN3_A, seq[i][2]);
  digitalWrite(IN4_A, seq[i][3]);
}

void applyStepB(int i) {
  digitalWrite(IN1_B, seq[i][0]);
  digitalWrite(IN2_B, seq[i][1]);
  digitalWrite(IN3_B, seq[i][2]);
  digitalWrite(IN4_B, seq[i][3]);
}

// =====================================================
// SETUP
// =====================================================
void setup() {
  Serial.begin(115200);

  // TMC2209
  pinMode(EN_PIN, OUTPUT);
  digitalWrite(EN_PIN, LOW);

  UART_PORT.begin(115200);
  driver.begin();
  driver.toff(5);
  driver.rms_current(900);
  driver.microsteps(8);
  driver.pwm_autoscale(true);

  tmcStepper.setMaxSpeed(8000);
  tmcStepper.setAcceleration(2000);
  tmcStepper.moveTo(10000);

  // Servo
  clawServo.attach(SERVO_PIN);

  // 28BYJ pins
  pinMode(IN1_A,OUTPUT); pinMode(IN2_A,OUTPUT);
  pinMode(IN3_A,OUTPUT); pinMode(IN4_A,OUTPUT);

  pinMode(IN1_B,OUTPUT); pinMode(IN2_B,OUTPUT);
  pinMode(IN3_B,OUTPUT); pinMode(IN4_B,OUTPUT);

  Serial.println("System Ready.");
}

// =====================================================
// MAIN LOOP
// =====================================================
void loop() {

  // =====================================================
  // 1) LINEAR ACTUATOR BACKGROUND (TMC2209)
  // =====================================================
  tmcStepper.run();
  if (tmcStepper.distanceToGo() == 0) {
    long next = (tmcStepper.targetPosition() > 0) ? -10000 : 10000;
    tmcStepper.moveTo(next);
  }

  // =====================================================
  // 2) READ JOYSTICK
  // =====================================================
  int yRaw = analogRead(JOY_Y);
  int xRaw = analogRead(JOY_X);

  int yDiff = yRaw - 512;
  int xDiff = xRaw - 512;

  // Y-axis (servo + stepper A + actuator speed)
  bool yForward  = (yDiff < -deadZone);
  bool yBackward = (yDiff >  deadZone);

  // X-axis (stepper B)
  bool xForward  = (xDiff < -deadZone);
  bool xBackward = (xDiff >  deadZone);

  unsigned long now = micros();

  // =====================================================
  // 3) 28BYJ STEPPER A (Y-axis)
  // =====================================================
  if (yForward || yBackward) {
    if (now - lastStepTimeA >= stepInterval) {
      lastStepTimeA = now;

      if (yForward)  seqIndexA = (seqIndexA + 1) % 8;
      if (yBackward) seqIndexA = (seqIndexA - 1 + 8) % 8;

      applyStepA(seqIndexA);
    }
  } else {
    applyStepA(0);
  }

  // =====================================================
  // 4) 28BYJ STEPPER B (X-axis)
  // =====================================================
  if (xForward || xBackward) {
    if (now - lastStepTimeB >= stepInterval) {
      lastStepTimeB = now;

      if (xForward)  seqIndexB = (seqIndexB + 1) % 8;
      if (xBackward) seqIndexB = (seqIndexB - 1 + 8) % 8;

      applyStepB(seqIndexB);
    }
  } else {
    applyStepB(0);
  }

  // =====================================================
  // 5) SERVO (Y-axis only)
  // =====================================================
  if (yForward) {
    servoPos++;
    if (servoPos > 180) servoPos = 180;
  }
  else if (yBackward) {
    servoPos--;
    if (servoPos < 0) servoPos = 0;
  }

  clawServo.write(servoPos);
>>>>>>> Stashed changes
}
