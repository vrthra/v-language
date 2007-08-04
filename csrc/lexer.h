#ifndef LEXER_H
#define LEXER_H
#include "common.h"
#include "token.h"
#include <stack>
class CharStream;
class Lexer {
    public:
        Lexer(CharStream* q);
        void lex();
        bool closed();
        void dump();
        bool hasNext();
        Token* next();
        static char closeCompound(char c);
    private:
        bool isStringBoundary(char c);
        bool isCompoundBoundary(char c);
        bool isPunctuation(char c);
        bool isWhitespace(char c);
        bool isBoundary(char c);
        void add(Token* term);
        char charconv(char n);
        void copen();
        void cclose();
        void lcomment();
        void string();
        void space();
        void character();
        void word();

        char _word[MaxBuf];
        int _wi;

        std::stack<char> _cstack;
        Node* _queue;
        Node* _first;

        CharStream* _stream;
        bool _has;
};
#endif
