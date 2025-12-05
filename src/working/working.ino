#include <Arduino.h>
#include <Servo.h>
#include <U8g2lib.h>
#include <Wire.h>
#include "Pins.h"

// Crew cap opening sequence:
// 1) Linear down 5.0 cm
// 2) Close claw
// 3) Rotate claw ~230 deg while lifting linear 1 cm
// 4) Lift linear another 2 cm
// 5) Rotate claw back and move rail back farther
// 6) Linear down another 5 cm
// 7) Open claw
// 8) Return rail and linear to start
//
// Tune the constants and direction signs to match your mechanics.

// Linear actuator (step/dir with enable)
const long LIN_STEPS_PER_MM = 600;
const float LIN_DROP1_MM = 52.0f;  // 5.0 cm first drop
const float LIN_LIFT_MM = 10.0f;   // 1 cm lift during rotate
const float LIN_PRE_ROTATE_UP_MM = 30.0f; // 2 cm lift before rotating back
const float LIN_DROP2_MM = 50.0f;  // 5 cm second drop
const long LIN_DROP1_STEPS = (long)(LIN_DROP1_MM * LIN_STEPS_PER_MM + 0.5f);
const long LIN_LIFT_STEPS = (long)(LIN_LIFT_MM * LIN_STEPS_PER_MM + 0.5f);
const long LIN_PRE_ROTATE_UP_STEPS = (long)(LIN_PRE_ROTATE_UP_MM * LIN_STEPS_PER_MM + 0.5f);
const long LIN_DROP2_STEPS = (long)(LIN_DROP2_MM * LIN_STEPS_PER_MM + 0.5f);
const long LIN_RETURN_STEPS = LIN_DROP1_STEPS - LIN_LIFT_STEPS - LIN_PRE_ROTATE_UP_STEPS + LIN_DROP2_STEPS;
const int LIN_PULSE_US = 50;

// Claw stepper (28BYJ-48 on ULN2003)
// Full revolution ~= 4096 half-steps => ~11.38 steps/degree
const float CLAW_STEPS_PER_DEG = 4096.0f / 360.0f;
const float CLAW_ROTATE_DEG = 340.0f;
const int CLAW_ROTATE_STEPS = (int)(CLAW_ROTATE_DEG * CLAW_STEPS_PER_DEG + 0.5f);
const int CLAW_DELAY_US = 900;

// Rail travel
const int RAIL_BACK_STEPS = 2500;         // base rail back distance
const int RAIL_EXTRA_BACK_STEPS = 1000;   // additional back travel before final drop/open
const int RAIL_DELAY_US = 1100;

// Servo claw angles
const int SERVO_OPEN = 130;
const int SERVO_CLOSE = 30;

// ==========================
// SNAP ROUTINE PARAMETERS
// ==========================

// --- Linear actuator ---
long SNAP_LIN_STEPS_PER_MM = 600;   // steps per millimeter
float SNAP_LIN_DOWN_MM     = 39.0f; // how far to go down before snapping
float SNAP_LIN_LIFT_MM     = 20.0f; // lift during rail move
int   SNAP_LIN_PULSE_US    = 200;   // linear pulse delay

// Computed step values (call updateSnapParams() in setup)
long SNAP_LIN_DOWN_STEPS   = 0;
long SNAP_LIN_LIFT_STEPS   = 0;

// --- Claw rotation (28BYJ-48) ---
float SNAP_CLAW_STEPS_PER_DEG = 4096.0f / 360.0f; 
float SNAP_CLAW_ROTATE_DEG    = 90.0f;           // rotation for opening cap
int   SNAP_CLAW_DELAY_US      = 1200;            // delay per step

// Computed
int SNAP_CLAW_ROTATE_STEPS    = 0;

// --- Rail travel ---
int SNAP_RAIL_BACK_STEPS = 2500;  // how far rail moves backward
int SNAP_RAIL_DELAY_US   = 1500;

// --- Servo claw angles ---
int SNAP_SERVO_OPEN  = 100;
int SNAP_SERVO_CLOSE = 45;

// ULN2003 half-step sequence
const uint8_t HALF_STEP_SEQ[8][4] = {
    {1, 0, 0, 0}, {1, 1, 0, 0}, {0, 1, 0, 0}, {0, 1, 1, 0},
    {0, 0, 1, 0}, {0, 0, 1, 1}, {0, 0, 0, 1}, {1, 0, 0, 1}
};

