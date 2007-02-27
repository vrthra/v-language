package v;

public class LexIterator extends QuoteIterator {
    // Console Iterator knows about compound quotes
    Lexer _lex = null;
    public LexIterator(QuoteStream qs) {
        super(qs);
        _lex = new Lexer(new ConsoleCharStream());
    }

    public boolean hasNext() {
        return true;
    }

    @SuppressWarnings("unchecked")
        public Term next() {
            Term t = _lex.next();
            if (t.type == Type.TOpen)
                return compound(t);
            return t;
        }

    @SuppressWarnings({"unchecked"})
        private Term compound(Term<Character> open) {
            QuoteStream local = new QuoteStream();
            while(true) {
                Term t = _lex.next();
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
            CmdQuote cq = new CmdQuote(local);
            return new Term<Quote>(Type.TQuote, cq);
        }

    public void remove() {
        throw new VException("Attempt to remove from QuoteIterator");
    }
}
