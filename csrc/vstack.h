#ifndef VSTACK_H
#define VSTACK_H
#include "term.h"

struct Node {
    Term* data;
    Node* link;
    Node(Term* e) {
        data = e;
    }
};
struct Quote;
class VStack {
    public:
        VStack();
        Node* now();
        Node* now(Node* n);
        Term* push(Term* t);
        Term* pop();
        bool empty();
        void clear();
        Term* peek();
        Quote* quote();
        void dequote(Quote* q);
        void dump();
    private:
        Node* _now;
        Node* _first;
};

#endif
