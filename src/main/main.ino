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
    Serial.begin(9600);
    ui.begin();
    // Serial.println("finished ui");
    safety.begin();
    // Serial.println("finished safety");
    lin.begin();
    // Serial.println("finished linear");
    rail.begin();
    // Serial.println("finished rail");
    claw.begin();
    // Serial.println("finished claw");
    servo.begin();
    // Serial.println("finished servo");

    routines.begin();

}

void loop() {
    // Serial.print("reached loop");
    Serial.print("JOY_SW=");
    Serial.print(digitalRead(PIN_JOY_SW));
    ui.update();
    safety.update();

    lin.update();
    rail.update();
    claw.update();
    servo.update();

    routines.update();
}
