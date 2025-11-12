#pragma once
#include <Arduino.h>

class Indicators {
public:
  static void modeOff();
  static void modeSnap();
  static void modeScrew();
  static void running();
  static void error();
};
