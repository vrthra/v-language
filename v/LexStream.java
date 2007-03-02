package v;

import java.util.Iterator;

public class LexStream extends QuoteStream {
    // Console stream knows about compounds so does quotestream.
    public Iterator<Term> iterator() {
        return new LexIterator(this);
    }	
}
