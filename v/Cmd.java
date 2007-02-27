package v;

import java.util.Stack;

public abstract class Cmd implements Quote {

    public Quote lookup(String key) {
        throw new VException("Commands does not have a dict.");
    }

    public Stack<Term> stack() {
        throw new VException("Commands does not have a stack.");
    }

    public Quote parent() {
        throw new VException("Commands does not have a parent?.");
    }

    public void def(String sym, Quote q) {
        throw new VException("Commands can not define internal commands.");
    }

    public TokenStream tokens() {
        throw new VException("Commands can not have tokens.");
    }	
}
