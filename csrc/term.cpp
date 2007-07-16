#include "term.h"

Term::Term(Type t,char val) {
    _cval = val;
}

Term::Term(Type t,char* val) {
    _sval = val;
}

Term::Term(Type t,long val) {
    _lval = val;
}

Term::Term(Type t,double val) {
    _dval = val;
}

Term::Term(Type t,Quote* val) {
    _qval = val;
}

Term::Term(Type t,VFrame* val) {
    _fval = val;
}

int Term::size() {
    return 0;
}

Type Term::type() {
    return _type;
}

char* Term::value() {
    return 0; //TODO
}

char* Term::svalue() {
    return _sval;
}

char Term::cvalue() {
    return _cval;
}

Quote* Term::qvalue() {
    return _qval;
}

VFrame* Term::fvalue() {
    return _fval;
}

long Term::ivalue() {
    return _lval;
}

double Term::dvalue() {
    return _dval;
}

