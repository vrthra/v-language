package v;

import java.util.*;

/* The world is a quote
 * */

public class CmdQuote implements Quote {
    /* The quote contains the dict which contains the currently
     * defined words, a Token stream, a current stack
     * */

    // The quote of the parent scope during creation. Used for lookup purposes
    Quote _parent = null;

    // Our bindings
    HashMap<String, Quote> _dict = null;

    // Our symbols to evaluate.
    TokenStream _tokens = null;

    // Our stack.
    QStack _stack = null;
    
    public void init(TokenStream tokens, Quote parent, HashMap<String,Quote> env) {
        _tokens = tokens;
        _tokens.scope(this); // set the scope for any future quotes
        _parent = parent;
        _idcount++;
        _id = _idcount;
        V.debug("Creating " + id() + " parent is " + _parent.id());
        _dict = env;
    }

    public CmdQuote(TokenStream tokens, Quote parent, HashMap<String,Quote> env) {
        init(tokens, parent, env);
    }

    public CmdQuote(TokenStream tokens, Quote parent) {
        // we capture the parent at creation time.
        HashMap<String, Quote> env = new HashMap<String, Quote>();
        init(tokens, parent, env);
    }

    public Quote clone() {
        return new CmdQuote(_tokens,_parent);
    }

    static int _idcount = 0;
    int _id;

    public String id() {
        return "CmdQuote[" + _id + "]";
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

    public HashMap<String, Quote> bindings() {
        return _dict;
    }

    public QStack stack() {
        return _stack;
    }

    private boolean cando() {
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
    public void eval(Quote scope) {
        eval(scope, false);
    }

    public void eval(Quote scope, boolean on_parent) {
        _stack = scope.stack();
        Iterator<Term> stream = _tokens.iterator();
        while(stream.hasNext()) {
            _stack.push(stream.next());
            if (cando())
                dofunction(on_parent ? scope : this);
        }
    }

    Object _pres = null;
    @SuppressWarnings ("unchecked")
    public void dofunction(Quote scope) {
        try {
            // pop the first token in the stack
            Token sym = _stack.pop();
            if (sym.type() != Type.TSymbol)
                throw new VException("err:not_symbol "+sym.value(),"Not a symbol");
            Quote q = scope.lookup(sym.value());
            if (q == null) {
                throw new VException("err:undef_symbol "+sym.value(),"Undefined symbol");
            }
            q = q.clone();
            // Invoke the quote on our quote by passing us as the parent.
            V.debug("Using " + scope.id() + " val " + sym.value() );
            try {
                q.eval(scope);
            } catch (VException e) {
                throw new VException(e, sym.value() );
            }
        } catch (VException e) {
            // do we have a $shield defined?
            Cmd q = (Cmd)_dict.get("$shield");
            if (q == null)
                throw e;
            Stack<Shield> stack = (Stack<Shield>)q.store().get("$info");
            if (stack.empty())
                throw e;
            Shield current = stack.pop();

            while (current != null) {
                // apply shield
                scope.stack().push(new Term<Quote>(Type.TQuote, e.quote(scope)));
                current.quote.eval(scope);
                if(_stack.pop().bvalue()) {
                    //restore the stack and continue.
                    _stack.now(current.stack);
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

    public void walk() { // no tokens are in lex stream.
        while(cando())
            dofunction(this);
    }

    V _v = null;
    public void setout(V v) {
        _v = v;
    }

    public void def(String sym, Quote q) {
        /*if (_dict.containsKey(sym) && V.pure)
            throw new VException("err:pure:redefine "+sym,"Attempt to redefine " + sym);*/
        _dict.put(sym, q);
    }

    public Quote parent() {
        return _parent;
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
