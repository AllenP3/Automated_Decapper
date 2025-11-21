#ifndef ROUTINE_MANAGER_H
#define ROUTINE_MANAGER_H

#include <Arduino.h>
#include "UI_OLED.h"
#include "Safety.h"
#include "SnapLinkRoutine.h"
#include "ScrewRoutine.h"
#include "HomeRoutine.h"
#include "JogRoutine.h"
#include "CalibrationRoutine.h"

// Actuators
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"

class RoutineManager {
public:
    RoutineManager(UI_OLED &uiRef,
                   Safety &safetyRef,
                   LinearActuator &linRef,
                   RailStepper &railRef,
                   ClawStepper &clawRef,
                   ServoClaw &servoRef);

    void begin();
    void update();                    // called every loop

private:
    UI_OLED &ui;
    Safety &safety;

    LinearActuator &lin;
    RailStepper &rail;
    ClawStepper &claw;
    ServoClaw &servo;

    // Currently active routine pointer
    BaseRoutine *activeRoutine = nullptr;

    void startRoutine(Mode mode);
    void stopRoutine();
};

#endif
