#include <sstream>
#include "cmdquote.h"
#include "vframe.h"
#include "vstack.h"
#include "cmd.h"
#include "quotestream.h"
#include "lexstream.h"
#include "buffcharstream.h"
#include "quoteiterator.h"
#include "vexception.h"
#include "prologue.h"

CmdQuote::CmdQuote(TokenStream* tokens):_tokens(tokens),_val(0) {
}

void CmdQuote::eval(VFrame* scope) {
    VStack_ stack = scope->stack();
    TokenIterator_ stream = _tokens->iterator();
    while(stream->hasNext()) {
        stack->push(stream->next());
        if (cando(stack))
            dofunction(scope);
    }
}

TokenStream* CmdQuote::tokens() {
    return _tokens;
}

void CmdQuote::dofunction(VFrame* scope) {
    Token_ sym = scope->stack()->pop();
    Quote_ q = scope->lookup(sym->svalue());
    if (!q)
        throw VException("err:undef_symbol", sym, sym->value());
    try {
        VFrame_ s = scope->child(); // kludge for keeping a ref.
        q->eval(s);
        gc();
    } catch (VException& e) {
        e.addLine(sym->value());
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
    if (!_val) {
        std::ostringstream outs;
        outs << '[';
        TokenIterator_ i = _tokens->iterator();
        while(i->hasNext()) {
            outs << i->next()->value();
            if (i->hasNext())
                outs << ' ';
        }
        outs << ']';
        _val = dup_str(outs.str().c_str());
    }
    return _val;
}

Quote* CmdQuote::getdef(char* buf) {
    return new (collect) CmdQuote(
            new (collect) LexStream(new (collect) BuffCharStream(buf)));
}
