#ifndef CALIBRATION_ROUTINE_H
#define CALIBRATION_ROUTINE_H

#include <Arduino.h>
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "UI_OLED.h"
#include "BaseRoutine.h"
#include "Config.h"

class CalibrationRoutine : public BaseRoutine {
public:
    CalibrationRoutine(LinearActuator &linRef,
                       RailStepper &railRef,
                       ClawStepper &clawRef,
                       ServoClaw &servoRef);

    void begin() override {}
    void update() override {}
    bool isFinished() override { return finished; }
    const char* getName() override { return "CALIB"; }

    static void run(UI_OLED &ui,
                    LinearActuator &lin,
                    RailStepper &rail,
                    ClawStepper &claw,
                    ServoClaw &servo);

private:
    bool finished = false;

    LinearActuator* lin;
    RailStepper* rail;
    ClawStepper* claw;
    ServoClaw* servo;
};


#endif
