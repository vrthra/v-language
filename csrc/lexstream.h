#ifndef LEXSTREAM_H
#define LEXSTREAM_H
#include "defs.h"
#include "quotestream.h"
class LexStream : public QuoteStream {
    public:
        LexStream(CharStream* c);
        virtual TokenIterator* iterator();
    private:
        CharStream_ _stream;
};
#endif
