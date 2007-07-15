package v;

import java.util.*;

public class QuoteStream implements TokenStream {
    List<Term> _terms = null;
    public QuoteStream() {
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
