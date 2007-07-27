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
            throw VException("err:not_symbol", sym, "%s %s", sym->value(), "Not a symbol");
        Quote* q = scope->lookup(sym->svalue());
        if (!q)
            throw VException("err:undef_symbol", sym, "%s %s", sym->value(), "Undefined Symbol");
        try {
            q->eval(scope->child());
        } catch (VException& e) {
            e.addLine(sym->value());
            throw e;
        }
    } catch (VException& e) {
        if(scope->dict().find("$shield") == scope->dict().end())
            throw e;
        Cmd* q = (Cmd*)scope->dict()["$shield"];
        if (q->store().find("$info") == q->store().end())
            throw e;
        Shield* current = q->store()["$info"];
        while(current) {
            // apply shield
            scope->stack()->push(new Term(TQuote, e.quote()));
            current->quote->eval(scope);
            if (st->pop()->bvalue()) {
                // restore the stack and continue.
                st->now(current->stack);
                return;
            }
            current = current->next;
        }
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

Quote* CmdQuote::getdef(char* buf) {
    return new CmdQuote(new LexStream(new BuffCharStream(buf)));
}
