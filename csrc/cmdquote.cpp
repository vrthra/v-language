#include "cmdquote.h"
#include "vframe.h"
#include "vstack.h"
#include "quotestream.h"

void CmdQuote::eval(VFrame* scope) {
    VStack* stack = scope->stack();
    QuoteStream::iterator stream = _tokens->begin();
    while(stream != _tokens->end()) {
        stack->push(*stream);
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
