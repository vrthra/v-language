#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H
#include "common.h"
class TokenIterator;
struct TokenStream : public virtual Obj {
    virtual TokenIterator* iterator()=0;
};

#endif
