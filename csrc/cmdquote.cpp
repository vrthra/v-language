#include <sstream>
#include "cmdquote.h"
#include "vframe.h"
#include "vstack.h"
#include "quotestream.h"
#include "quoteiterator.h"
#include "vexception.h"

void CmdQuote::eval(VFrame* scope) {
    VStack* stack = scope->stack();
    TokenIterator* stream = _tokens->iterator();
    while(stream->hasNext()) {
        stack->push(stream->next());
        if (cando(stack))
            dofunction(scope);
    }
}
TokenStream* CmdQuote::tokens() {
    return _tokens;
}
CmdQuote::CmdQuote(TokenStream* tokens) {
    _tokens = tokens;
}
void CmdQuote::dofunction(VFrame* scope) {
    VStack* st = scope->stack();
    try {
        Token* sym = st->pop();
        if (sym->type()!= TSymbol)
            throw VException("err:not_symbol", "Not a symbol");
        Quote* q = scope->lookup(sym->svalue());
        if (!q)
            throw VException("err:undef_symbol", "Undefined Symbol");
        try {
            q->eval(scope->child());
        } catch (VException e) {
            throw VException(e);
        }
    } catch (VException &e) {
        // TODO:
        throw e;
    }
}
bool CmdQuote::cando(VStack* stack) {
    if (stack->empty())
        return false;
    if (stack->peek()->type() == TSymbol)
        return true;
    return false;
}

char* CmdQuote::to_s() {
    std::ostringstream outs;
    outs << '[';
    TokenIterator* i = _tokens->iterator();
    while(i->hasNext()) {
        outs << i->next()->value();
        if (i->hasNext())
            outs << ' ';
    }
    outs << ']';
    char* out = new char[outs.str().length()];
    std::strcpy(out, outs.str().c_str());
    return out;
}
