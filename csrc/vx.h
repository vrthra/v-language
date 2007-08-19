#ifndef VX_H
#define VX_H
#include "common.h"
#include "defs.h"

struct Vx : public virtual Obj {
    virtual char* message() = 0;
    virtual void addLine(char* v, ...) = 0;
};

#endif
