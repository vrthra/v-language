#ifndef QUOTEITERATOR_H
#define QUOTEITERATOR_H
#include "tokeniterator.h"
class QuoteStream;
class QuoteIterator : public TokenIterator {
    public:
        virtual bool hasNext();
        virtual Token* next();
        QuoteIterator(QuoteStream* q);
    private:
        QuoteStream* _qs;
        int _index;
};
#endif
