package v;

import java.util.*;

public class V {
    public static String version = "0.003";

    static void banner() {
        outln("\t|V|\t");
    }


    public static void main(final String[] args) {
        final VFrame frame = new VFrame(); // our frame chain.
        for(String s : args)
            frame.stack().push(new Term<String>(Type.TString, s));
        final boolean interactive = args.length == 0 ? true : false;
        // Setup the world quote.

        Prologue.init(frame);
        try {
            // do we have any args?
            CharStream cs = null;
            if (args.length > 0) {
                debug("Opening:" + args[0]);
                cs = new FileCharStream(args[0]);
            } else {
                banner();
                cs = new ConsoleCharStream();
            }

            CmdQuote program = new CmdQuote(new LexStream(cs)) {
                public void dofunction(VFrame scope) {
                    if (interactive) {
                        try {
                            super.dofunction(scope);
                        } catch (Exception e) {
                            outln(">" + e.getMessage());
                            frame.dump();
                            frame.reinit();
                            V.debug(e);
                        }
                    } else super.dofunction(scope);
                }
            };
            program.eval(frame.child()); // we save the original defs.
        } catch (Exception e) {
            outln(e.getMessage());
            debug(e);
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

    public static void debug(Exception e) {
        if (_debug)
            e.printStackTrace();
    }
    public static void debug(String s) {
        if (_debug) outln(s);
    }

    static boolean _debug = false;
    static void debug(boolean val) {
        _debug = val;
    }
}
