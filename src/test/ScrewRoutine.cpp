#include "ScrewRoutine.h"

// -------------------------------------------------------
// ScrewRoutine screw(&clawServo, 150, 80, rotationPins); should create an instance like this in the main file
ScrewRoutine::ScrewRoutine(Servo *claw,
                           int openAngle,
                           int closeAngle,
                           int stepPins[4])
{
    this->claw = claw;
    this->openAngle = openAngle;
    this->closeAngle = closeAngle;

    for (int i = 0; i < 4; i++)
        this->motorPins[i] = stepPins[i];

    stepIndex = 0;
}

// -------------------------------------------------------
void ScrewRoutine::begin()
{
    for (int i = 0; i < 4; i++)
        pinMode(motorPins[i], OUTPUT);
}

// -------------------------------------------------------
void ScrewRoutine::openClaw()
{
    claw->write(openAngle);
    delay(300);
}

// -------------------------------------------------------
void ScrewRoutine::closeClaw()
{
    claw->write(closeAngle);
    delay(300);
}

// -------------------------------------------------------
void ScrewRoutine::rotateOneStepCW()
{ // going from seq[0] to seq[7] makes the stepper turn clockwise 1 step
    for (int i = 0; i < 4; i++)
        digitalWrite(motorPins[i], seq[stepIndex][i]);

    stepIndex++;
    if (stepIndex >= 8)
        stepIndex = 0;
}
void ScrewRoutine::rotateOneStepCCW()
{
    for (int i = 0; i < 4; i++)
        digitalWrite(motorPins[i], seq[stepIndex][i]);

    stepIndex--;
    if (stepIndex < 0)
        stepIndex = 7;
}
// call this from the main after creating a ScrewRoutine instance
void ScrewRoutine::run_Sequence(LinearActuator &actuator)
{

    // 1. Open the claw
    openClaw();

    // 2. Move down
    actuator.moveDown(30); // move 30 mm down

    // 3. Close the claw
    closeClaw();

    // 4. Rotate
    for (int i = 0; i < 1500; i++)
    { // not sure about 1500
        rotateOneStepCW();
        delay(2); // step speed
    }

    // 5. Move up
    actuator.moveUp(30);
}
