#include "quoteiterator.h"
#include "quotestream.h"

QuoteIterator::QuoteIterator(QuoteStream* q):_qs(q),_index(0) {}

bool QuoteIterator::hasNext() {
    if (_qs->size() > _index)
        return true;
    return false;
}

Token* QuoteIterator::next() {
    return (Token*)_qs->get(_index++);
}
