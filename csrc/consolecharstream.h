#ifndef CONSOLECHARSTREAM_H
#define CONSOLECHARSTREAM_H
#include <vector>
#include "charstream.h"
struct Lexer;
class ConsoleCharStream : public CharStream {
    public:
        ConsoleCharStream();
        virtual char read();
        virtual char peek();
        virtual char current();
        virtual void lexer(Lexer* l);
        virtual int index();
    private:
        char _buf[1024];
        int _index;
        Lexer* _lexer;
        char _current;
        char* read_nobuf();
};
#endif
