#ifndef TERM_H
#define TERM_H
#include "type.h"
#include "num.h"
class VFrame;
class Quote;
class Term {
    public:
        Term(Type t, long v);
        Term(Type t, double v);
        Term(Type t, char* v);
        Term(Type t, char v);
        Term(Type t, Quote* v);
        Term(Type t, VFrame* v);
        Type type();
        int size();
        char* value();
        bool bvalue();
        char cvalue();
        long ivalue();
        double dvalue();
        char* svalue();
        Num numvalue();
        Quote* qvalue();
        VFrame* fvalue();
    private:
        Type _type;
        union {
            char _cval;
            long _lval;
            double _dval;
            char* _sval;
            Quote* _qval;
            VFrame* _fval;
        };
        Num _num;
};
#endif
