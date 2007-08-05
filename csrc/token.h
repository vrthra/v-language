#ifndef TOKEN_H
#define TOKEN_H
#include "type.h"
#include "num.h"
class Quote;
class VFrame;
class Num;
struct Token {
    virtual char* value()=0;
    virtual Type type()=0;
    virtual bool bvalue()=0;
    virtual char cvalue()=0;
    virtual long ivalue()=0;
    virtual double dvalue()=0;
    virtual char* svalue()=0;
    virtual Quote* qvalue()=0;
    virtual VFrame* fvalue()=0;
    virtual Num numvalue()=0;
};

struct Node {
    Token* data;
    Node* link;
    Node(Token* e) {
        data = e;
        link = 0;
    }
};

#endif
