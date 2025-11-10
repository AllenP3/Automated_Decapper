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
}
