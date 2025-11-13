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
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ---------- Step driver ----------
void stepSeq(int p[4], int idx){
  for (int i=0;i<4;i++) digitalWrite(p[i], seq[idx][i]);
}
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// =========== LCD SETUP ===========
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C address 0x27, 16x2 display

// =========== PIN DEFINITIONS ===========
#define JOY_X_PIN   A0
#define JOY_Y_PIN   A1   // used in JOG mode
#define JOY_SW_PIN  4    // joystick button (to GND, INPUT_PULLUP)

#define BTN_START   2    // START button (to GND, INPUT_PULLUP)
#define BTN_STOP    3    // STOP  button (to GND, INPUT_PULLUP)

// =========== MODES ===========
enum Mode {
  MODE_SNAP = 0,
  MODE_SCREW = 1,
  MODE_HOME = 2,
  MODE_JOG  = 3,
  MODE_INFO = 4
};

Mode currentMode = MODE_SNAP;
bool isRunning   = false;
bool needsHoming = false;   // true = must home before next SNAP/SCREW
long snapCount   = 0;       // successful SNAP cycles
long screwCount  = 0;       // successful SCREW cycles

// simple debounce for START
unsigned long lastButtonTime = 0;
const unsigned long debounceMs = 200;

// ---------- LCD helpers ----------
void showScreen(const char* line1, const char* line2) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void showIdleScreen() {
  const char* modeStr;
  switch (currentMode) {
    case MODE_SNAP:  modeStr = "SNAP";  break;
    case MODE_SCREW: modeStr = "SCREW"; break;
    case MODE_HOME:  modeStr = "HOME";  break;
    case MODE_JOG:   modeStr = "JOG";   break;
    case MODE_INFO:  modeStr = "INFO";  break;
    default:         modeStr = "???";   break;
  }

  char line1[17];

  // Show current mode + a relevant count
  if (currentMode == MODE_SNAP) {
    snprintf(line1, sizeof(line1), "SNAP C:%3ld    ", snapCount);
  } else if (currentMode == MODE_SCREW) {
    snprintf(line1, sizeof(line1), "SCREW C:%3ld   ", screwCount);
  } else if (currentMode == MODE_INFO) {
    long total = snapCount + screwCount;
    snprintf(line1, sizeof(line1), "INFO T:%3ld    ", total);
  } else {
    // HOME or JOG
    snprintf(line1, sizeof(line1), "M:%-4s         ", modeStr);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  if (needsHoming) {
    lcd.print("Home required   ");
  } else {
    lcd.print("START=D2 STOP=D3");
  }
}

// ---------- joystick: left/right to change mode ----------
void handleJoystickModeSelect() {
  int xVal = analogRead(JOY_X_PIN);

  static unsigned long lastJoyMove = 0;
  const unsigned long joyMoveDelay = 250;

  unsigned long now = millis();
  if (now - lastJoyMove < joyMoveDelay) return;

  if (xVal < 350) {
    // move LEFT through modes
    if (currentMode == MODE_SNAP) currentMode = MODE_INFO;
    else currentMode = (Mode)((int)currentMode - 1);
    lastJoyMove = now;
    showIdleScreen();
  } 
  else if (xVal > 700) {
    // move RIGHT through modes
    if (currentMode == MODE_INFO) currentMode = MODE_SNAP;
    else currentMode = (Mode)((int)currentMode + 1);
    lastJoyMove = now;
    showIdleScreen();
  }
}

// ---------- button press with debounce (for START) ----------
bool buttonPressed(int pin) {
  if (digitalRead(pin) == LOW) {
    unsigned long now = millis();
    if (now - lastButtonTime > debounceMs) {
      lastButtonTime = now;
      return true;
    }
  }
  return false;
}

// ============ PHASE HELPER WITH STOP + SPINNER ============
// Returns false if STOP was pressed, true if phase completed.
bool runPhase(const char* line1, const char* baseText, unsigned long durationMs) {
  const char spinner[4] = {'|', '/', '-', '\\'};
  int spinIndex = 0;

  unsigned long start = millis();

  while (millis() - start < durationMs) {
    // check STOP at high rate
    if (digitalRead(BTN_STOP) == LOW) {
      needsHoming = true; // machine state uncertain -> homing required
      showScreen("Stopped        ", "STOP pressed    ");
      Serial.println("STOP pressed during phase.");
      delay(800);
      return false;
    }

    char line2[17];
    snprintf(line2, sizeof(line2), "%-12s %c", baseText, spinner[spinIndex]);
    spinIndex = (spinIndex + 1) % 4;

    showScreen(line1, line2);

    delay(150);  // spinner speed
  }

  return true;
}

// ===================== SIMULATED ACTIONS =====================

void runSnapCycle() {
  isRunning = true;
  Serial.println("Starting SNAP cycle...");

  if (!runPhase("Running SNAP   ", "Opening     ", 2500)) {
    isRunning = false;
    showIdleScreen();
    return;
  }

  if (!runPhase("Homing SNAP    ", "Returning   ", 2500)) {
    isRunning = false;
    showIdleScreen();
    return;
  }

  // full cycle finished including homing => no extra homing needed
  needsHoming = false;
  snapCount++;  // successful SNAP

  showScreen("SNAP Done!      ", "Process finished");
  Serial.println("SNAP finished.");
  delay(1200);

  isRunning = false;
  showIdleScreen();
}

void runScrewCycle() {
  isRunning = true;
  Serial.println("Starting SCREW cycle...");

  if (!runPhase("Running SCREW  ", "Opening     ", 2500)) {
    isRunning = false;
    showIdleScreen();
    return;
  }

  if (!runPhase("Homing SCREW   ", "Returning   ", 2500)) {
    isRunning = false;
    showIdleScreen();
    return;
  }

  needsHoming = false;
  screwCount++;  // successful SCREW

  showScreen("SCREW Done!     ", "Process finished");
  Serial.println("SCREW finished.");
  delay(1200);

  isRunning = false;
  showIdleScreen();
}

void runHomeOnly() {
  isRunning = true;
  Serial.println("Starting HOME sequence...");

  if (!runPhase("Homing system  ", "Please wait ", 2500)) {
    // STOP during home → still not homed
    isRunning = false;
    showIdleScreen();
    return;
  }

  // Home finished successfully → clear homing requirement
  needsHoming = false;

  showScreen("Home OK         ", "System ready  ");
  Serial.println("Home finished.");
  delay(1200);

  isRunning = false;
  showIdleScreen();
}

// ===================== JOG MODE =====================
// Manual control idea:
//  - X left/right  -> rotation -/+
//  - Y up/down     -> fine -/+
// Currently just displays direction; you can add motor steps later.
void runJogMode() {
  isRunning = true;
  Serial.println("Entering JOG mode...");
  showScreen("JOG mode        ", "STOP/START=exit");

  while (true) {
    // Exit JOG on STOP or START
    if (digitalRead(BTN_STOP) == LOW || digitalRead(BTN_START) == LOW) {
      // After JOG, require homing before automatic cycles
      needsHoming = true;
      Serial.println("Leaving JOG mode. Homing required.");
      showScreen("Exit JOG        ", "Home required  ");
      delay(800);
      break;
    }

    int xVal = analogRead(JOY_X_PIN);
    int yVal = analogRead(JOY_Y_PIN);

    const int centerLow  = 400;
    const int centerHigh = 600;

    const char* line2 = "Stick idle     ";

    if (xVal < centerLow) {
      line2 = "Rot - (left)   ";
      // TODO: add real motor step for rotation negative
    } else if (xVal > centerHigh) {
      line2 = "Rot + (right)  ";
      // TODO: add real motor step for rotation positive
    } else if (yVal < centerLow) {
      line2 = "Fine + (up)    ";
      // TODO: add real motor step for fine + direction
    } else if (yVal > centerHigh) {
      line2 = "Fine - (down)  ";
      // TODO: add real motor step for fine - direction
    }

    showScreen("JOG mode        ", line2);
    delay(150);
  }

  isRunning = false;
  showIdleScreen();
}

// ===================== INFO MODE =====================
// Show total and per-type counts
void runInfoMode() {
  isRunning = true;
  long total = snapCount + screwCount;

  char line1[17];
  char line2[17];

  // Example:
  // Tot: 12 SNAP:  7
  // SCREW:  5
  snprintf(line1, sizeof(line1), "Tot:%3ld SNAP:%3ld", total, snapCount);
  snprintf(line2, sizeof(line2), "SCREW:%3ld       ", screwCount);

  showScreen(line1, line2);
  Serial.print("INFO -> Tot: "); Serial.print(total);
  Serial.print(" SNAP: "); Serial.print(snapCount);
  Serial.print(" SCREW: "); Serial.println(screwCount);

  delay(2500);

  isRunning = false;
  showIdleScreen();
}

// ===================== SETUP =====================
void setup() {
  Serial.begin(9600);

  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_STOP, INPUT_PULLUP);

  lcd.init();        
  lcd.backlight();

  showScreen("Decapping Panel ", "Initializing... ");
  delay(1200);
  showIdleScreen();

  Serial.println("UI ready.");
}

// ===================== LOOP =====================
void loop() {
  if (!isRunning) {
    handleJoystickModeSelect();
  }

  // START: only when idle
  if (!isRunning && buttonPressed(BTN_START)) {
    // Block SNAP/SCREW when homing required, but allow HOME, JOG, INFO
    if (needsHoming &&
        currentMode != MODE_HOME &&
        currentMode != MODE_JOG  &&
        currentMode != MODE_INFO) {

      showScreen("Homing required ", "Select HOME/JOG ");
      Serial.println("Start blocked: homing required.");
      delay(1200);
      showIdleScreen();
    } else {
      switch (currentMode) {
        case MODE_SNAP:  runSnapCycle();  break;
        case MODE_SCREW: runScrewCycle(); break;
        case MODE_HOME:  runHomeOnly();   break;
        case MODE_JOG:   runJogMode();    break;
        case MODE_INFO:  runInfoMode();   break;
      }
    }
  }

  // no big delays here – STOP handled inside runPhase() and runJogMode()
}
