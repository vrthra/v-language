package v;

import java.util.*;

public class V {

    static void banner() {
        outln("\t|V|\t");
    }

    public static void main(final String[] args) {
        banner();
        // Setup the world quote.
        Quote world = new Quote() {
            HashMap<String, Quote> _dict = new HashMap<String, Quote>();
            public Stack<Term> stack() {
                Stack<Term> st = new Stack<Term>();
                for(String s : args)
                    st.push(new Term<String>(Type.TString, s));
                return st;
            }

            public void eval(Quote parent) {
                throw new VException("Attempt to eval world.");
            }

            public Quote lookup(String key) {
                return _dict.get(key);
            }

            public Quote parent() {
                throw new VException("world does not have a parent.");
            }

            public TokenStream tokens() {
                throw new VException("world does not have a token stream.");
            }

            public void def(String sym, Quote q) {
                _dict.put(sym, q);
            }


        };
        Prologue.init(world);

        CmdQuote program = new CmdQuote(new LexStream());
        program.setout(new V());
        program.eval(world);
    }

    public static void outln(String var) {
        System.out.println(var);
    }

    public static void out(String var) {
        System.out.print(var);
    }

    @SuppressWarnings("unchecked")
    public void outln(Term term) {
        outln(term.value());
    }
}
