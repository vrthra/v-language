#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H
class TokenIterator;
struct TokenStream {
    virtual TokenIterator* iterator()=0;
};
#endif
