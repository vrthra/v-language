#ifndef CONSOLECHARSTREAM_H
#define CONSOLECHARSTREAM_H
#include "charstream.h"
struct Lexer;
class ConsoleCharStream : public CharStream {
    public:
        ConsoleCharStream();
        virtual char read();
        virtual char peek();
        virtual char current();
        virtual void lexer(Lexer* l);
        virtual bool eof();
    private:
        char _buf[MaxBuf];
        int _index;
        Lexer* _lexer;
        char _current;
        char* read_nobuf();
        virtual int index();
};
#endif
