#ifndef LEXITERATOR_H
#define LEXITERATOR_H
class Token;
class Lexer;
class QuoteStream;
class CharStream;
#include "quoteiterator.h"
class LexIterator : public QuoteIterator {
    public:
        LexIterator(QuoteStream* qs, CharStream* cs);
        virtual bool hasNext();
        virtual Token* next();
    private:
        Token* lex_next();
        Token* compound(Token* open);
    private:
        Lexer* _lex;
        Token* _current;
};
#endif
