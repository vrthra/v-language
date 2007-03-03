package v;

public class LexIterator extends QuoteIterator {
    // Console Iterator knows about compound quotes
    Lexer _lex = null;
    public LexIterator(QuoteStream qs, CharStream cs) {
        super(qs);
        _lex = new Lexer(cs);
    }

    Term _current = null;
    public boolean hasNext() {
        if (_current == null)
            _current = _lex.next();
        if (_current == null)
            return false;
        return true;
    }

    Term lex_next() {
        if (_current != null) {
            Term t = _current;
            _current = null;
            return t;
        }
        return _lex.next();
    }

    @SuppressWarnings("unchecked")
    public Term next() {
        Term t = lex_next();
        if (t.type == Type.TOpen)
            return compound(t);
        return t;
    }

    @SuppressWarnings({"unchecked"})
    private Term compound(Term<Character> open) {
        QuoteStream local = new QuoteStream();
        while(true) {
            Term t = lex_next();
            if (t == null)
                throw new VException("Still expecting compound to close.");
            if (t.type == Type.TClose) {
                Term<Character> c = t;
                if (c.val == Lexer.closeCompound(open.val))
                    break;
            }
            if (t.type == Type.TOpen) {
                local.add(compound(t));
            } else {
                local.add(t);
            }
        }

        CmdQuote cq = new CmdQuote(local, _qs.scope());
        return new Term<Quote>(Type.TQuote, cq);
    }

    public void remove() {
        throw new VException("Attempt to remove from QuoteIterator");
    }
}

