#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

/* =========================================
   LINEAR ACTUATOR (TMC2209)
   ========================================= */

#define PIN_LIN_LIMIT   2  // Limit switch (active LOW)
#define PIN_LIN_EN      10
#define PIN_LIN_STEP    11
#define PIN_LIN_DIR     12


/* =========================================
   RAIL STEPPER (28BYJ-48 / ULN2003)
   ========================================= */


#define PIN_RAIL_IN1    31
#define PIN_RAIL_IN2    33
#define PIN_RAIL_IN3    35
#define PIN_RAIL_IN4    37

#define PIN_RAIL_LIMIT  41   // same switch (rear)


/* =========================================
   CLAW STEPPER (28BYJ-48 / ULN2003)
   ========================================= */

#define PIN_CLAW_IN1    23
#define PIN_CLAW_IN2    25
#define PIN_CLAW_IN3    27
#define PIN_CLAW_IN4    29

/* =========================================
   SERVO CLAW (MG996R)
   ========================================= */

#define PIN_SERVO_CLAW  39

/* =========================================
   STATUS LEDS
   ========================================= */

#define PIN_LED_SNAP    45   // indicates SNAP routine
#define PIN_LED_SCREW   47   // indicates SCREW routine


/* =========================================
   UI / BUTTONS / JOYSTICK
   ========================================= */

#define PIN_JOY_X        A0
#define PIN_JOY_Y        A1
#define PIN_JOY_SW       22   // joystick push button (ACTIVE HIGH)

#define PIN_BTN_START    PIN_JOY_SW    // use joystick button for START
#define PIN_BTN_STOP     5             // external stop button (ACTIVE LOW)


/* =========================================
   SAFETY INPUTS
   ========================================= */

#define PIN_ESTOP        52    // emergency stop (active LOW)
#define PIN_HALL_SENSOR   7    // hall effect sensor

#endif