#ifndef VSTACK_H
#define VSTACK_H
#include "term.h"
class VStack : public virtual Obj {
    public:
        VStack();
        Node* now();
        Node* now(Node* n);
        Token* push(Token* t);
        Token* pop();
        bool empty();
        void clear();
        Token* peek();
        Quote* quote();
        void dequote(Quote* q);
        void dump();
    private:
        Node_ _now;
        Node_ _first;
        Node* getList();
};

#endif
