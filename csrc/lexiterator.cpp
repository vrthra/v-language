#include "lexiterator.h"
#include "quotestream.h"
#include "lexer.h"
#include "token.h"
#include "term.h"
#include "cmdquote.h"
#include "vexception.h"
LexIterator::LexIterator(QuoteStream* qs, CharStream* cs)
    :QuoteIterator(qs),_lex(new Lexer(cs)) {}

bool LexIterator::hasNext() {
    if (!_current)
        _current = (Token*)_lex->next();
    if (!_current)
        return false;
    return true;
}
Token* LexIterator::next() {
    Token* t = lex_next();
    if (t->type() == TOpen)
        return compound(t);
    return t;
}
Token* LexIterator::lex_next() {
    if (!_current) {
        Token* t = _current;
        _current = 0;
        return t;
    }
    return (Token*)_lex->next();
}
Token* LexIterator::compound(Token* open) {
    QuoteStream* local = new QuoteStream();
    while(true) {
        Token* t = lex_next();
        if (!t)
            throw new VSynException("err:lex:close","Compound not closed");
        if (t->type() == TClose)
            if (t->cvalue() == Lexer::closeCompound(open->cvalue()))
                break;
        if (t->type() == TOpen)
            local->add(compound(t));
        else
            local->add(t);
    }
    CmdQuote* cq = new CmdQuote(local);
    return (Token*)new Term(TQuote, cq);
}
