#ifndef VFRAME_H
#define VFRAME_H
#include <map>
#include "common.h"
class Quote;
class VStack;
typedef std::map<char*, Quote*, cmp_str> QMap;
class VFrame {
    public:
        QMap& dict();
        int id();

        VFrame();
        VFrame(VFrame* parent);
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
        static int _idcount;
        int _id;
        bool hasKey(char* key);
        QMap _dict;
        VStack* _stack;
        VFrame* _parent;
};
#endif
