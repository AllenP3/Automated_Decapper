#pragma once
#include "../motors/LinearActuator.h"
#include "../motors/RailStepper.h"
#include "../motors/ClawStepper.h"

namespace Calibration {
  void runAll(LinearActuator&, RailStepper&, ClawStepper&);
}
