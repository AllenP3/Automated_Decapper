#pragma once
#include <Arduino.h>

class PanelIO {
public:
    PanelIO(int modePinA, int modePinB,
            int funcPin,
            int startPin, int stopPin);

    void begin();

    // Read simple results
    String getMode();        // "OFF", "SNAPLINK", "SCREW"
    String getFunction();    // "CALIBRATE", "EXECUTE"

    bool startPressed();
    bool stopPressed();

private:
    int pinA, pinB;
    int pinFunc;
    int pinStart, pinStop;
};
