#include "vexception.h"
#include "charstream.h"
#include "cmdquote.h"
#include <stdarg.h>

VException::VException(char* err, char* msgfmt, ...) {
    info = new std::stringstream();
    *info << err;
    _err = err;
    char buffer[MaxBuf];
    va_list argp;
    va_start(argp, msgfmt);
    std::vsprintf(buffer, msgfmt, argp);
    va_end(argp);
    *info <<' '<< buffer;
}

char* VException::message() {
    return (char*)info->str().c_str();
}

Quote* VException::quote() {
    return CmdQuote::getdef(_err);
}

void VException::addLine(char* v, ...) {
    char buffer[MaxBuf];

    va_list argp;
    va_start(argp, v);
    std::vsprintf(buffer, v, argp);
    va_end(argp);

    *info <<"\n\t"<< buffer;
}

VSynException::VSynException(char* err, char* msgfmt, ...) {
    info = new std::stringstream();
    *info << err;

    char buffer[MaxBuf];
    va_list argp;
    va_start(argp, msgfmt);
    std::vsprintf(buffer, msgfmt, argp);
    va_end(argp);
    *info << buffer;
}

char* VSynException::message() {
    return (char*)info->str().c_str();
}

void VSynException::addLine(char* v, ...) {
    char buffer[MaxBuf];

    va_list argp;
    va_start(argp, v);
    std::vsprintf(buffer, v, argp);
    va_end(argp);

    *info <<"\n\t"<< buffer;
}
