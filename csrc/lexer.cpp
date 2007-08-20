#include <map>
#include <ctype.h>
#include <stdlib.h>
#include "lexer.h"
#include "type.h"
#include "term.h"
#include "charstream.h"
#include "vexception.h"
#include "v.h"
#include "sym.h"

// we need the cmp_str here since the pointers in the buffer
// is different from the const string.


Lexer::Lexer(CharStream* q):_wi(0),_cstack(0),_queue(0),_first(0)
                            ,_stream(q),_has(true) {
    _stream->lexer(this);
    _first = _queue = new (collect) Node(0);
    _cstack = new (collect) CNode(0);
}

void Lexer::lex() {
    // Use the read to fetch the values.
    // loop on each char, and add the found
    // symbols to the end of queue.
    char c = _stream->read();
    switch (c) {
        case '#':
            lcomment();
            break;
        case ' ':
        case '\t':
        case '\n':
        case '\r':
        case '\f':
        case '\b':
            space();
            break;

            //string
        case '"':
        case '`':
        case '\'':
            string();
            break;

        case '~':
            character();
            break;

            //compound.
        case '[':
        case '(':
        case '{':
            copen();
            break;

        case ']':
        case ')':
        case '}':
            cclose();
            break;
        case 0:
            _has = false;
            break;
        default: // word fetch until the next space.
            if (c >= 32 && c <= 126)
                word();
    }
}

bool Lexer::closed() {
    return _cstack->link == 0;
}
void Lexer::dump() {
    Node_ i = _first->link;
    while(i) {
        V::out("%s;", i->data.val);
        i = i->link;
    }
    V::outln("");
}
bool Lexer::hasNext() {
    return _has;
}
Token* Lexer::next() {
    if (!hasNext()) return 0;
    // do we have any thing on the stack?
    // if we have return it from there.
    // else run lex and try again.
    if (_first->link != 0) {
        Token_ t = _first->link->data;
        _first = _first->link;
        return t;
    } else {
        lex();
    }
    return next();
}
bool Lexer::isWhitespace(char c) {
    if (c == ' ' || c =='\n' || c == '\r' || c == '\t')
        return true;
    return false;
}
bool Lexer::isStringBoundary(char c) {
    if (c == '"' || c== '\'' || c =='`') 
        return true;
    return false;
}
bool Lexer::isCompoundBoundary(char c) {
    if (c == '[' || c == ']' || c == '(' || c == ')' || c == '{' || c == '}' )
        return true;
    return false;
}
bool Lexer::isPunctuation(char c) {
    return false;
}
bool Lexer::isBoundary(char c) {
    if (c == 0)
        return true;
    if (isWhitespace(c))
        return true;
    if (isStringBoundary(c))
        return true;
    if (isCompoundBoundary(c))
        return true;
    if (isPunctuation(c))
        return true;
    return false;
}
void Lexer::add(Token* term) {
    _queue->link = new (collect) Node(term);
    _queue = _queue->link;
}
char Lexer::charconv(char n) {
    switch(n) {
        case 't':
            return '\t';
        case 'n':
            return '\n';
        case 'r':
            return '\r';
        case 'f':
            return '\f';
        case 'b':
            return '\b';
        default: 
            return n;
    }
}
char Lexer::closeCompound(char c) {
    switch(c) {
        case '{':
            return '}';
        case '(':
            return ')';
        case '[':
            return ']';
        default:
            throw VSynException("err:internal:invalid_open","Invalid open ");
    }
}
void Lexer::copen() {
    char c = _stream->current();

    CNode_ t = new (collect) CNode(closeCompound(c));
    t->link = _cstack;
    _cstack = t;

    add(new (collect) Term(TOpen, c));
}
void Lexer::cclose() {
        if (!_cstack->link)
            throw VSynException("err:internal:invalid_close","Invalid close - No open statement.");
        char c = _cstack->c;
        _cstack = _cstack->link;

        if (c != _stream->current())
            throw VSynException("err:internal:invalid_close","Invalid close  - Need close");
        add(new (collect) Term(TClose, _stream->current()));
}
void Lexer::lcomment() {
    while((_stream->read() != '\n') && (!_stream->eof()));
}
void Lexer::string() {
    char start = _stream->current();
    // look for unescaped end same as start.
    char c;
    while (true) {
        c = _stream->read();
        if (c == '\\') { // escaped.
            // read next char and continue.
            _word[_wi++] = charconv(_stream->read());
            continue;
        } else if (c == 0)
            break;
        if (start == c)
            break;
        _word[_wi++] = c;
    }
    _word[_wi++] = '\0';
    add(new (collect) Term(TString, dup_str(_word)));
    _wi = 0;
}
void Lexer::space() {
    while(true) {
        char c = _stream->peek();
        if (c != '\n' && isWhitespace(c))
            _stream->read();
        else
            break;
    }
}
void Lexer::character() {
    add(new (collect) Term(TChar, _stream->read()));
}
bool isint(char* v) {
    if (*v == '-') ++v;
    if(!strlen(v))
        return false;
    while(*v) {
        if (!isdigit(*v))
            return false;
        ++v;
    }
    return true;
}
bool isfloat(char* v) {
    if (*v == '-') ++v;
    int len = strlen(v);
    if (len < 2)
        return false;
    char* one = strchr(v, '.');
    if (!one)
        return false;
    *one = '1';
    bool res = isint(v);
    *one = '.';
    return res;
}
void Lexer::word() {
    _word[_wi++] = _stream->current();
    while (!isBoundary(_stream->peek()))
        _word[_wi++] = _stream->read();

    // does it look like a number?
    _word[_wi++] = '\0';

    if (isint(_word))
        add(new (collect) Term(TInt, atol(_word)));
    else if (isfloat(_word))
        add(new (collect) Term(TDouble, atof(_word)));
    else
        add(new (collect) Term(TSymbol, Sym::lookup(_word)));
    _wi = 0;
}


