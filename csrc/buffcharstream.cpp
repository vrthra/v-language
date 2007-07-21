#include <fstream>
#include <sstream>
#include "buffcharstream.h"

char BuffCharStream::read() {
    ++_current;
    return _buf[_current];
}
char BuffCharStream::peek() {
    return _buf[_current+1];
}
char BuffCharStream::current() {
    return _buf[_current];
}
void BuffCharStream::lexer(Lexer* l) {
}
BuffCharStream::BuffCharStream(char* buff):_current(0) {
    _buf = new char[strlen(buff)+1];
    std::strcpy(_buf, buff);
}
