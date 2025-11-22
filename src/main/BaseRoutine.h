#ifndef BASE_ROUTINE_H
#define BASE_ROUTINE_H

#include <Arduino.h>

class UI_OLED;

class BaseRoutine {
public:
    virtual void begin() = 0;
    virtual void update() = 0;
    virtual bool isFinished() = 0;
    virtual const char* getName() = 0;

    // NEW — needed because Screw/Snap override stop()
    virtual void stop() {}   

    UI_OLED* ui = nullptr;
};

#endif