Servo clawServo;

const int RAIL_PINS[4] = {PIN_RAIL_IN1, PIN_RAIL_IN2, PIN_RAIL_IN3, PIN_RAIL_IN4};
const int CLAW_PINS[4] = {PIN_CLAW_IN1, PIN_CLAW_IN2, PIN_CLAW_IN3, PIN_CLAW_IN4};

uint8_t clawSeqIdx = 0;
uint8_t railSeqIdx = 0;

void stepLinear(long steps, bool dirHigh);
void spinStepper(const int pins[4], int steps, int dir, uint8_t &seqIdx, int delayUs);
void railMove(int steps, int dir);
void railMoveReturn(int steps);
void clawRotate(int steps, int dir);
void clawRotateWithLinearLift(int steps, int dir, long linLiftSteps);
void handleSerial();
void screwRoutine();
void snapRoutine();
void updateSnapParams();
void railMoveWithLinearLift(int steps, int dir, long linLiftSteps);
void drawMenu();
void handleJoystickMenu();
void executeMenuAction(int optionIdx);
void showStatus(const char *line1, const char *line2 = nullptr);
void screwSnapRoutine();
void setSnapLed(bool on);
void setScrewLed(bool on);
void setBothLeds(bool on);


enum RunState {
    RUN_IDLE,
    RUN_BUSY
};

RunState runState = RUN_IDLE;

enum MenuOption {
    MENU_SNAP = 0,
    MENU_SCREW = 1,
    MENU_SCREW_SNAP = 2,
    MENU_HOME = 3,
    MENU_COUNT = 4
};

// Minimal RAM buffer display. Adjust constructor if your screen differs.
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R2, U8X8_PIN_NONE);

MenuOption currentMenu = MENU_SNAP;
unsigned long lastJoyMoveMs = 0;
const unsigned long JOY_MOVE_DEBOUNCE_MS = 180;
const int JOY_LOW = 350;
const int JOY_HIGH = 700;

void updateSnapParams() {
    SNAP_LIN_DOWN_STEPS = (long)(SNAP_LIN_DOWN_MM * SNAP_LIN_STEPS_PER_MM + 0.5f);
    SNAP_LIN_LIFT_STEPS = (long)(SNAP_LIN_LIFT_MM * SNAP_LIN_STEPS_PER_MM + 0.5f);
    SNAP_CLAW_ROTATE_STEPS = (int)(SNAP_CLAW_ROTATE_DEG * SNAP_CLAW_STEPS_PER_DEG + 0.5f);
}
// =============================
// HOMING ROUTINES
// =============================

// 1) Linear homing: move up for a safe fixed distance
void homeLinear() {
    Serial.println("Homing: Linear actuator UP");

    pinMode(PIN_LIN_LIMIT, INPUT_PULLUP);  // <-- correct pin

    digitalWrite(PIN_LIN_DIR, HIGH); // UP
    delayMicroseconds(50);

    // Move up until switch goes LOW (pressed)
    while (digitalRead(PIN_LIN_LIMIT) == HIGH) {
        digitalWrite(PIN_LIN_STEP, HIGH);
        delayMicroseconds(LIN_PULSE_US);
        digitalWrite(PIN_LIN_STEP, LOW);
        delayMicroseconds(LIN_PULSE_US);
    }

    Serial.println("Linear homing done (switch hit)");
}


