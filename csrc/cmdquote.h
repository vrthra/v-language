#ifndef CMDQUOTE_H
#define CMDQUOTE_H
#include "quote.h"
class LexStream;
class VFrame;
class CmdQuote : public Quote {
    public:
    void eval(VFrame* scope);
    TokenStream* tokens();
    CmdQuote(LexStream* l);
    void dofunction(VFrame* scope);
};
#endif
