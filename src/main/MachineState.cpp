// MachineState.cpp
#include "MachineState.h"

bool MachineState::running     = false;
bool MachineState::needsHoming = false;

long MachineState::snapCount   = 0;
long MachineState::screwCount  = 0;

void MachineState::init() {
  running     = false;
  needsHoming = false;
  snapCount   = 0;
  screwCount  = 0;
}