// 2) Claw homing using hall sensor
void homeClaw() {
    Serial.println("Homing: Claw (Ã‚Â±90Ã‚Â° search)");

    pinMode(PIN_HALL_SENSOR, INPUT_PULLUP);
    clawSeqIdx = 0;

    const int NINETY_DEG_STEPS = (int)(90.0f * CLAW_STEPS_PER_DEG + 0.5f); // ~1024

    // -------------------------------------
    // 0) Already homed?
    // -------------------------------------
    if (digitalRead(PIN_HALL_SENSOR) == LOW) {
        Serial.println("Claw already homed (hall active at start)");
        return;
    }

    // -------------------------------------
    // 1) Sweep LEFT 90Ã‚Â°
    // -------------------------------------
    Serial.println(" - Sweeping LEFT 90Ã‚Â°");

    for (int s = 0; s < NINETY_DEG_STEPS; s++) {

        // Check hall
        if (digitalRead(PIN_HALL_SENSOR) == LOW) {
            Serial.println("Claw homed (found hall on LEFT sweep)");
            goto release_done;
        }

        // Step LEFT
        clawSeqIdx = (clawSeqIdx + 1) & 0x07; // -1 mod 8
        for (int i = 0; i < 4; i++)
            digitalWrite(CLAW_PINS[i], HALF_STEP_SEQ[clawSeqIdx][i]);

        delayMicroseconds(CLAW_DELAY_US);
    }

    // -------------------------------------
    // 2) Sweep RIGHT 180Ã‚Â° total
    //    (Back through center + 90Ã‚Â° right)
    // -------------------------------------
    Serial.println(" - Sweeping RIGHT 180Ã‚Â°");

    for (int s = 0; s < NINETY_DEG_STEPS * 2; s++) {

        // Check hall
        if (digitalRead(PIN_HALL_SENSOR) == LOW) {
            Serial.println("Claw homed (found hall on RIGHT sweep)");
            goto release_done;
        }

        // Step RIGHT
        clawSeqIdx = (clawSeqIdx + 7) & 0x07; // +1 mod 8
        for (int i = 0; i < 4; i++)
            digitalWrite(CLAW_PINS[i], HALF_STEP_SEQ[clawSeqIdx][i]);

        delayMicroseconds(CLAW_DELAY_US);
    }

    Serial.println("WARNING: Hall not found in Ã‚Â±90Ã‚Â° range!");

release_done:
    // Release coils
    for (int i = 0; i < 4; i++)
        digitalWrite(CLAW_PINS[i], LOW);

    Serial.println("Claw homing sequence complete.");
}



// 3) Rail homing using limit switch (active LOW)
void homeRail() {
    Serial.println("Homing: Rail backwards until limit hit");

    pinMode(PIN_RAIL_LIMIT, INPUT_PULLUP);
    railSeqIdx = 0;

    // Move backward until switch goes LOW
    while (digitalRead(PIN_RAIL_LIMIT) == HIGH) {  // HIGH means NOT pressed
        railSeqIdx = (railSeqIdx + 1) & 0x07;
        for (int i = 0; i < 4; i++) {
            digitalWrite(RAIL_PINS[i], HALF_STEP_SEQ[railSeqIdx][i]);
        }
        delayMicroseconds(RAIL_DELAY_US);
    }

    // release coils
    for (int i = 0; i < 4; i++) digitalWrite(RAIL_PINS[i], LOW);

    Serial.println("Rail homing done (limit hit)");
}
void homeAll() {
    Serial.println("=========================");
    Serial.println(" BEGIN HOMING SEQUENCE");
    Serial.println("=========================");
    setBothLeds(true);

    // 1) Home claw first (rotation zero)
    Serial.println("Step 1: Homing Claw...");
    homeClaw();
    delay(300);

    // 2) Home rail (backwards to limit switch)
    Serial.println("Step 2: Homing Rail...");
    homeRail();
    delay(300);

    // 3) Home linear actuator (move UP until switch)
    Serial.println("Step 3: Homing Linear...");
    homeLinear();
    delay(300);

    Serial.println("=========================");
    Serial.println(" HOMING COMPLETE");
    Serial.println("=========================");
    setBothLeds(false);
}

