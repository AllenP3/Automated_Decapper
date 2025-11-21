#include "SnapLinkRoutine.h"

SnapLinkRoutine::SnapLinkRoutine()
: state(IDLE), active(false) {}

void SnapLinkRoutine::begin() {
    state = IDLE;
    active = false;
}

void SnapLinkRoutine::start() {
    active = true;
    state = MOVE_RAIL_FORWARD;
    phaseStart = millis();
}

void SnapLinkRoutine::update() {
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
            servo->close();
            state = PULL_UP;
        }
        break;

    case PULL_UP:
        if (!servo->isBusy()) {
            lin->moveToMM(0);
            state = MOVE_RAIL_BACK;
        }
        break;

    case MOVE_RAIL_BACK:
        if (!lin->isBusy()) {
            rail->moveTo(0);
            state = DONE;
        }
        break;

    case DONE:
        if (!rail->isBusy()) {
            active = false;
            state = IDLE;
        }
        break;
    }
}
