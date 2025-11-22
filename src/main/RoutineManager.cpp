#include "RoutineManager.h"
#include "Config.h"

RoutineManager::RoutineManager(UI_OLED &uiRef,
                               Safety &safetyRef,
                               LinearActuator &linRef,
                               RailStepper &railRef,
                               ClawStepper &clawRef,
                               ServoClaw &servoRef)
: ui(uiRef),
  safety(safetyRef),
  lin(linRef),
  rail(railRef),
  claw(clawRef),
  servo(servoRef)
{}

void RoutineManager::begin() {
    activeRoutine = nullptr;
}

void RoutineManager::update() {

    // SAFETY OVERRIDE
    if (safety.stopRequested()) {
        stopRoutine();
        ui.showMessage("STOPPED", "Emergency Stop");
        ui.requireHoming(true);
        return;
    }

    // IF ROUTINE RUNNING
    if (activeRoutine) {

        activeRoutine->update();

        if (activeRoutine->isFinished()) {

            ui.showMessage("DONE", activeRoutine->getName(), 900);

            delete activeRoutine;
            activeRoutine = nullptr;
        }

        return;
    }

    // IDLE: check start
    if (ui.startRequested()) {
        
        if (ui.homingRequired() &&
            ui.getMode() != MODE_HOME &&
            ui.getMode() != MODE_JOG &&
            ui.getMode() != MODE_INFO)
        {
            ui.showMessage("HOMING REQUIRED", "Select HOME/JOG");
            return;
            Serial.println("Stuck in homing");
        }

        startRoutine(ui.getMode());
        Serial.println("Starting routine");
    }
}

void RoutineManager::startRoutine(Mode mode) {

    if (activeRoutine != nullptr) return;

    switch (mode) {

        case MODE_SNAP:
            activeRoutine = new SnapLinkRoutine(lin, rail, claw, servo);
            break;

        case MODE_SCREW:
            activeRoutine = new ScrewRoutine(lin, rail, claw, servo);
            break;

        case MODE_HOME:
    activeRoutine = new HomeRoutine(ui, lin, rail, claw);
    break;

case MODE_JOG:
    activeRoutine = new JogRoutine(ui, lin, rail, claw, servo);
    break;

case MODE_CALIBRATE:
    activeRoutine = new CalibrationRoutine(ui, lin, rail, claw, servo);
    break;


        default:
            ui.showMessage("INVALID MODE", "");
            return;
    }

    activeRoutine->update();
    Serial.println("Started begin");
}

void RoutineManager::stopRoutine() {

    if (!activeRoutine) return;

    activeRoutine->stop();
    delete activeRoutine;
    activeRoutine = nullptr;

    lin.stop();
    rail.moveTo(rail.getPosition());
    claw.rotateSteps(0);
    servo.open();

    ui.requireHoming(true);
}
