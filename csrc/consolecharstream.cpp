#include "consolecharstream.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "v.h"
ConsoleCharStream::ConsoleCharStream():_index(0),_eof(false),_lexer(0) {
}
char ConsoleCharStream::read() {
    if (strlen(_buf) > (_index + 1)) {
        ++_index;
        _current = _buf[_index];
    } else {
        _index = 0;
        read_nobuf();
        _current = _buf[_index];
    }
    return _current;
}
char ConsoleCharStream::peek() {
    if (strlen(_buf) > (_index+1))
        return _buf[_index+1];
    return 0;
}
char ConsoleCharStream::current() {
    return _current;
}
void ConsoleCharStream::lexer(Lexer* l) {
    _lexer = l;
}

void ConsoleCharStream::read_nobuf() {
    if (_lexer->closed())
        printf("|");
    if(!fgets(_buf, MaxBuf, stdin)) {
        _buf[0] = 0;
        _eof = true;
    }
}
int ConsoleCharStream::index() {
    return _index;
}
bool ConsoleCharStream::eof() {
    return _eof;
}
