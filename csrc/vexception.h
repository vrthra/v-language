#ifndef VEXCEPTION_H
#define VEXCEPTION_H
#include "vx.h"
struct VException : public Vx {
    char _message[1024];
    char _detail[2048];
    VException(char* v, char* u);
    char* message();
};
struct VSynException : public VException {
    VSynException(char* v, char* u):VException(v,u) {}
};
#endif
