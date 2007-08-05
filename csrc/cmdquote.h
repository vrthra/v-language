#ifndef CMDQUOTE_H
#define CMDQUOTE_H
#include "quote.h"
class VFrame;
class VStack;
class CmdQuote : public Quote {
    public:
        CmdQuote(TokenStream* l);
        virtual void eval(VFrame* scope);
        virtual TokenStream* tokens();
        virtual void dofunction(VFrame* scope);
        virtual char* to_s();
        static Quote* getdef(char* def);
    private:
        bool cando(VStack* stack);
        TokenStream* _tokens;
        char* _val;
};
#endif
