#include "term.h"
#include "quote.h"
#include "quotestream.h"
#include "vframe.h"
#include "charstream.h"
#include "vexception.h"

Term::Term(Type t,bool val):_type(t),_hold(0) {
    _bval = val;
}

Term::Term(Type t,char val):_type(t),_hold(0) {
    _cval = val;
}

Term::Term(Type t,char* val):_type(t),_hold(0) {
    _sval = val;
    _hold = val;
}

Term::Term(Type t,long val):_num(val),_type(t),_hold(0) {
    _lval = val;
}

Term::Term(Type t,double val):_num(val),_type(t),_hold(0) {
    _dval = val;
}

Term::Term(Type t,Quote* val):_type(t),_hold(0) {
    _qval = val;
    _hold = val;
}

Term::Term(Type t,VFrame* val):_type(t),_hold(0) {
    _fval = val;
    _hold = val;
}

int Term::size() {
    if (_type != TQuote)
        return 1;
    return ((QuoteStream*)qvalue()->tokens())->size();
}

Type Term::type() {
    return _type;
}

char* Term::value() {
    //TSymbol, TQuote, TFrame, TString, TInt, TDouble, TChar,TBool,
    switch(_type) {
        case TInt:
            std::sprintf(_buffer,"%d", ivalue());
            break;
        case TDouble:
            std::sprintf(_buffer,"%f", dvalue());
            break;
        case TSymbol:
        case TString:
            return svalue();
        case TChar:
            _buffer[0] = '~';
            _buffer[1] = cvalue();
            _buffer[2] = 0;
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
    return dup_str(_buffer);
}

char* Term::stype(Type t) {
    switch(t) {
        case TInt: return "<int>";
        case TDouble: return "<float>";
        case TSymbol: return "<symbol>";
        case TString: return "<string>";
        case TChar: return "<char>";
        case TBool: return "<bool>";
        case TQuote: return "<quote>";
        case TFrame: return "<frame>";
        default: return "<?>";
    }
}

void Term::checkType(Type t, Term* term) {
    if (t != term->type()) {
        switch(term->type()) {
            case TInt:
                throw VException("err:type",term, "need %s, has %s (%d)", stype(t), stype(term->type()), term->_lval);
            case TDouble:
                throw VException("err:type",term, "need %s, has %s (%f)", stype(t), stype(term->type()), term->_dval);
            case TSymbol:
                throw VException("err:type",term, "need %s, has %s (%s)", stype(t), stype(term->type()), term->_sval);
            case TString:
                throw VException("err:type",term, "need %s, has %s (%s)", stype(t), stype(term->type()), term->_sval);
            case TChar:
                throw VException("err:type",term, "need %s, has %s (%c)", stype(t), stype(term->type()), term->_cval);
            case TBool:
                throw VException("err:type",term, "need %s, has %s (%s)", stype(t), stype(term->type()), (term->_bval ? ":t" : ":f"));
            case TQuote:
                throw VException("err:type",term, "need %s, has %s (%s)", stype(t), stype(term->type()), term->_qval->to_s());
            case TFrame:
                throw VException("err:type",term, "need %s, has %s (%s)", stype(t), stype(term->type()), term->_fval->to_s());
            default:
                throw VException("err:type",term, "need %s, has %s (%d)", stype(t), stype(term->type()), term->type());
        }
    }
}

char* Term::svalue() {
    if (_type != TString && _type != TSymbol) {
        throw VException("err:type",this, "need String|Sym, has %s (%s)", stype(_type), value());
    }
    return _sval;
}

char Term::cvalue() {
    if (_type != TChar && _type != TOpen && _type != TClose) {
        throw VException("err:type",this, "need Char|[|], has %s (%s)", stype(_type), value());
    }
    return _cval;
}

Quote* Term::qvalue() {
    checkType(TQuote, this);
    return _qval;
}

VFrame* Term::fvalue() {
    checkType(TFrame, this);
    return _fval;
}

long Term::ivalue() {
    checkType(TInt, this);
    return _lval;
}

double Term::dvalue() {
    checkType(TDouble, this);
    return _dval;
}

bool Term::bvalue() {
    checkType(TBool, this);
    return _bval;
}

Num Term::numvalue() {
    if (_type != TInt && _type != TDouble) {
        throw VException("err:type",this, "need Num, has %s (%s)", stype(_type), value());
    }
    return _num;
}

