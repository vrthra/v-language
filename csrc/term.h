#ifndef TERM_H
#define TERM_H
#include "common.h"
#include "type.h"
#include "token.h"
class VFrame;
class Quote;
class Term : public Token {
    public:
        Term(Type t, bool v);
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
            bool _bval;
            char _cval;
            long _lval;
            double _dval;
            char* _sval;
            Quote* _qval;
            VFrame* _fval;
        };
        Void_ _hold;
        Num _num;
        char* stype(Type t);
        void checkType(Type t, Term* term);
        char _buffer[MaxBuf];
};
#endif