void screwRoutine() {

    Serial.println("====================================");
    Serial.println("       BEGIN SCREW CAP ROUTINE");
    Serial.println("====================================");
    setScrewLed(true);

    // ============================================================
    // 0) Move rail forward to tube (4100 steps)
    // ============================================================
    Serial.println("0) Rail forward to tube (4100)");
    railMove(4200, -1);   // +1 = forward (adjust if needed)
    delay(200);

    // ============================================================
    // 1) Linear down 6cm
    // ============================================================
    Serial.println("1) Linear DOWN 6cm");
    stepLinear(LIN_DROP1_STEPS, false);
    delay(200);

    // ============================================================
    // 2) Close claw around cap
    // ============================================================
    Serial.println("2) Closing claw");
    clawServo.write(SERVO_CLOSE);
    delay(300);

    // ============================================================
    // 3) Break torque (strong tight cap)
    // ============================================================
    Serial.println("3) Break torque");
    const int TORQUE_BREAK_DEG = 20;
    const int TORQUE_BREAK_STEPS = (int)(TORQUE_BREAK_DEG * CLAW_STEPS_PER_DEG);

    // small clockwise
    clawRotate(TORQUE_BREAK_STEPS, -1);
    delay(200);

    // larger counter-clockwise
    clawRotate(TORQUE_BREAK_STEPS * 2, +1);
    delay(200);

    // ============================================================
    // 4) Main unscrewing rotation while lifting 1cm
    // ============================================================
    Serial.println("4) UNSCREW rotation + 1cm lift");
    clawRotateWithLinearLift(CLAW_ROTATE_STEPS, +1, LIN_LIFT_STEPS);
    delay(200);

    // ============================================================
    // 5) Extra 2cm lift to clean threads
    // ============================================================
    Serial.println("5) Extra lift (2 cm)");
    stepLinear(LIN_PRE_ROTATE_UP_STEPS, true);
    delay(300);

    // ============================================================
    // 6) RETURN TO ORIGINAL ANGLE Ã¢â‚¬â€ exact reverse rotation
    //     (no stabilizing rotation anymore)
    // ============================================================
    Serial.println("6) Returning claw to original angle");
    clawRotate(CLAW_ROTATE_STEPS, -1);  // exact opposite of step 4
    delay(200);

    // ============================================================
    // 7) Move rail backwards to pull cap away
    // ============================================================
    Serial.println("7) Rail BACK remove cap");
    railMove(RAIL_BACK_STEPS + RAIL_EXTRA_BACK_STEPS, +1);
    delay(200);

    // ============================================================
    // 8) Linear down another 5cm for drop-off
    // ============================================================
    Serial.println("8) Linear DOWN 5cm (drop-off)");
    stepLinear(LIN_DROP2_STEPS, false);
    delay(200);

    // ============================================================
    // 9) Open claw (release cap)
    // ============================================================
    Serial.println("9) Open claw");
    clawServo.write(SERVO_OPEN);
    delay(200);

    // ============================================================
    // 10) Rail return to start
    // ============================================================
    Serial.println("10) Rail RETURN");
    railMoveReturn(RAIL_BACK_STEPS);
    delay(200);

    // ============================================================
    // 11) Linear return to start
    // ============================================================
    Serial.println("11) Linear RETURN");
    // stepLinear(LIN_RETURN_STEPS, true);
    delay(300);

    Serial.println("====================================");
    Serial.println("         SCREW ROUTINE DONE");
    Serial.println("====================================");
    setScrewLed(false);
}

// Hybrid flow: run the screw routine through the unscrew + claw return,
// then instead of moving the rail back just open the claw in place.
void screwSnapRoutine() {

    Serial.println("====================================");
    Serial.println("     BEGIN SCREW/SNAP ROUTINE");
    Serial.println("====================================");
    setScrewLed(true);

    // 0) Rail forward to tube (4100 steps)
    Serial.println("0) Rail forward to tube (4100)");
    railMove(4100, -1);
    delay(300);

    // 1) Linear down 6cm
    Serial.println("1) Linear DOWN 6cm");
    stepLinear(LIN_DROP1_STEPS, false);
    delay(300);

    // 2) Close claw around cap
    Serial.println("2) Closing claw");
    clawServo.write(SERVO_CLOSE);
    delay(700);

    // 3) Break torque
    Serial.println("3) Break torque");
    const int TORQUE_BREAK_DEG = 20;
    const int TORQUE_BREAK_STEPS = (int)(TORQUE_BREAK_DEG * CLAW_STEPS_PER_DEG);
    clawRotate(TORQUE_BREAK_STEPS, -1);          // small clockwise
    delay(200);
    clawRotate(TORQUE_BREAK_STEPS * 2, +1);      // larger counter-clockwise
    delay(200);

    // 4) Main unscrewing rotation while lifting 1cm
    Serial.println("4) UNSCREW rotation + 1cm lift");
    clawRotateWithLinearLift(CLAW_ROTATE_STEPS, +1, LIN_LIFT_STEPS);
    delay(400);

    // 5) Extra 2cm lift to clean threads
   // Serial.println("5) Extra lift (2 cm)");
    //stepLinear(LIN_PRE_ROTATE_UP_STEPS, true);
    //delay(300);

    // 6) Return claw to original angle
    //Serial.println("6) Returning claw to original angle");
    //clawRotate(CLAW_ROTATE_STEPS, -1);
    //delay(400);

    // 7) Instead of rail move back, just open the claw right here
    Serial.println("7) Open claw (no rail move back)");
    clawServo.write(SERVO_OPEN);
    delay(800);

    // 5) Extra 2cm lift to clean threads
    Serial.println("5) Extra lift (2 cm)");
    stepLinear(LIN_PRE_ROTATE_UP_STEPS, true);
    delay(300);

    Serial.println("====================================");
    Serial.println("    SCREW/SNAP ROUTINE COMPLETE");
    Serial.println("====================================");
    setScrewLed(false);
}

