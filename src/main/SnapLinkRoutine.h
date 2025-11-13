#pragma once
#include "LinearActuator.h"
#include "RailStepper.h"
#include "ClawStepper.h"
#include "ServoClaw.h"

namespace SnaplinkRoutine {
  void run(LinearActuator&, RailStepper&, ClawStepper&, ServoClaw&);
}
