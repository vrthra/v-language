#ifndef QUOTEITERATOR_H
#define QUOTEITERATOR_H
#include "tokeniterator.h"
class QuoteStream;
class QuoteIterator : public TokenIterator {
    QuoteStream* _qs;
    int _index;
public:
    virtual bool hasNext();
    virtual Token* next();
    QuoteIterator(QuoteStream* q);
};
#endif
