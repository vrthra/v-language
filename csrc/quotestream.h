#ifndef QUOTESTREAM_H
#define QUOTESTREAM_H
#include "token.h"
#include "tokenstream.h"
class Token;
class QuoteIterator;
class QuoteStream : public TokenStream {
    public:
        QuoteStream();
        virtual void add(Token* t);
        virtual int size();
        virtual TokenIterator* iterator();
    protected:
        Node* _stream;
        Node* _first;
        long _size;
};
#endif
