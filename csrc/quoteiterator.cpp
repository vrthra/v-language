#include "quoteiterator.h"
#include "quotestream.h"

QuoteIterator::QuoteIterator(Node* q):_qs(q) {}

bool QuoteIterator::hasNext() {
    return _qs->link != 0;
}

Token* QuoteIterator::next() {
    _qs = _qs->link;
    return _qs->data;
}
