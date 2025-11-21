#include "Pins.h"
#include "Config.h"

#include "UI_OLED.h"
#include "Safety.h"
#include "RoutineManager.h"

#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"

// ----------------- GLOBAL OBJECTS -----------------

UI_OLED ui;
Safety safety;

LinearActuator lin;
RailStepper rail;
ClawStepper claw;
ServoClaw servo;

RoutineManager routines(ui, safety, lin, rail, claw, servo);

void setup() {
    ui.begin();
    safety.begin();
    lin.begin();
    rail.begin();
    claw.begin();
    servo.begin();

    routines.begin();
}

void loop() {
    ui.update();
    safety.update();

    lin.update();
    rail.update();
    claw.update();
    servo.update();

    routines.update();
}
