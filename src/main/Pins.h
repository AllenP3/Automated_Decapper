#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// =======================================================
//                   UI / PANEL INPUTS
// =======================================================

// Joystick
#define PIN_JOY_X       A0
#define PIN_JOY_Y       A1
#define PIN_JOY_SW      22    // pull-up, active LOW

// Start & Stop buttons
#define PIN_BTN_START    2    // pull-up, active LOW
#define PIN_BTN_STOP     3    // pull-up, active LOW


// =======================================================
//                   OLED DISPLAY (I2C)
// =======================================================
// SCL -> Arduino Mega Pin 21
// SDA -> Arduino Mega Pin 20



// =======================================================
//                 LINEAR ACTUATOR (TMC2209)
// =======================================================

#define PIN_LIN_STEP     11
#define PIN_LIN_DIR      12
#define PIN_LIN_EN       10

#define PIN_LIN_LIMIT    23   // Limit switch for linear actuator (active LOW)


// =======================================================
//                RAIL STEPPER (28BYJ-48)
//                ULN2003 DRIVER BOARD
// =======================================================

// Rail Stepper (ULN2003)
#define PIN_RAIL_IN1     4
#define PIN_RAIL_IN2     5
#define PIN_RAIL_IN3     6
#define PIN_RAIL_IN4     7

#define PIN_RAIL_LIMIT   24   // Limit switch for rail (active LOW)


// =======================================================
//                 CLAW ROTATION STEPPER (28BYJ-48)
//                 ULN2003 DRIVER BOARD
// =======================================================

#define PIN_CLAW_IN1     26
#define PIN_CLAW_IN2     27
#define PIN_CLAW_IN3     28
#define PIN_CLAW_IN4     29

// Hall effect sensor for claw indexing
#define PIN_CLAW_HALL    25   // Active LOW when magnet present


// =======================================================
//                      SERVO CLAW
// =======================================================

#define PIN_SERVO_CLAW   8    // MG996R PWM output pin



// =======================================================
//                      INDICATORS
// =======================================================

#define PIN_LED_STATUS    30
#define PIN_LED_ERROR     31
#define PIN_LED_RUNNING   32



// =======================================================
//                      SAFETY INPUTS
// =======================================================
// These are optional depending on your system design.
// Add/remove as required.

#define PIN_ESTOP         33   // Emergency stop (active LOW)
#define PIN_DOOR_SWITCH   34   // Door/cover interlock (active LOW)



// =======================================================
//                    POWER MONITORING (optional)
// =======================================================

#define PIN_VOLTAGE_SENSE A2
#define PIN_CURRENT_SENSE A3



#endif
