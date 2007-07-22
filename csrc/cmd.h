#ifndef CMD_H
#define CMD_H
#include <map>
#include "quote.h"
class TokenStream;
class Shield;
class Cmd : public Quote {
    public:
        typedef std::map<char*, Shield*> VMap;
        VMap& store() {
            return _store;
        }
        virtual TokenStream* tokens(){return 0;}
        char* to_s(){return "<cmd>";}
    private:
        VMap _store;
};
#endif
