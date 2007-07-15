#ifndef FILECHARSTREAM_H
#define FILECHARSTREAM_H
#include "charstream.h"
class FileCharStream : public CharStream {
    public:
    char read();
    char peek();
    char current();
    void lexer(Lexer* l);
    FileCharStream(char* filename);
};
#endif
