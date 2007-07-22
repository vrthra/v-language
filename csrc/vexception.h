#ifndef VEXCEPTION_H
#define VEXCEPTION_H
#include <sstream>
#include "vx.h"
class VException : public Vx {
    public:
        VException(char* err, char* msgfmt, ...);
        virtual char* message();
        virtual void addLine(char* v, ...);
        std::stringstream* info;
};

class VSynException : public Vx {
    public:
        VSynException(char* err, char* msgfmt, ...);
        virtual char* message();
        virtual void addLine(char* v, ...);
        std::stringstream* info;
};
#endif
