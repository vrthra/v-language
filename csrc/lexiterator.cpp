#include "lexiterator.h"
#include "quotestream.h"
#include "lexer.h"
#include "token.h"
#include "term.h"
#include "cmdquote.h"
#include "vexception.h"
LexIterator::LexIterator(CharStream* cs)
    :QuoteIterator(0),_lex(new (collect) Lexer(cs)),_current(0) {}

bool LexIterator::hasNext() {
    if (!_current)
        _current = _lex->next();
    if (!_current)
        return false;
    return true;
}
Token* LexIterator::next() {
    Token_ t = lex_next();
    if (t->type() == TOpen)
        return compound(t);
    return t;
}
Token* LexIterator::lex_next() {
    if (_current) {
        Token_ t = _current;
        _current = 0;
        return t;
    }
    return _lex->next();
}
Token* LexIterator::compound(Token* open) {
    QuoteStream_ local = new (collect) QuoteStream();
    while(true) {
        Token_ t = lex_next();
        if (!t)
            throw VSynException("err:lex:close","Compound not closed");
        if (t->type() == TClose)
            if (t->cvalue() == Lexer::closeCompound(open->cvalue()))
                break;
        if (t->type() == TOpen)
            local->add(compound(t));
        else
            local->add(t);
    }
    CmdQuote_ cq = new (collect) CmdQuote(local);
    return new (collect) Term(TQuote, cq);
}
