#pragma once
#include "../motors/LinearActuator.h"
#include "../motors/RailStepper.h"
#include "../motors/ClawStepper.h"
#include "../motors/ServoClaw.h"

namespace SnaplinkRoutine {
  void run(LinearActuator&, RailStepper&, ClawStepper&, ServoClaw&);
}
