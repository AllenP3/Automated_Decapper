#include "Safety.h"
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"

Safety::Safety() {}

void Safety::begin() {
    pinMode(PIN_BTN_STOP, INPUT_PULLUP);

    // optional lines
    pinMode(PIN_ESTOP, INPUT_PULLUP);
    pinMode(PIN_DOOR_SWITCH, INPUT_PULLUP);
}

void Safety::attachActuators(LinearActuator* lin,
                             RailStepper* rail,
                             ClawStepper* claw,
                             ServoClaw* servo)
{
    linAct = lin;
    railAct = rail;
    clawAct = claw;
    servoAct = servo;
}

bool Safety::stopRequested() const {
    return stopFlag;
}

bool Safety::systemLocked() const {
    return locked;
}

void Safety::emergencyStop() {
    stopFlag = true;
    locked   = true;

    if (linAct)  linAct->stop();
    if (railAct) railAct->moveTo(railAct->homed ? 0 : 0); // freeze
    if (clawAct) clawAct->rotateSteps(0);
    if (servoAct) servoAct->open();
}

void Safety::clearStop() {
    stopFlag = false;
}

void Safety::update() {
    unsigned long now = millis();

    // STOP button pressed
    if (digitalRead(PIN_BTN_STOP) == LOW) {
        if (now - lastStopPress > debounce) {
            emergencyStop();
        }
        lastStopPress = now;
    }

    // Optional: physical emergency stop
    if (digitalRead(PIN_ESTOP) == LOW) {
        emergencyStop();
    }

    // Optional: safety interlock
    if (digitalRead(PIN_DOOR_SWITCH) == LOW) {
        emergencyStop();
    }
}
