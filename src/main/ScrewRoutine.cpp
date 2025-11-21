#include "ScrewRoutine.h"

ScrewRoutine::ScrewRoutine()
: state(IDLE), active(false) {}

void ScrewRoutine::begin() {
    active = false;
    state = IDLE;
}

void ScrewRoutine::start() {
    active = true;
    state = MOVE_RAIL_FORWARD;
    phaseStart = millis();
}

void ScrewRoutine::update() {
    if (!active) return;

    switch (state) {

    case MOVE_RAIL_FORWARD:
        rail->moveTo(RAIL_MAX_TRAVEL_STEPS);
        state = MOVE_LINEAR_DOWN;
        break;

    case MOVE_LINEAR_DOWN:
        if (!rail->isBusy()) {
            lin->moveToMM(50);
            state = CLOSE_CLAW;
        }
        break;

    case CLOSE_CLAW:
        if (!lin->isBusy()) {
            servo->closeStrong();
            state = BREAK_TORQUE;
            phaseStart = millis();
        }
        break;

    case BREAK_TORQUE:
        if (!servo->isBusy()) {
            claw->rotateDegrees(CLAW_TWIST_BREAK_DEGREES);
            state = UNSCREW_ROTATE;
        }
        break;

    case UNSCREW_ROTATE:
        if (!claw->isBusy()) {
            // Begin continuous CCW rotation
            claw->rotateSteps(99999);

            // Linear actuator rises slowly while claw rotates
            long upSteps = (LIN_UNSCREW_UP_MMPS / LIN_MM_PER_STEP) * 2; // small chunk
            lin->moveSteps(-upSteps);

            state = LIFT_UP;
        }
        break;

    case LIFT_UP:
        claw->update();
        lin->update();

        if (!claw->isBusy() && !lin->isBusy()) {
            state = MOVE_RAIL_BACK;
            rail->moveTo(0);
        }
        break;

    case MOVE_RAIL_BACK:
        if (!rail->isBusy()) {
            active = false;
            state = IDLE;
        }
        break;
    }
}
