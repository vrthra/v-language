#ifndef LEXITERATOR_H
#define LEXITERATOR_H
#include "quoteiterator.h"
class LexIterator : public QuoteIterator {
    public:
        LexIterator(CharStream* cs);
        virtual bool hasNext();
        virtual Token* next();
    private:
        Token* lex_next();
        Token* compound(Token* open);
        Lexer_ _lex;
        Token_ _current;
};
#endif
