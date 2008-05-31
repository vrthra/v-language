package v;

import java.util.*;

public class QuoteStream implements TokenStream {
    List<Term> _terms = null;
    public QuoteStream() {
        /* We use an Array here as the quotes are immutable.
         * thus unlike lisp, we can provide O(1) access time
         * to any indexed element.
         * */
        _terms = new ArrayList<Term>();
    }

    public void add(Term t) {
        _terms.add(t);
    }

    public Term get(int idx) {
        return _terms.get(idx);
    }

    public int size() {
        return _terms.size();
    }

    public Iterator<Term> iterator() {
        return new QuoteIterator(this);
    }
}
