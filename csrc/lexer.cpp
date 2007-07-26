#include "lexer.h"
#include "type.h"
#include "term.h"
#include "charstream.h"
#include "vexception.h"
#include "v.h"
Lexer::Lexer(CharStream* q) {
    _stream = q;
    _stream->lexer(this);
    _word = new std::vector<char>();
    _has = true;
}

void Lexer::lex() {
    // Use the read to fetch the values.
    // loop on each char, and add the found
    // symbols to the end of queue.
    // _word->clear();
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
void Lexer::reset() {
    _queue.erase(_queue.begin(), _queue.end());
}
bool Lexer::closed() {
    return _cstack.empty();
}
void Lexer::dump() {
    for (std::list<Term*>::iterator i = _queue.begin(); i != _queue.end(); i++) {
        V::out("%s;", *i);
    }
    V::outln("");
}
bool Lexer::hasNext() {
    return _has;
}
Term* Lexer::next() {
    if (!hasNext()) return 0;
    // do we have any thing on the stack?
    // if we have return it from there.
    // else run lex and try again.
    if (_queue.size() != 0) {
        Term* t = _queue.front();
        _queue.pop_front();
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
void Lexer::add(Term* term) {
    _queue.push_back(term);
    //_word->clear();
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
    _cstack.push(closeCompound(c));
    add(new Term(TOpen, c));
}
void Lexer::cclose() {
        if (_cstack.empty())
            throw VSynException("err:internal:invalid_close","Invalid close - No open statement.");
        char c = _cstack.top();
        _cstack.pop();
        if (c != _stream->current())
            throw new VSynException("err:internal:invalid_close","Invalid close  - Need close");
        add(new Term(TClose, _stream->current()));
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
            _word->push_back(charconv(_stream->read()));
            continue;
        } else if (c == 0)
            break;
        if (start == c)
            break;
        _word->push_back(c);
    }
    _word->push_back('\0');
    add(new Term(TString, (char*)&(*_word)[0]));
    _word = new std::vector<char>();
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
    add(new Term(TChar, _stream->read()));
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
    _word->push_back(_stream->current());
    while (!isBoundary(_stream->peek()))
        _word->push_back(_stream->read());

    // does it look like a number?
    _word->push_back('\0');
    char* word = (char*)&(*_word)[0];

    if (isint(word))
        add(new Term(TInt, atol(word)));
    else if (isfloat(word))
        add(new Term(TDouble, atof(word)));
    else
        add(new Term(TSymbol, word));
    _word = new std::vector<char>();
}
