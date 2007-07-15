#ifndef CMDQUOTE_H
#define CMDQUOTE_H
#include "quote.h"
class LexStream;
class VFrame;
class VStack;
class CmdQuote : public Quote {
    TokenStream* _tokens;
    public:
    void eval(VFrame* scope);
    TokenStream* tokens();
    CmdQuote(TokenStream* l);
    void dofunction(VFrame* scope);
    private:
    bool cando(VStack* stack);
};
#endif
