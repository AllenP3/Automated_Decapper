#ifndef CONFIG_H
#define CONFIG_H

// ===================== SERVO CLAW =====================
#define SERVO_PIN              6
#define SERVO_OPEN_ANGLE       160
#define SERVO_CLOSE_ANGLE      60
#define SERVO_STRONG_CLOSE_ANGLE 40
#define SERVO_INITIAL_ANGLE    SERVO_OPEN_ANGLE

#define SERVO_STEP_INTERVAL_MS 10
#define SERVO_STEP_SIZE        2


// ===================== LINEAR ACTUATOR =====================
#define LIN_STEP_PIN        11
#define LIN_DIR_PIN         12
#define LIN_EN_PIN          10

#define LIN_UART_BAUD       115200
#define LIN_R_SENSE         0.11
#define LIN_DRIVER_ADDRESS  0b00
#define LIN_RMS_CURRENT     900
#define LIN_MICROSTEPS      8

#define LIN_MAX_SPEED       10000
#define LIN_ACCEL           2000     // <-- FIX  
#define LIN_HOMING_SPEED    800
#define LIN_BACKOFF_STEPS   200

#define LIN_MIN_POS         0
#define LIN_MAX_POS         5000

#define LIN_MM_PER_STEP     0.01f     // <-- FIX YOU CAN ADJUST
#define LIN_UNSCREW_UP_MMPS 1.5f      // <-- FIX small slow lift speed


// ===================== RAIL STEPPER =====================
#define RAIL_IN1 22
#define RAIL_IN2 23
#define RAIL_IN3 24
#define RAIL_IN4 25
#define RAIL_HOME_PIN  26
#define RAIL_STEP_DELAY_US 1200

#define RAIL_LIMIT_ACTIVE_LOW true   // <-- FIX  
#define RAIL_MAX_TRAVEL_STEPS 3000   // <-- FIX adjust to your travel


// ===================== CLAW ROTATION STEPPER =====================
#define CLAW_IN1 30
#define CLAW_IN2 31
#define CLAW_IN3 32
#define CLAW_IN4 33

#define CLAW_HALL_PIN 34
#define CLAW_DEGREES_PER_STEP 0.703125
#define CLAW_STEP_DELAY_US 1200

#define CLAW_TWIST_BREAK_DEGREES 15   // <-- FIX


// ===================== UI / PANEL =====================
#define BTN_START_PIN   40
#define BTN_STOP_PIN    41
#define MODE_SWITCH_A   42
#define MODE_SWITCH_B   43
#define FUNC_SWITCH     44

#define UI_BLINK_INTERVAL_MS 500

// Jog speed fixes
#define JOG_CLAW_SPEED_STEPS    4
#define JOG_LINEAR_SPEED_STEPS  8


// Additional mode for calibration
#define MODE_CALIBRATE 5   // <-- FIX


#endif
