#ifndef QUOTESTREAM_H
#define QUOTESTREAM_H
#include <vector>
#include "tokenstream.h"
class Token;
class QuoteIterator;
class QuoteStream : public TokenStream {
    public:
        QuoteStream();
        virtual void add(Token* t);
        virtual int size();
        virtual Token* get(int i);
        virtual TokenIterator* iterator();
    protected:
        std::vector<Token*> _stream;
};
#endif
