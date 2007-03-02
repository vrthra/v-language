package v;

import java.util.*;

public class QuoteStream implements TokenStream {
    List<Term> _terms = null;
    Quote _scope = null;
    public QuoteStream() {
        _terms = new ArrayList<Term>();
    }

    public void add(Term t) {
        _terms.add(t);
    }

    Term get(int idx) {
        return _terms.get(idx);
    }

    int size() {
        return _terms.size();
    }

    public Quote scope() {
        return _scope;
    }

    public void scope(Quote q) {
        _scope = q;
    }

    public Iterator<Term> iterator() {
        return new QuoteIterator(this);
    }
}
