#include "quotestream.h"
#include "quoteiterator.h"
QuoteStream::QuoteStream():_first(new Node(0)),_stream(0),_size(0) {
    _stream = _first;
}
void QuoteStream::add(Token* t) {
    _stream->link = new Node(t);
    _stream = _stream->link;
    ++_size;
}

int QuoteStream::size() {
    return _size;
}

TokenIterator* QuoteStream::iterator() {
    return new QuoteIterator(_first);
}
