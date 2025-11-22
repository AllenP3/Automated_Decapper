#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

/* =========================================
   LINEAR ACTUATOR (TMC2209)
   ========================================= */

#define PIN_LIN_LIMIT   41   // Limit switch (active LOW)
#define PIN_LIN_EN      10
#define PIN_LIN_STEP    11
#define PIN_LIN_DIR     12


/* =========================================
   RAIL STEPPER (28BYJ-48 / ULN2003)
   ========================================= */

#define PIN_RAIL_IN1    23
#define PIN_RAIL_IN2    25
#define PIN_RAIL_IN3    27
#define PIN_RAIL_IN4    29

#define PIN_RAIL_LIMIT  41   // Same limit switch (rear)


/* =========================================
   CLAW STEPPER (28BYJ-48 / ULN2003)
   ========================================= */

#define PIN_CLAW_IN1    31
#define PIN_CLAW_IN2    33
#define PIN_CLAW_IN3    35
#define PIN_CLAW_IN4    37


/* =========================================
   SERVO CLAW (MG996R)
   ========================================= */

#define PIN_SERVO_CLAW  39


/* =========================================
   UI / BUTTONS / JOYSTICK
   ========================================= */

#define PIN_JOY_X        A0
#define PIN_JOY_Y        A1
#define PIN_JOY_SW       22   // joystick switch

// #define PIN_BTN_START     2   // start button
#define PIN_BTN_STOP      3   // stop button


/* =========================================
   SAFETY INPUTS
   ========================================= */

#define PIN_ESTOP        52    // emergency stop button (active LOW)
#define PIN_DOOR_SWITCH  5    // door/cover switch (active LOW)


/* =========================================
   OLED (I2C)
   ========================================= */
#define SCL = 21
#define SDA = 20

#endif
