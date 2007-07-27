#ifndef CMD_H
#define CMD_H
#include <map>
#include "common.h"
#include "quote.h"
class TokenStream;
class Shield;
class Cmd : public Quote {
    public:
        typedef std::map<char*, Shield*, cmp_str> VMap;
        VMap& store() {
            return _store;
        }
        virtual TokenStream* tokens(){return 0;}
        virtual char* to_s(){ return "<cmd>"; }
    private:
        VMap _store;
};
#endif
