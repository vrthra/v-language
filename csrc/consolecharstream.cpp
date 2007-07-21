#include "consolecharstream.h"
#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
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
ConsoleCharStream::ConsoleCharStream() {
    _index = 0;
}
char* ConsoleCharStream::read_nobuf() {
    if (_lexer->closed())
        printf("|");
    else
        printf("");
    if(!fgets(_buf, 1024, stdin))
        exit(0); //^D
    return _buf;
}
int ConsoleCharStream::index() {
    return _index;
}
bool ConsoleCharStream::eof() {
    return false;
}