void screwExtra() {

    Serial.println("====================================");
    Serial.println("     BEGIN SCREW-EXTRA ROUTINE");
    Serial.println("====================================");
    setScrewLed(true);

    // ============================================================
    // 0) Move rail forward to tube (4100 steps)
    // ============================================================
    Serial.println("0) Rail forward (4100)");
    railMove(4100, -1);
    delay(300);

    // ============================================================
    // 1) Linear down 5cm to grip height
    // ============================================================
    Serial.println("1) Linear DOWN 5cm");
    stepLinear(LIN_DROP1_STEPS, false);
    delay(300);

    // ============================================================
    // 2) Close claw to grip cap
    // ============================================================
    Serial.println("2) Close claw");
    clawServo.write(SERVO_CLOSE);
    delay(700);

    // ============================================================
    // 3) Break torque like real screw caps
    // ============================================================
    Serial.println("3) Break torque");
    const int TORQUE_BREAK_DEG = 20;
    const int TORQUE_BREAK_STEPS = (int)(TORQUE_BREAK_DEG * CLAW_STEPS_PER_DEG);

    // small clockwise
    clawRotate(TORQUE_BREAK_STEPS, -1);
    delay(200);

    // larger counter-clockwise
    clawRotate(TORQUE_BREAK_STEPS * 2, +1);
    delay(200);

    // ============================================================
    // 4) Main UNSCREW rotation while lifting 1cm
    // ============================================================
    Serial.println("4) UNSCREW rotation + lift 1cm");
    clawRotateWithLinearLift(CLAW_ROTATE_STEPS, +1, LIN_LIFT_STEPS);
    delay(400);

    // ============================================================
    // 5) Lift extra 2 cm to clear threads fully
    // ============================================================
    Serial.println("5) Extra lift (2cm)");
    stepLinear(LIN_PRE_ROTATE_UP_STEPS, true);
    delay(300);

    // ============================================================
    // 6) MOVE CAP UP AWAY FROM THREADS
    // ============================================================
    Serial.println("6) Lift slightly more to fully clear");
    stepLinear(600, true);  // +1mm extra lift
    delay(300);

    // ============================================================
    // 7) COME BACK DOWN to screwing height
    //     Must match the total lift exactly.
    // ============================================================
    Serial.println("7) Move DOWN to screw height");
    long totalLift = LIN_LIFT_STEPS + LIN_PRE_ROTATE_UP_STEPS + 600;
    stepLinear(totalLift, false);  // DOWN
    delay(300);

    // ============================================================
    // 8) SCREW CAP BACK ON:
    //     Full clockwise rotation (reverse of unscrew)
    // ============================================================
    Serial.println("8) SCREW BACK ON (full rotation)");
    clawRotate(CLAW_ROTATE_STEPS, -1);   // clockwise tighten
    delay(400);

    // Final tightening torque
    Serial.println("8b) Apply tightening torque");
    clawRotate((int)(30 * CLAW_STEPS_PER_DEG), -1); // 30Ã‚Â° snug
    delay(300);

    // ============================================================
    // 9) Open claw (release tube)
    // ============================================================
    Serial.println("9) Opening claw");
    clawServo.write(SERVO_OPEN);
    delay(600);

    // ============================================================
    // 10) Rail return
    // ============================================================
    Serial.println("10) Rail return");
    railMoveReturn(RAIL_BACK_STEPS);
    delay(300);

    // ============================================================
    // 11) Linear return
    // ============================================================
    Serial.println("11) Linear return");
    // stepLinear(LIN_RETURN_STEPS, true);
    delay(300);

    Serial.println("====================================");
    Serial.println("   SCREW-EXTRA ROUTINE COMPLETE");
    Serial.println("====================================");
    setScrewLed(false);
}

