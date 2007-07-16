#include "cmdquote.h"
#include "vframe.h"
#include "vstack.h"
#include "quotestream.h"
#include "quoteiterator.h"

void CmdQuote::eval(VFrame* scope) {
    VStack* stack = scope->stack();
    TokenIterator* stream = _tokens->iterator();
    while(stream->hasNext()) {
        stack->push((Term*)stream->next());
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
}
bool CmdQuote::cando(VStack* stack) {
    if (stack->empty())
        return false;
    if (stack->peek()->type() == TSymbol)
        return true;
    return false;
}
