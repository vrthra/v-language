#include "vexception.h"
#include "charstream.h"
#include "cmdquote.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

VException::VException(char* err, Token* t, char* msgfmt, ...):_token(t)
   ,_err(0),_i(0) {
    va_list argp;
    va_start(argp, msgfmt);
    vsnprintf(_info + _i, MaxBuf * 16 - _i, msgfmt, argp);
    va_end(argp);
    _i = strchr(_info, '\0') - _info;
}

char* VException::message() {
    return _info;
}

Token* VException::token() {
    return _token;
}

void VException::addLine(char* v, ...) {
    strncpy(_info + _i, "\n\t", MaxBuf * 16 - _i);
    _i += 2;
    va_list argp;
    va_start(argp, v);
    vsprintf(_info + _i, v, argp);
    va_end(argp);

    _i = strchr(_info, '\0') - _info;
}

VSynException::VSynException(char* err, char* msgfmt, ...):_i(0) {
    va_list argp;
    va_start(argp, msgfmt);
    vsnprintf(_info + _i, MaxBuf * 16 - _i, msgfmt, argp);
    va_end(argp);
    _i = strchr(_info, '\0') - _info;
}

char* VSynException::message() {
    return _info;
}

void VSynException::addLine(char* v, ...) {
    strncpy(_info + _i, "\n\t", MaxBuf * 16 - _i);
    _i += 2;
    va_list argp;
    va_start(argp, v);
    vsnprintf(_info + _i, MaxBuf * 16 - _i, v, argp);
    va_end(argp);

    _i = strchr(_info, '\0') - _info;
}
