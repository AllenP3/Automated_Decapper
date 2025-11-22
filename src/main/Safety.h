#ifndef SAFETY_H
#define SAFETY_H

#include <Arduino.h>
#include "Pins.h"

// Forward declarations to stop all motors
class LinearActuator;
class RailStepper;
class ClawStepper;
class ServoClaw;

class Safety {
public:
    Safety();

    void begin();
    void attachActuators(LinearActuator* lin,
                         RailStepper* rail,
                         ClawStepper* claw,
                         ServoClaw* servo);

    void update();                 // call every loop
    void emergencyStop();          // force stop everything
    void clearStop();              // clear safety lock

    bool stopRequested() const;
    bool systemLocked() const;

private:
    bool stopFlag = false;
    bool locked   = false;

    // actuator references
    LinearActuator* linAct = nullptr;
    RailStepper*    railAct = nullptr;
    ClawStepper*    clawAct = nullptr;
    ServoClaw*      servoAct = nullptr;

    // debounce
    unsigned long lastStopPress = 0;
    const unsigned long debounce = 120;
};

#endif
