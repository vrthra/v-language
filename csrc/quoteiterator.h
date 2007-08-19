#ifndef QUOTEITERATOR_H
#define QUOTEITERATOR_H
#include "tokeniterator.h"
class QuoteIterator : public TokenIterator {
    public:
        virtual bool hasNext();
        virtual Token* next();
        QuoteIterator(Node* q);
    private:
        Node_ _qs;
};
#endif
