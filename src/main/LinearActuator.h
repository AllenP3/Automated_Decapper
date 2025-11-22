#ifndef LINEAR_ACTUATOR_H
#define LINEAR_ACTUATOR_H

#include <Arduino.h>
#include <AccelStepper.h>
#include <TMCStepper.h>
#include "Pins.h"
#include "Config.h"

// ---------------------------------------------------
//  LinearActuator Class (TMC2209 + AccelStepper)
// ---------------------------------------------------
//  - Non-blocking step/dir motion
//  - UART TMC2209 driver configuration
//  - Homing with limit switch
//  - mm-based movement
//  - Slow-lift mode for unscrewing
// ---------------------------------------------------

class LinearActuator {
public:
    LinearActuator();

    void begin();
    void update();             // MUST be called every loop
    void stop();

    // Motion
    void moveToMM(float mm);
    void moveSteps(long s);
    void moveToSteps(long s);

    // Homing
    void home();
    bool isHomed() const { return homed; }

    // Status
    bool isBusy() const { return (stepper.distanceToGo() != 0); }

private:
    HardwareSerial &uart = Serial3;
    TMC2209Stepper driver;
    AccelStepper stepper;

    bool homed = false;

    // Polarity (can adjust later if needed)
    bool limitActiveLow = true;
};

#endif
