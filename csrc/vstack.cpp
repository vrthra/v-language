#include <stack>
#include "vstack.h"
#include "tokeniterator.h"
#include "quotestream.h"
#include "cmdquote.h"
#include "vexception.h"
VStack::VStack() {
    _now = new Node(0);
    _first = _now;
}

Node* VStack::now() {
    return _now;
}

Node* VStack::now(Node* n) {
    return _now = n;
}

Token* VStack::push(Token* t) {
    Node* n = new Node(t);
    n->link = _now;
    _now = n;
    return _now->data;
}

Token* VStack::pop() {
    if (!_now || !_now->data)
        throw VException("err:stack_empty", "Empty Stack.");
    Token* t = _now->data;
    _now = _now->link;
    return t;
}

bool VStack::empty() {
    return _now->link == 0;
}

void VStack::clear() {
    _now = _first;
}

Token* VStack::peek() {
    return _now->data;
}

Node* VStack::getList() {
    Node* current = _now;
    Node* result = 0;
    Node* t = 0;
    while (current && current->data) {
        t = new Node(current->data);
        t->link = result;
        result = t;
        current = current->link;
    }
    return result;
}

Quote* VStack::quote() {
    Node* s = getList();
    QuoteStream* qs = new QuoteStream();
    while(s) {
        qs->add(s->data);
        s = s->link;
    }
    return new CmdQuote(qs);
}

void VStack::dequote(Quote* q) {
    Node* current = _now;
    TokenIterator* it = q->tokens()->iterator();

    _now = new Node(0);
    _first = _now;

    while(it->hasNext())
        push(it->next());
}

void VStack::dump() {
    Node* s = getList();
    printf("(");
    bool first = true;
    while(s) {
        printf("%s%s", first ? "" : " ",s->data->value());
        first = false;
        s = s->link;
    }
    printf(")\n");
}
