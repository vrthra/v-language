#ifndef VEXCEPTION_H
#define VEXCEPTION_H
#include "vx.h"
struct VException : public Vx {
    VException(char* v, char* u);
};
struct VSynException : public VException {
    VSynException(char* v, char* u):VException(v,u){
    }
};
#endif
