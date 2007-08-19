#ifndef VFRAME_H
#define VFRAME_H
#include <map>
#include "common.h"
class Quote;
class VStack;
// constant strings, does not need cmp_str.
typedef std::map<P<char>, P<Quote> > QMap;
class VFrame : public virtual Obj {
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
        P<VStack> _stack;
        P<VFrame> _parent;
};
#endif
