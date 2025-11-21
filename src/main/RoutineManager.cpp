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

    // ---------------- SAFETY OVERRIDE ----------------
    if (safety.stopPressed()) {
        stopRoutine();
        ui.showMessage("STOPPED", "Emergency Stop");
        ui.requireHoming(true);
        return;
    }

    // ---------------- HANDLE ACTIVE ROUTINE ----------------
    if (activeRoutine) {

        activeRoutine->update();

        if (activeRoutine->isFinished()) {
            ui.showMessage("DONE", activeRoutine->getName(), 900);
            delete activeRoutine;
            activeRoutine = nullptr;

            return;
        }

        return; // no UI idle handling while routine running
    }

    // ---------------- WHEN IDLE: CHECK FOR START ----------------
    if (ui.startRequested()) {
        Mode m = ui.getMode();

        if (ui.homingRequired() &&
            m != MODE_HOME &&
            m != MODE_JOG &&
            m != MODE_INFO)
        {
            ui.showMessage("HOMING REQUIRED", "Select HOME/JOG", 900);
            return;
        }

        startRoutine(m);
    }
}

void RoutineManager::startRoutine(Mode mode) {

    if (activeRoutine != nullptr) return;

    switch (mode) {

        case MODE_SNAP:
            activeRoutine = new SnapLinkRoutine(ui, lin, rail, claw, servo);
            break;

        case MODE_SCREW:
            activeRoutine = new ScrewRoutine(ui, lin, rail, claw, servo);
            break;

        case MODE_HOME:
            activeRoutine = new HomeRoutine(ui, lin, rail, claw, servo);
            break;

        case MODE_JOG:
            activeRoutine = new JogRoutine(ui, lin, rail, claw, servo);
            break;

        case MODE_INFO:
            ui.showInfoScreen();
            break;

        case MODE_CALIBRATE:
            activeRoutine = new CalibrationRoutine(ui, lin, rail, claw, servo);
            break;

        default:
            ui.showMessage("INVALID MODE", "", 700);
            break;
    }

    if (activeRoutine) {
        activeRoutine->begin();
    }
}

void RoutineManager::stopRoutine() {
    if (!activeRoutine) return;

    activeRoutine->stop();
    delete activeRoutine;
    activeRoutine = nullptr;

    lin.stop();
    rail.moveTo(rail.getPosition());  // freeze
    claw.rotateSteps(0);              // freeze
    servo.open();

    ui.requireHoming(true);
}
