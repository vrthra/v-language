package v;

import java.util.*;

/* The world is a quote
 * */

public class CmdQuote implements Quote {
    /* The quote contains a Token stream
     * */

    // Our symbols to evaluate.
    TokenStream _tokens = null;

    public CmdQuote(TokenStream tokens) {
        _tokens = tokens;
    }

    private boolean cando(VStack stack) {
        if (stack.empty())
            return false;
        if (stack.peek().type == Type.TSymbol)
            return true;
        return false;
    }

    /* Evaluate the current stack
     * logic:
     * 		Check if our stack has any applyabl tokens (symbols).
     * if we have, apply it on the stack. if not, get next from the
     * tokenstream and repeat the procedure. If it is a compound '['
     * then push the entire quote rather than the first one.
     * */

    public void eval(VFrame scope) {
        VStack stack = scope.stack();
        Iterator<Term> stream = _tokens.iterator();
        while(stream.hasNext()) {
            stack.push(stream.next());
            if (cando(stack))
                dofunction(scope);
        }
    }

    public void dofunction(VFrame scope) {
        VStack st = scope.stack();
        // pop the first token in the stack
        Token sym = st.pop();
        Quote q = scope.lookup(sym.value());
        if (q == null)
            throw new VException("err:undef_symbol",sym, sym.value());
        // Invoke the quote on our quote by passing us as the parent.
        try {
            q.eval(scope.child());
        } catch (VException e) {
            e.addLine(sym.value());
            throw e;
        }
    }

    public TokenStream tokens() {
        return _tokens;
    }

    public String toString() {
        StringBuffer sb = new StringBuffer();
        sb.append("[");
        Iterator<Term> i = _tokens.iterator();
        while(i.hasNext()) {
            sb.append(i.next().value());
            if (i.hasNext())
                sb.append(" ");
        }
        sb.append("]");
        return sb.toString();
    }

    HashMap<String, Object> _store = new HashMap<String, Object>();
    public HashMap<String, Object> store() {
        return _store;
    }
}