void railMoveWithLinearLift(int steps, int dir, long linLiftSteps) {
    // Move rail while proportionally stepping linear upward
    int dirStep = (dir > 0) ? 1 : -1;
    double linPerRail = (steps == 0) ? 0.0 : ((double)linLiftSteps / (double)steps);
    double linAccum = 0.0;
    long linDone = 0;

    // Set linear direction to UP (dirHigh = true). Flip if your polarity differs.
    digitalWrite(PIN_LIN_DIR, HIGH);
    delayMicroseconds(50);

    railSeqIdx = 0; // start from known phase

    for (int s = 0; s < steps; s++) {
        railSeqIdx = (railSeqIdx + (dirStep > 0 ? 1 : 7)) & 0x07;
        for (int i = 0; i < 4; i++) {
            digitalWrite(RAIL_PINS[i], HALF_STEP_SEQ[railSeqIdx][i]);
        }

        // accumulate and step linear as needed
        linAccum += linPerRail;
        while (linDone < (long)linAccum && linDone < linLiftSteps) {
            digitalWrite(PIN_LIN_STEP, HIGH);
            delayMicroseconds(LIN_PULSE_US);
            digitalWrite(PIN_LIN_STEP, LOW);
            delayMicroseconds(LIN_PULSE_US);
            linDone++;
        }

        delayMicroseconds(RAIL_DELAY_US);
    }

    // release rail coils
    for (int i = 0; i < 4; i++) digitalWrite(RAIL_PINS[i], LOW);
}

void snapRoutine() {

    Serial.println("====================================");
    Serial.println("         BEGIN SNAP ROUTINE");
    Serial.println("====================================");
    setSnapLed(true);

    // ============================================================
    // 0) Rail forward 4100 steps to reach tube
    // ============================================================
    Serial.println("0) Rail FORWARD 4100");
    railMove(4100, -1);   // -1 = forward (your hardware)
    delay(100);


    // ============================================================
    // 1) Linear down to snapping height
    // ============================================================
    Serial.println("1) Linear DOWN");
    stepLinear(SNAP_LIN_DOWN_STEPS, false);   // false = DOWN
    delay(200);


    // ============================================================
    // 2) Rotate claw -90Ã‚Â° to the snap position
    // ============================================================
    Serial.println("2) Claw rotate -90Ã‚Â°");
    clawRotate(SNAP_CLAW_ROTATE_STEPS, +1);
    delay(200);


    // ============================================================
    // 3) Close claw to grip snap cap
    // ============================================================
    Serial.println("3) Close claw");
    clawServo.write(SNAP_SERVO_CLOSE);
    delay(200);


    // ============================================================
    // 4) Rail BACKWARD again (another 4100 steps) 
    //    while lifting linear by SNAP_LIN_LIFT_STEPS
    // ============================================================
    Serial.println("4) Rail BACKWARD + Linear LIFT");
    railMoveWithLinearLift(4000, +1, SNAP_LIN_LIFT_STEPS);  
    delay(200);


    // ============================================================
    // 5) Open claw to release snap cap
    // ============================================================
    Serial.println("5) Open claw");
    clawServo.write(SNAP_SERVO_OPEN);
    delay(200);


    // ============================================================
    // 6) Rotate claw back +90 deg
    // ============================================================
    Serial.println("6) Claw rotate +90 deg");
    clawRotate(SNAP_CLAW_ROTATE_STEPS, -1);
    delay(200);


    // ============================================================
    // 7) Linear UP (return to start)
    // ============================================================
    Serial.println("7) Linear UP");
    long linUp = SNAP_LIN_DOWN_STEPS - SNAP_LIN_LIFT_STEPS;
    if (linUp < 0) linUp = 0;
    stepLinear(linUp, true);    // true = UP
    delay(200);


    Serial.println("====================================");
    Serial.println("        SNAP ROUTINE COMPLETE");
    Serial.println("====================================");
    setSnapLed(false);
}





void setup() {
    Serial.begin(115200);
    Serial.println("Starting crew cap open sequence");
    updateSnapParams();

    pinMode(PIN_LIN_EN, OUTPUT);
    pinMode(PIN_LIN_STEP, OUTPUT);
    pinMode(PIN_LIN_DIR, OUTPUT);
    digitalWrite(PIN_LIN_EN, LOW); // enable driver (active LOW)

    for (int i = 0; i < 4; i++) {
        pinMode(RAIL_PINS[i], OUTPUT);
        digitalWrite(RAIL_PINS[i], LOW);
        pinMode(CLAW_PINS[i], OUTPUT);
        digitalWrite(CLAW_PINS[i], LOW);
    }

    clawServo.attach(PIN_SERVO_CLAW);
    clawServo.write(SERVO_OPEN);
    delay(500);

    pinMode(PIN_JOY_SW, INPUT);  // joystick button is active HIGH
    pinMode(PIN_LED_SNAP, OUTPUT);
    pinMode(PIN_LED_SCREW, OUTPUT);
    setBothLeds(false);  // default off until a sequence runs

    u8g2.begin();
    drawMenu();

    // Home everything on startup for a known reference.
    homeAll();
}

