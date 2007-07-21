#ifndef BUFFCHARSTREAM_H
#define BUFFCHARSTREAM_H
#include "charstream.h"
class BuffCharStream : public CharStream {
    public:
        BuffCharStream(char* buff);
        virtual char read();
        virtual char peek();
        virtual char current();
        virtual void lexer(Lexer* l);
    private:
        char* _buf;
        int _current;
        char _next;
};
#endif
