#ifndef LEXSTREAM_H
#define LEXSTREAM_H
#include "quotestream.h"
class CharStream;
class LexStream : public QuoteStream {
    public:
        LexStream(CharStream* c);
        virtual TokenIterator* iterator();
    private:
        CharStream* _stream;
};
#endif
