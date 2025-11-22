#ifndef BASE_ROUTINE_H
#define BASE_ROUTINE_H

class BaseRoutine {
public:
    virtual void begin() = 0;
    virtual void update() = 0;
    virtual void stop() {}  
    virtual bool isFinished() { return false; }
    virtual const char* getName() { return "Routine"; }
    virtual ~BaseRoutine() {}
};

#endif
