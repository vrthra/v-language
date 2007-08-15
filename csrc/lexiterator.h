#ifndef LEXITERATOR_H
#define LEXITERATOR_H
#include "quoteiterator.h"
class Token;
class Lexer;
class QuoteStream;
class CharStream;
class LexIterator : public QuoteIterator {
    public:
        LexIterator(CharStream* cs);
        virtual bool hasNext();
        virtual Token* next();
    private:
        Token* lex_next();
        Token* compound(Token* open);
        P<Lexer> _lex;
        P<Token> _current;
};
#endif
