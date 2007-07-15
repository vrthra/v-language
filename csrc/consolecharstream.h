#ifndef CONSOLECHARSTREAM_H
#define CONSOLECHARSTREAM_H
#include <vector>
#include "charstream.h"
struct Lexer;
class ConsoleCharStream : public CharStream {
    char _buf[1024];
    int _index;
    Lexer* _lexer;
    char _current;

    char* read_nobuf();
    public:
    char read();
    char peek();
    char current();
    void lexer(Lexer* l);
    ConsoleCharStream();
    int index();
};
#endif
