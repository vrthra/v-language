#ifndef VSTACK_H
#define VSTACK_H
#include "term.h"

struct Node {
    Token* data;
    Node* link;
    Node(Token* e) {
        data = e;
    }
};
struct Quote;
class VStack {
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
        Node* _now;
        Node* _first;
        Node* getList();
};

#endif
