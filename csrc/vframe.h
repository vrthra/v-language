#ifndef VFRAME_H
#define VFRAME_H
#include <map>
#include "common.h"
class Quote;
class VStack;
// constant strings, does not need cmp_str.
typedef std::map<char*, Quote*> QMap;
class VFrame {
    public:
        int id();

        VFrame();
        Quote* lookup(char* key);
        Quote* words();
        void def(char* sym, Quote* q);
        VFrame* parent();
        VFrame* child();
        VStack* stack();
        void dump();
        void reinit();
        char* to_s();
    private:
        VFrame(VFrame* parent);
        static int _idcount;
        int _id;
        bool hasKey(char* key);
        QMap _dict;
        VStack* _stack;
        VFrame* _parent;
};
#endif
