package v;

import java.util.Stack;

public interface Quote {

    /* Try and fetch the definition of a symbol in the current scope.
     * If not found in the current scope, look it up in parent scope.
     * */
    public abstract Quote lookup(String key);

    public abstract Stack<Term> stack();

    /* Evaluate the current stack
     * logic:
     * 		Check if our stack has any applicable tokens (symbols).
     * if we have, apply it on the stack. if not, get next from the
     * tokenstream and repeat the procedure. If it is a compound '['
     * then push the entire quote rather than the first one.
     * */
    public abstract void eval(Quote parent);

    public abstract Quote parent();

    public abstract void def(String sym, Quote q);

    public abstract TokenStream tokens();
}
