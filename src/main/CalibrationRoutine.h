#ifndef CALIBRATION_ROUTINE_H
#define CALIBRATION_ROUTINE_H

#include <Arduino.h>
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"
#include "UI_OLED.h"
#include "Config.h"

class CalibrationRoutine {
public:
    static void run(
        UI_OLED &ui,
        LinearActuator &lin,
        RailStepper &rail,
        ClawStepper &claw,
        ServoClaw &servo
    );
};

#endif
