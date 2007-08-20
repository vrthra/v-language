#include "buffcharstream.h"
#include <string.h>

BuffCharStream::BuffCharStream(char* buff):_buf(0),_current(-1),_len(0){
    if (buff) {
        _len = strlen(buff);
        _buf = dup_str(buff);
    }
}
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

