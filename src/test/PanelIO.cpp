#include "PanelIO.h"

// --------------------------create an instance like the main file and run -----------------------
/*PanelIO panel(pinModeA, pinModeB, pinFunc, pinStart, pinStop);
if (panel.getMode() == "SCREW" && 
    panel.getFunction() == "EXECUTE" &&
    panel.startPressed()) 
{
    screw.run_Sequence(actuator);
}
*/
PanelIO::PanelIO(int modePinA, int modePinB,
                 int funcPin,
                 int startPin, int stopPin)
{
    pinA = modePinA;
    pinB = modePinB;
    pinFunc = funcPin;
    pinStart = startPin;
    pinStop = stopPin;
}

// ---------------------------------------------------
void PanelIO::begin() {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinFunc, INPUT_PULLUP);
    pinMode(pinStart, INPUT_PULLUP);
    pinMode(pinStop, INPUT_PULLUP);
}

String PanelIO::getMode() {

    int A = digitalRead(pinA);
    int B = digitalRead(pinB);

    if (A == HIGH && B == HIGH) return "OFF";
    if (A == LOW  && B == HIGH) return "SNAPLINK";
    if (A == HIGH && B == LOW ) return "SCREW";

    return "OFF";
}

// ---------------------------------------------------
String PanelIO::getFunction() {

    int f = digitalRead(pinFunc);

    if (f == LOW) return "CALIBRATE";  // switch down
    return "EXECUTE";                  // switch up
}

// ---------------------------------------------------
bool PanelIO::startPressed() {
    return digitalRead(pinStart) == LOW;
}

// ---------------------------------------------------
bool PanelIO::stopPressed() {
    return digitalRead(pinStop) == LOW;
}
