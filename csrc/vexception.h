#ifndef VEXCEPTION_H
#define VEXCEPTION_H
#include <sstream>
#include "vx.h"
#include "token.h"
class Quote;
class VException : public Vx {
    public:
        VException(char* err, Token* t, char* msgfmt, ...);
        virtual char* message();
        virtual void addLine(char* v, ...);
        std::stringstream* info;
        Token* token();
    private:
        char* _err;
        Token* _token;
};

class VSynException : public Vx {
    public:
        VSynException(char* err, char* msgfmt, ...);
        virtual char* message();
        virtual void addLine(char* v, ...);
        std::stringstream* info;
};
#endif
