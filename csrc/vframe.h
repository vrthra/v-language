#ifndef VFRAME_H
#define VFRAME_H
#include "vstack.h"
#include <string>
#include <map>
class Quote;
struct cmp_str {
    bool operator()(char const *a, char const *b) {
        return std::strcmp(a, b) < 0;
    }
};    

typedef std::map<char*, Quote*, cmp_str> QMap;
class VFrame {
    QMap _id;
    public:
        QMap& dict();
        char* id();

        VFrame();
        VFrame(VFrame* parent);
        Quote* lookup(char* key);
        void def(char* sym, Quote* q);
        VFrame* parent();
        VFrame* child();
        VStack* stack();
        void dump();
        void reinit();
};
#endif
