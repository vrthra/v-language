package v;

import java.util.Iterator;

public class LexStream extends QuoteStream {
    CharStream _stream = null;
    public LexStream(CharStream cs) {
        _stream = cs;
    }

    // Console stream knows about compounds so does quotestream.
    public Iterator<Term> iterator() {
        return new LexIterator(this, _stream);
    }
}
