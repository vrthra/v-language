#ifndef FILECHARSTREAM_H
#define FILECHARSTREAM_H
#include "charstream.h"
class FileCharStream : public CharStream {
    public:
        FileCharStream(char* filename);
        virtual char read();
        virtual char peek();
        virtual char current();
        virtual void lexer(Lexer* l);
};
#endif
