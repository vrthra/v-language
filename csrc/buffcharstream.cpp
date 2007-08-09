#include "buffcharstream.h"
#include <string.h>

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
bool BuffCharStream::eof() {
    return (_current+1) >= _len;
}
BuffCharStream::BuffCharStream(char* buff):_current(-1) {
    if (buff) {
        _len = strlen(buff);
        _buf = dup_str(buff);
    }
}
