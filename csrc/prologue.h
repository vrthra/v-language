#ifndef PROLOGUE_H
#define PROLOGUE_H
class VFrame;
class VStack;
class Node;
class Quote;
struct Shield {
    Node* stack;
    Quote* quote;
    Shield* next;
    Shield(VStack* s, Quote* q);
};
struct Prologue {
    static void init(VFrame* frame);
};
#endif
