#ifndef CMD_H
#define CMD_H
#include "common.h"
#include "quote.h"
class TokenStream;
class Cmd : public Quote {
    public:
        virtual TokenStream* tokens(){return 0;}
        virtual char* to_s(){ return "<cmd>"; }
};
#endif
