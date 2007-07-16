#ifndef CMD_H
#define CMD_H
#include <map>
#include "quote.h"
class TokenStream;
class Cmd : public Quote {
    public:
        typedef std::map<char*, void*> VMap;
        VMap& store();
        virtual TokenStream* tokens(){return 0;}
    private:
        VMap _store;
};
#endif
