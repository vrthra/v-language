#ifndef LEXER_H
#define LEXER_H
#include <vector>
#include <stack>
#include <queue>
class CharStream;
class Term;
class Lexer {
    public:
        Lexer(CharStream* q);
        void lex();
        void reset();
        bool closed();
        void dump();
        bool hasNext();
        Term* next();
        static char closeCompound(char c);
    private:
        bool isStringBoundary(char c);
        bool isCompoundBoundary(char c);
        bool isPunctuation(char c);
        bool isWhitespace(char c);
        bool isBoundary(char c);
        void add(Term* term);
        char charconv(char n);
        void copen();
        void cclose();
        void lcomment();
        void string();
        void space();
        void character();
        void word();

        std::vector<char>* _word;
        std::stack<char> _cstack;
        std::queue<Term*> _queue;
        CharStream* _stream;
        bool _has;
};
#endif
