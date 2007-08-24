#ifndef VEXCEPTION_H
#define VEXCEPTION_H
#include "common.h"
#include "vx.h"
#include "token.h"
class Quote;
class VException : public Vx {
    public:
        VException(char* err, Token* t, char* msgfmt, ...);
        virtual char* message();
        virtual void addLine(char* v, ...);
        Token* token();
    private:
        Char_ _err;
        Token_ _token;
        char _info[MaxBuf*16];
        int _i;
};

class VSynException : public Vx {
    public:
        VSynException(char* err, char* msgfmt, ...);
        virtual char* message();
        virtual void addLine(char* v, ...);
    private:
        Char_ _err;
        char _info[MaxBuf*16];
        int _i;
};
#endif
