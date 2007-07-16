#include "quotestream.h"
#include "quoteiterator.h"
QuoteStream::QuoteStream() {
}
void QuoteStream::add(Token* t) {
    _stream.push_back(t);
}

int QuoteStream::size() {
    return _stream.size();
}

Token* QuoteStream::get(int i) {
    return _stream[i];
}

TokenIterator* QuoteStream::iterator() {
    return (TokenIterator*)new QuoteIterator(this);
}
