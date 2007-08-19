#ifndef TOKENITERATOR_H
#define TOKENITERATOR_H
#include "common.h"
class Token;
struct TokenIterator : public virtual Obj {
    virtual bool hasNext()=0;
    virtual Token* next()=0;
};
#endif
