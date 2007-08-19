#ifndef VSTACK_H
#define VSTACK_H
#include "term.h"
struct Quote;
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
        P<Node> _now;
        P<Node> _first;
        Node* getList();
};

#endif
