#include "Indicators.h"

// Stub — you can add LED pins later

void Indicators::modeOff()   { Serial.println("[LED] Mode: OFF"); }
void Indicators::modeSnap()  { Serial.println("[LED] Mode: SNAPLINK"); }
void Indicators::modeScrew() { Serial.println("[LED] Mode: SCREW"); }
void Indicators::running()   { Serial.println("[LED] RUNNING"); }
void Indicators::error()     { Serial.println("[LED] ERROR"); }
