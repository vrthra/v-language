#ifndef TOKENITERATOR_H
#define TOKENITERATOR_H
class Token;
struct TokenIterator {
    virtual bool hasNext()=0;
    virtual Token* next()=0;
};
#endif
