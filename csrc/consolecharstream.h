#ifndef CONSOLECHARSTREAM_H
#define CONSOLECHARSTREAM_H
#include "charstream.h"
class ConsoleCharStream : public CharStream {
    public:
    char read();
    char peek();
    char current();
    void lexer(Lexer* l);
    ConsoleCharStream();
};
#endif
