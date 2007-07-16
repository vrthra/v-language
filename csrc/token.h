#ifndef TOKEN_H
#define TOKEN_H
#include "type.h"
class Quote;
class VFrame;
class Token {
    public:
        virtual char* value()=0;
        virtual Type type()=0;
        virtual bool bvalue();
        virtual char cvalue();
        virtual int ivalue();
        virtual double dvalue();
        virtual char* svalue();
        virtual Quote* qvalue();
        virtual VFrame* fvalue();
};
#endif
