package v;

import java.util.*;

/* The world is a quote
 * */

public class CmdQuote implements Quote {
    /* The quote contains the dict which contains the currently
     * defined words, a Token stream, a current stack
     * */

    // The quote of the outside scope. Used for lookup purposes
    Quote _parent = null;

    // Our bindings
    HashMap<String, Quote> _dict = null;

    // Our symbols to evaluate.
    TokenStream _tokens = null;

    // Our stack.
    Stack<Term> _stack = null;

    public CmdQuote(TokenStream tokens) {
        _dict = new HashMap<String, Quote>();
        _tokens = tokens;
    }

    /* Try and fetch the definition of a symbol in the current scope.
     * If not found in the current scope, look it up in parent scope.
     * */
    public Quote lookup(String key) {
        // look it up ourselves.
        if (!_dict.containsKey(key))
            return _parent.lookup(key);
        return _dict.get(key);
    }

    public Stack<Term> stack() {
        return _stack;
    }

    private boolean canApply() {
        if (_stack.empty())
            return false;
        if (_stack.peek().type == Type.TSymbol)
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
    public void eval(Quote parent) {
        _parent = parent;
        _stack = parent.stack();
        Iterator<Term> stream = _tokens.iterator();
        while(true) {
            try {
            if (canApply())
                apply();
            else if (stream.hasNext())
                // TokenStream returns entire quotes as a single term
                // of type TQuote
                _stack.push(stream.next());
            else
                break;
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }

    Object _pres = null;
    public void apply() {
        // pop the first token in the stack
        Token sym = _stack.pop();
        if (sym.type() != Type.TSymbol)
            throw new VException("Attempt to apply NotSymbol");
        Quote q = lookup(sym.value());
        if (q == null)
            throw new VException("Attempt to invoke undefined word (" + sym.value()+ ")");
        // Invoke the quote on our quote by passing us as the parent.
        q.eval(this);

        if (_v!= null) {
            if (stack().size() > 0) {
                Object res = stack().peek();
                if (res != _pres) {
                    _v.outln(stack().peek());
                    _pres = res;
                }
            }
        }
    }

    V _v = null;
    public void setout(V v) {
        _v = v;
    }

    public void def(String sym, Quote q) {
        _dict.put(sym, q);
    }

    public Quote parent() {
        return _parent;
    }

    public TokenStream tokens() {
        return _tokens;
    }
}
