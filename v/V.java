package v;

import java.util.*;

public class V {

    static QStack _stack = null;
    static void banner() {
        outln("\t|V|\t");
    }


    public static void main(final String[] args) {
        try {
            _stack = new QStack(); // our singleton eval stack.
            // Setup the world quote.
            Quote world = new Quote() {
                HashMap<String, Quote> _dict = new HashMap<String, Quote>();
                {
                    for(String s : args)
                        _stack.push(new Term<String>(Type.TString, s));
                }

                public QStack stack() {
                    return _stack;
                }

                public String id() {
                    return "Quote[world]";
                }

                public void eval(Quote scope, boolean on_parent) {
                    throw new VException("Attempt to eval world.");
                }
                public void eval(Quote scope) {
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

                public HashMap<String, Quote> bindings() {
                    return _dict;
                }

            };

            Prologue.init(world);
            // do we have any args?
            CharStream cs = null;
            if (args.length > 0) {
                debug("Opening:" + args[0]);
                cs = new FileCharStream(args[0]);
            } else {
                banner();
                cs = new ConsoleCharStream();
            }

            CmdQuote program = new CmdQuote(new LexStream(cs), world);
            program.setout(new V());
            program.eval(world);

        } catch (Exception e) {
            outln(e.getMessage());
        }
    }

    public static void outln(String var) {
        System.out.println(var);
    }

    public static void out(String var) {
        System.out.print(var);
    }

    public void outln(Term term) {
        outln(term.value());
    }

    public static void debug(String s) {
        if (_debug) outln(s);
    }

    static boolean _debug = false;
    static void debug(boolean val) {
        _debug = val;
    }
}
