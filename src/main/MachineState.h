// MachineState.h
#pragma once
#include <Arduino.h>

class MachineState {
public:
  static void init();

  // global status flags
  static bool running;        // a routine is currently active
  static bool needsHoming;    // must home before next SNAP/SCREW

  // counters
  static long snapCount;      // successful SNAP cycles
  static long screwCount;     // successful SCREW cycles
};
