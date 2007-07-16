#ifndef TERM_H
#define TERM_H
#include "type.h"
class VFrame;
class Quote;
class Term {
        Type _type;
        union {
            char _cval;
            long _lval;
            double _dval;
            char* _sval;
            Quote* _qval;
            VFrame* _fval;
        };

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
        Quote* qvalue();
        VFrame* fvalue();
};
#endif