void loop() {
    handleSerial();  // still allow manual commands at any time
    handleJoystickMenu();
}



void stepLinear(long steps, bool dirHigh) {
    digitalWrite(PIN_LIN_DIR, dirHigh ? HIGH : LOW);
    delayMicroseconds(50);
    for (long i = 0; i < steps; i++) {
        digitalWrite(PIN_LIN_STEP, HIGH);
        delayMicroseconds(LIN_PULSE_US);
        digitalWrite(PIN_LIN_STEP, LOW);
        delayMicroseconds(LIN_PULSE_US);
    }

}

void spinStepper(const int pins[4], int steps, int dir, uint8_t &seqIdx, int delayUs) {
    int dirStep = (dir > 0) ? 1 : -1;
    for (int s = 0; s < steps; s++) {
        seqIdx = (seqIdx + (dirStep > 0 ? 1 : 7)) & 0x07;
        for (int i = 0; i < 4; i++) {
            digitalWrite(pins[i], HALF_STEP_SEQ[seqIdx][i]);
        }
        delayMicroseconds(delayUs);
    }
    for (int i = 0; i < 4; i++) digitalWrite(pins[i], LOW); // release coils
}

void railMove(int steps, int dir) {
    railSeqIdx = 0;
    spinStepper(RAIL_PINS, steps, dir, railSeqIdx, RAIL_DELAY_US);
}

void railMoveReturn(int steps) {
    railSeqIdx = 0;
    spinStepper(RAIL_PINS, steps, -1, railSeqIdx, RAIL_DELAY_US);
}

void clawRotate(int steps, int dir) {
    // Reverse claw direction by inverting the requested dir
    spinStepper(CLAW_PINS, steps, -dir, clawSeqIdx, CLAW_DELAY_US);
}

void clawRotateWithLinearLift(int steps, int dir, long linLiftSteps) {
    // Rotate claw while proportionally lifting the linear actuator.
    int dirStep = (dir > 0) ? -1 : 1; // reversed claw direction
    double linPerClaw = (steps == 0) ? 0.0 : ((double)linLiftSteps / (double)steps);
    double linAccum = 0.0;
    long linDone = 0;

    digitalWrite(PIN_LIN_DIR, HIGH); // set linear to up
    delayMicroseconds(50);

    for (int s = 0; s < steps; s++) {
        clawSeqIdx = (clawSeqIdx + (dirStep > 0 ? 1 : 7)) & 0x07;
        for (int i = 0; i < 4; i++) {
            digitalWrite(CLAW_PINS[i], HALF_STEP_SEQ[clawSeqIdx][i]);
        }

        linAccum += linPerClaw;
        while (linDone < (long)linAccum && linDone < linLiftSteps) {
            digitalWrite(PIN_LIN_STEP, HIGH);
            delayMicroseconds(LIN_PULSE_US);
            digitalWrite(PIN_LIN_STEP, LOW);
            delayMicroseconds(LIN_PULSE_US);
            linDone++;
        }

        delayMicroseconds(CLAW_DELAY_US);
    }

    for (int i = 0; i < 4; i++) digitalWrite(CLAW_PINS[i], LOW);
}

