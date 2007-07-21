#ifndef VFRAME_H
#define VFRAME_H
#include <string>
#include <map>
class Quote;
struct cmp_str {
    bool operator()(char const *a, char const *b) {
        return std::strcmp(a, b) < 0;
    }
};

typedef std::map<char*, Quote*, cmp_str> QMap;
class VStack;
class VFrame {
    public:
        QMap& dict();
        int id();

        VFrame();
        VFrame(VFrame* parent);
        Quote* lookup(char* key);
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
