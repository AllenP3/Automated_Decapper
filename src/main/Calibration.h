#pragma once
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"

namespace Calibration {
  void runAll(LinearActuator&, RailStepper&, ClawStepper&);
}
