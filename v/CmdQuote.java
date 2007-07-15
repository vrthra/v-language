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

    @SuppressWarnings ("unchecked")
    public void dofunction(VFrame scope) {
        VStack st = scope.stack();
        try {
            // pop the first token in the stack
            Token sym = st.pop();
            if (sym.type() != Type.TSymbol)
                throw new VException("err:not_symbol "+sym.value(),"Not a symbol");
            Quote q = scope.lookup(sym.value());
            if (q == null)
                throw new VException("err:undef_symbol ("+sym.value() +")","Undefined symbol");
            // Invoke the quote on our quote by passing us as the parent.
            V.debug("Using " + scope.id() + " val " + sym.value() );
            try {
                q.eval(scope.child());
            } catch (VException e) {
                throw new VException(e, sym.value() );
            }
        } catch (VException e) {
            // do we have a $shield defined?
            // the scope we get is the child scope of executing environment.
            // so we take the parent since we are accessing dict directly.
            V.debug("Shield?" + scope.id() + "|" + e.getMessage());
            Cmd q = (Cmd)scope.dict().get("$shield");
            if (q == null)
                throw e;
            Stack<Shield> stack = (Stack<Shield>)q.store().get("$info");
            if (stack.empty())
                throw e;
            Shield current = stack.pop();

            while (current != null) {
                // apply shield
                scope.stack().push(new Term<Quote>(Type.TQuote, e.quote()));
                current.quote.eval(scope);
                if(st.pop().bvalue()) {
                    //restore the stack and continue.
                    st.now(current.stack);
                    return;
                }
                if (!stack.empty())
                    current = stack.pop();
                else
                    current = null;
            }
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
