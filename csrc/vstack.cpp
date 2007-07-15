#include "vstack.h"
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

Term* VStack::push(Term* t) {
    Node* n = new Node(t);
    n->link = _now;
    _now = n;
    return _now->data;
}

Term* VStack::pop() {
    if (!_now || !_now->data)
        throw VException("err:stack_empty", "Empty Stack.");
    Term* t = _now->data;
    _now = _now->link;
    return t;
}

bool VStack::empty() {
    return _now->link == 0;
}

void VStack::clear() {
    _now = _first;
}

Term* VStack::peek() {
    return _now->data;
}

Quote* VStack::quote() {
    return 0;
}
void VStack::dequote(Quote* q) {
}
void VStack::dump() {
}
