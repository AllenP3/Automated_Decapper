#include "LinearActuator.h"

LinearActuator::LinearActuator()
: driver(&uart, LIN_R_SENSE, LIN_DRIVER_ADDRESS),
  stepper(AccelStepper::DRIVER, PIN_LIN_STEP, PIN_LIN_DIR)
{}

void LinearActuator::begin() {

    // Enable pin
    pinMode(PIN_LIN_EN, OUTPUT);
    digitalWrite(PIN_LIN_EN, LOW);  // ENABLE DRIVER

    // Limit switch
    pinMode(PIN_LIN_LIMIT, INPUT_PULLUP);

    // UART begin
    uart.begin(LIN_UART_BAUD);

    // TMC setup
    driver.begin();
    driver.toff(5);
    driver.rms_current(LIN_RMS_CURRENT);
    driver.microsteps(LIN_MICROSTEPS);
    driver.pwm_autoscale(true);

    // Motion settings
    stepper.setMaxSpeed(LIN_MAX_SPEED);
    stepper.setAcceleration(LIN_ACCEL);
}

void LinearActuator::update() {
    stepper.run();
}

void LinearActuator::stop() {
    stepper.stop();
}

void LinearActuator::moveToMM(float mm) {
    if (!homed) return;
    long steps = mm / LIN_MM_PER_STEP;
    stepper.moveTo(steps);
}

void LinearActuator::moveSteps(long s) {
    if (!homed) return;
    stepper.move(s);
}

void LinearActuator::moveToSteps(long s) {
    if (!homed) return;
    stepper.moveTo(s);
}

void LinearActuator::home() {

    // Move upward until limit triggers
    stepper.setSpeed(LIN_HOMING_SPEED);

    while ( (limitActiveLow && digitalRead(PIN_LIN_LIMIT)) ||
            (!limitActiveLow && !digitalRead(PIN_LIN_LIMIT)) )
    {
        stepper.runSpeed();
    }

    // Small retreat
    stepper.move(-LIN_BACKOFF_STEPS);
    while (isBusy()) stepper.run();

    // Reset position
    stepper.setCurrentPosition(0);
    homed = true;
}
