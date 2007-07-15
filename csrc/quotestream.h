#ifndef QUOTESTREAM_H
#define QUOTESTREAM_H
#include "tokenstream.h"
class QuoteStream : public TokenStream {
    public:
    QuoteStream();
    void add(Term* t);
    int size();
    // need to replace with a true iterator
};
#endif
