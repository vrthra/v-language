#include <string>
#include "vexception.h"

VException::VException(char* u, char* v) {
    std::strncpy(_message,u,1024);
    std::strncpy(_detail,v,2048);
}

char* VException::message() {
    return _message;
}
