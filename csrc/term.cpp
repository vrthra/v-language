#include <string>
#include "term.h"
#include "quote.h"
#include "quotestream.h"
#include "vframe.h"

Term::Term(Type t,bool val):_type(t) {
    _bval = val;
}

Term::Term(Type t,char val):_type(t) {
    _cval = val;
}

Term::Term(Type t,char* val):_type(t) {
    _sval = val;
}

Term::Term(Type t,long val):_num(val),_type(t) {
    _lval = val;
}

Term::Term(Type t,double val):_num(val),_type(t) {
    _dval = val;
}

Term::Term(Type t,Quote* val):_type(t) {
    _qval = val;
}

Term::Term(Type t,VFrame* val):_type(t) {
    _fval = val;
}

int Term::size() {
    if (_type != TQuote)
        return 1;
    return ((QuoteStream*)qvalue()->tokens())->size();
}

Type Term::type() {
    return _type;
}

char buffer[1024];
char* Term::value() {
    //TSymbol, TQuote, TFrame, TString, TInt, TDouble, TChar,TBool,
    switch(_type) {
        case TInt:
            std::sprintf(buffer,"%d", ivalue());
            break;
        case TDouble:
            std::sprintf(buffer,"%f", dvalue());
            break;
        case TSymbol:
            return svalue();
        case TString:
            return svalue();
        case TChar:
            buffer[0] = cvalue();
            buffer[1] = 0;
            break;
        case TBool:
            return (char*) (bvalue() ? "true": "false");
        case TQuote:
            return qvalue()->to_s();
        case TFrame:
            return fvalue()->to_s();
        default:
            return "<default>";
    }
    return buffer;
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

Num Term::numvalue() {
    return _num;
}
bool Term::bvalue() {
    return _bval;
}
