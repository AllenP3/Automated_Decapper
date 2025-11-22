#ifndef CALIBRATION_ROUTINE_H
#define CALIBRATION_ROUTINE_H

#include <Arduino.h>
#include "BaseRoutine.h"

// Forward declarations
class UI_OLED;
class LinearActuator;
class RailStepper;
class ClawStepper;
class ServoClaw;

class CalibrationRoutine : public BaseRoutine {
public:
    CalibrationRoutine(UI_OLED &uiRef,
                       LinearActuator &linRef,
                       RailStepper &railRef,
                       ClawStepper &clawRef,
                       ServoClaw &servoRef);

    void begin() override;
    void update() override;
    bool isFinished() override { return finished; }
    const char* getName() override { return "CALIB"; }

private:
    LinearActuator* lin;
    RailStepper* rail;
    ClawStepper* claw;
    ServoClaw* servo;

    bool finished = false;
};

#endif
