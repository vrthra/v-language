package v;

public class QuoteIterator implements TokenIterator {
    QuoteStream _qs = null;
    int _index = 0;
    public QuoteIterator(QuoteStream qs) {
        // we can store the state.
        _qs = qs;
        _index = 0;
    }

    public boolean hasNext() {
        if (_qs.size() > _index)
            return true;
        return false;
    }

    public Term next() {
        return _qs.get(_index++);
    }

    public void remove() {}
}
