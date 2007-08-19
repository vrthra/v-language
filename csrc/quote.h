#ifndef QUOTE_H
#define QUOTE_H
#include "common.h"
class TokenStream;
class VFrame;
struct Quote : public virtual Obj {
    virtual void eval(VFrame* scope) = 0;
    virtual TokenStream* tokens() = 0;
    virtual char* to_s() = 0;
};

#endif
