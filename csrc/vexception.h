#ifndef VEXCEPTION_H
#define VEXCEPTION_H
#include "vx.h"
struct VException : public Vx {
    VException(char* v, char* u);
};
#endif
