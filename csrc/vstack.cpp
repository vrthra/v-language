#include <stdio.h>
#include "tokeniterator.h"
#include "quotestream.h"
#include "cmdquote.h"
#include "vexception.h"
#include "vstack.h"
VStack::VStack():_now(new (collect) Node(0)),_first(_now) {
}

Node* VStack::now() {
    return _now;
}

Node* VStack::now(Node* n) {
    return _now = n;
}

Token* VStack::push(Token* t) {
    Node_ n = new (collect) Node(t);
    n->link = _now;
    _now = n;
    return _now->data;
}

Token* VStack::pop() {
    if (!_now || !_now->data)
        throw VException("err:stack_empty", new (collect) Term(TInt, (long)0), "Empty Stack.");
    Token_ t = _now->data;
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
    Node_ current = _now;
    Node_ result = 0;
    Node_ t = 0;
    while (current && current->data) {
        t = new (collect) Node(current->data);
        t->link = result;
        result = t;
        current = current->link;
    }
    return result;
}

Quote* VStack::quote() {
    Node_ s = getList();
    QuoteStream_ qs = new (collect) QuoteStream();
    while(s) {
        qs->add(s->data);
        s = s->link;
    }
    return new (collect) CmdQuote(qs);
}

void VStack::dequote(Quote* q) {
    Node_ current = _now;
    TokenIterator_ it = q->tokens()->iterator();

    _now = new (collect) Node(0);
    _first = _now;

    while(it->hasNext())
        push(it->next());
}

void VStack::dump() {
    Node_ s = getList();
    printf("(");
    bool first = true;
    while(s) {
        printf("%s%s", first ? "" : " ",s->data->value());
        first = false;
        s = s->link;
    }
    printf(")\n");
}