// ---------------------------------------------
// Serial command handler
// Commands (newline terminated, case sensitive):
//   LIN <steps>   (positive = up, negative = down)
//   RAIL <steps>  (positive dir = +1, negative = -1)
//   CLAW <deg>    (degrees, positive/negative)
//   SERVO <angle> (0-180)
// ---------------------------------------------
void handleSerial() {
    if (!Serial.available()) return;

    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.length() == 0) return;

    char type[8];
    long val = 0;
    if (sscanf(cmd.c_str(), "%7s %ld", type, &val) != 2) {
        Serial.println("Bad cmd. Use: LIN steps | RAIL steps | CLAW deg | SERVO angle");
        return;
    }

    if (strcmp(type, "LIN") == 0) {
        stepLinear(labs(val), val > 0 ? true : false); // DIR HIGH = up, LOW = down
        Serial.println("OK LIN");
    } else if (strcmp(type, "RAIL") == 0) {
        railMove((int)labs(val), val > 0 ? +1 : -1);
        Serial.println("OK RAIL");
    } else if (strcmp(type, "CLAW") == 0) {
        int steps = (int)(fabs(val) * CLAW_STEPS_PER_DEG + 0.5f);
        clawRotate(steps, val > 0 ? +1 : -1);
        Serial.println("OK CLAW");
    } else if (strcmp(type, "SERVO") == 0) {
        int ang = constrain((int)val, 0, 180);
        clawServo.write(ang);
        Serial.println("OK SERVO");
    } else {
        Serial.println("Unknown type. Use LIN/RAIL/CLAW/SERVO");
    }
}

// ---------------------------------------------
// Minimal joystick + OLED menu (SNAP / SCREW / HOME)
// ---------------------------------------------
void drawMenu() {
    const char *labels[MENU_COUNT] = {"SNAP", "SCREW", "SCREW/SNAP", "HOME"};
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_6x10_mf);
        u8g2.setFontPosTop();
        u8g2.drawStr(2, 0, "Select mode");
        for (int i = 0; i < MENU_COUNT; i++) {
            int y = 16 + i * 14;  // consistent spacing for small screens
            if (i == (int)currentMenu) {
                u8g2.drawStr(2, y, ">");
            }
            u8g2.drawStr(12, y, labels[i]);  // text starts a bit in from the edge
        }
    } while (u8g2.nextPage());
}

void showStatus(const char *line1, const char *line2) {
    u8g2.firstPage();
    do {
        u8g2.setFont(u8g2_font_6x10_mf);
        u8g2.setFontPosTop();
        u8g2.drawStr(2, 12, line1);
        if (line2 != nullptr) {
            u8g2.drawStr(2, 28, line2);
        }
    } while (u8g2.nextPage());
}

void handleJoystickMenu() {
    if (runState != RUN_IDLE) {
        return;
    }

    unsigned long now = millis();
    int yVal = analogRead(PIN_JOY_Y);

    if (now - lastJoyMoveMs > JOY_MOVE_DEBOUNCE_MS) {
        if (yVal < JOY_LOW) {
            currentMenu = (MenuOption)((currentMenu + MENU_COUNT - 1) % MENU_COUNT);
            lastJoyMoveMs = now;
            drawMenu();
        } else if (yVal > JOY_HIGH) {
            currentMenu = (MenuOption)((currentMenu + 1) % MENU_COUNT);
            lastJoyMoveMs = now;
            drawMenu();
        }
    }

    static bool lastPressed = false;
    bool pressed = (digitalRead(PIN_JOY_SW) == HIGH);
    if (pressed && !lastPressed) {
        runState = RUN_BUSY;
        if (currentMenu == MENU_SNAP) {
            showStatus("Running SNAP", "");
        } else if (currentMenu == MENU_SCREW) {
            showStatus("Running SCREW", "");
        } else if (currentMenu == MENU_SCREW_SNAP) {
            showStatus("Running SCREW/SNAP", "");
        } else {
            showStatus("Homing all axes", "");
        }
        executeMenuAction((int)currentMenu);
        runState = RUN_IDLE;
        drawMenu();
    }
    lastPressed = pressed;
}

void executeMenuAction(int optionIdx) {
    MenuOption opt = (MenuOption)optionIdx;
    if (opt == MENU_HOME) {
        homeAll();
        return;
    }

    // Home before and after every routine to keep positions safe.
    homeAll();
    if (opt == MENU_SNAP) {
        snapRoutine();
    } else if (opt == MENU_SCREW) {
        screwRoutine();
    } else if (opt == MENU_SCREW_SNAP) {
        screwSnapRoutine();
    }
    homeAll();
}

// LED helpers keep indicators alive even if the screen fails.
void setSnapLed(bool on) {
    digitalWrite(PIN_LED_SNAP, on ? HIGH : LOW);
}

void setScrewLed(bool on) {
    digitalWrite(PIN_LED_SCREW, on ? HIGH : LOW);
}

void setBothLeds(bool on) {
    setSnapLed(on);
    setScrewLed(on);
}