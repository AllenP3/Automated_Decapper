#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ============================================================================
//                              GLOBAL CONFIG
// ============================================================================

// System loop timing
#define SYSTEM_LOOP_HZ        200       // 200 Hz = 5 ms loop tick
#define SYSTEM_LOOP_DT_MS     (1000 / SYSTEM_LOOP_HZ)


// ============================================================================
//                           LIMIT SWITCH POLARITIES
// ============================================================================
//
// These depend on your actual hardware when it arrives.
// - true  = switch is ACTIVE LOW (pulls pin to GND when triggered)
// - false = switch is ACTIVE HIGH (pulls pin HIGH when triggered)
//
// You can change these at any time without touching any other code.
//

// Rail stepper limit switch (home)
static bool RAIL_LIMIT_ACTIVE_LOW = true;

// Claw hall-effect sensor
static bool CLAW_HALL_ACTIVE_LOW  = true;

// Linear actuator limit (top)
static bool LINEAR_LIMIT_ACTIVE_LOW = true;


// ============================================================================
//                         SERVO LIMITS / BEHAVIOR
// ============================================================================
//
// Change these AFTER you test the claw mechanical limits.
//

// Basic movement
static int SERVO_OPEN_ANGLE       = 40;    // wide open
static int SERVO_CLOSE_ANGLE      = 120;   // normal grip
static int SERVO_CLOSE_STRONG     = 140;   // screw-cap extra grip

// Speed of servo easing
static int SERVO_STEP_SPEED       = 4;     // degrees per update tick


// ============================================================================
//                            RAIL STEPPER CONFIG
// ============================================================================

static int RAIL_STEP_DELAY_US     = 1800;  // speed control
static long RAIL_MAX_TRAVEL_STEPS = 6000;  // change after calibration


// ============================================================================
//                          CLAW ROTATION STEPPER CONFIG
// ============================================================================

static int CLAW_STEP_DELAY_US       = 1600;
static float CLAW_STEPS_PER_DEGREE  = 11.38; // 28BYJ-48 approx

// For screw routine
static int CLAW_TWIST_BREAK_DEGREES = 20;    // torque break
static int CLAW_UNSCREW_SPEED_SLOW  = 2000;  // use with linear actuator


// ============================================================================
//                        LINEAR ACTUATOR (TMC2209) CONFIG
// ============================================================================
//
// THIS USES YOUR EXACT WORKING SETTINGS.
//

static float LIN_MM_PER_STEP         = 0.0025;   // adjust after real measurement
static int   LIN_MAX_SPEED           = 10000;
static int   LIN_ACCEL               = 2000;

// unscrewing: constant slow upward motion
static float LIN_UNSCREW_UP_MMPS     = 0.5;       // mm/second


// ============================================================================
//                               SAFETY SETTINGS
// ============================================================================

// Emergency STOP pin behavior
static bool ESTOP_ACTIVE_LOW = true;

// Optional door switch
static bool DOOR_ACTIVE_LOW = true;

// Motor overload protection (software)
static int MAX_SIMULTANEOUS_ACTUATORS = 2;


// ============================================================================
//                           OLED / UI CONFIG
// ============================================================================

static unsigned long UI_PROGRESS_UPDATE_MS = 30;
static unsigned long UI_BLINK_INTERVAL_MS  = 400;


// ============================================================================
//                           ROUTINE TIMING
// ============================================================================

static unsigned long SNAP_OPEN_MS   = 2000;
static unsigned long SNAP_PULL_MS   = 2000;

static unsigned long SCREW_OPEN_MS  = 2000;
static unsigned long SCREW_UNSCREW_BREAK_MS = 500;


// ============================================================================
//                    CALIBRATION DEFAULT SPEEDS (JOG MODE)
// ============================================================================

static int JOG_LINEAR_SPEED_STEPS   = 400;
static int JOG_CLAW_SPEED_STEPS     = 20;
static int JOG_RAIL_SPEED_STEPS     = 30;


// ============================================================================
//                           MOTION SCHEDULING
// ============================================================================
//
// We enforce limited simultaneous motion due to power limits.
// Routines will check these.
//

static bool ALLOW_LINEAR_PLUS_CLAW_SIMULT = true;
static bool ALLOW_SERVO_WITH_ONE_STEPPER  = true;
static bool ALLOW_RAIL_WITH_SERVO         = true;

// Never allow all 4 motors at once
static bool ALLOW_FULL_PARALLEL = false;


#endif
