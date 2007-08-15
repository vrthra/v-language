#include "lexstream.h"
#include "lexiterator.h"

LexStream::LexStream(CharStream* c) : _stream(c) {
}
TokenIterator* LexStream::iterator() {
    return new (collect) LexIterator(_stream);
}
