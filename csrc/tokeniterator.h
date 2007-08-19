#ifndef TOKENITERATOR_H
#define TOKENITERATOR_H
#include "defs.h"
#include "common.h"
struct TokenIterator : public virtual Obj {
    virtual bool hasNext()=0;
    virtual Token* next()=0;
};
#endif
