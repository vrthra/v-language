package v;
import java.util.*;
public class Prologue {

    // accepts a quote as an argument.
    private static Cmd _def = new Cmd() {
        @SuppressWarnings("unchecked")
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.pop();
            Iterator<Term> it = t.qvalue().tokens().iterator();
            Term<String> symbol = it.next();

            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            while (it.hasNext())
                nts.add(it.next());
            // we define it on the parent.
            q.def(symbol.val, new CmdQuote(nts));
        }
    };

    private static Cmd _true = new Cmd() {
        public void eval(Quote q) {
            Term t = new Term<Boolean>(Type.TBool, true);
            q.stack().push(t);
        }
    };

    private static Cmd _false = new Cmd() {
        public void eval(Quote q) {
            Term t = new Term<Boolean>(Type.TBool, false);
            q.stack().push(t);
        }
    };

    // Control structures
    private static Cmd _if = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();

            Term action = p.pop();
            Term cond = p.pop();

            // execute the cond on the stack first.
            if (cond.type == Type.TQuote) {
                cond.qvalue().eval(q);
                // and get it back from stack.
                cond = p.pop();
            }
            // dequote the action and push it to stack.
            if (cond.bvalue())
                action.qvalue().eval(q);
        }
    };

    private static Cmd _ifte = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();

            Term eaction = p.pop();
            Term action = p.pop();
            Term cond = p.pop();

            if (cond.type == Type.TQuote) {
                cond.qvalue().eval(q);
                // and get it back from stack.
                cond = p.pop();
            }
            // dequote the action and push it to stack.
            if (cond.bvalue())
                action.qvalue().eval(q);
            else
                eaction.qvalue().eval(q);
        }		
    };

    // Libraries
    private static Cmd _print = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.pop();
            V.out(t.value());
        }
    };

    private static Cmd _println = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.pop();
            V.outln(t.value());
        }
    };

    private static Cmd _peek = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            if (p.empty()) {
                V.outln("");
            } else {
                Term t = p.peek();
                V.outln(t.value());
            }
        }
    };

    private static Cmd _show = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            if (p.empty()) {
                V.outln("");
            } else {
                for(Term t: p)
                    V.outln(t.value());
            }
        }
    };

    private static Cmd _dup = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.peek();
            p.push(t);
        }
    };

    private static Cmd _swap = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term x = p.pop();
            Term y = p.pop();
            p.push(x);
            p.push(y);
        }
    };

    private static Cmd _lroll = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.pop();
            Iterator<Term> it = t.qvalue().tokens().iterator();
            Term first = it.next();

            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            while (it.hasNext())
                nts.add(it.next());
            nts.add(first);
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };

    private static Cmd _rroll = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.pop();
            Iterator<Term> it = t.qvalue().tokens().iterator();
        
            List<Term> lst = new LinkedList<Term>();
            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            while (it.hasNext())
                lst.add(it.next());

            nts.add(lst.remove(lst.size() -1));

            for(Term e: lst)
                nts.add(e);
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };

    private static Cmd _map = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();

            Term action = p.pop();
            Term list = p.pop();

            Iterator<Term> fstream = list.qvalue().tokens().iterator();

            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            while (fstream.hasNext()) {
                // extract the relevant element from list,
                Term t = fstream.next();
                // push it on our current stack
                p.push(t);
                // apply the action
                action.qvalue().eval(q);
                // pop it back into a new quote
                Term res = p.pop();
                nts.add(res);
            }
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };

    private static Cmd _rev = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();

            Term list = p.pop();

            Iterator<Term> fstream = list.qvalue().tokens().iterator();
            Stack<Term> st = new Stack<Term>();
            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            while (fstream.hasNext())
                st.push(fstream.next());
           
            while(!st.empty()) 
                nts.add(st.pop());
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };



    private static Cmd _concat = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();

            Term next = p.pop();
            Term first = p.pop();
            // dequote both, append and push it back to stack.
            Iterator<Term> fstream = first.qvalue().tokens().iterator();
            Iterator<Term> nstream = next.qvalue().tokens().iterator();

            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            while (fstream.hasNext())
                nts.add(fstream.next());
            while (nstream.hasNext())
                nts.add(nstream.next());
            // we define it on the parent.
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };

    private static Cmd _dequote = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();

            Term first = p.pop();
            // dequote both, append and push it back to stack.
            Iterator<Term> fstream = first.qvalue().tokens().iterator();

            // copy the rest of tokens to our own stream.
            while (fstream.hasNext())
                p.push(fstream.next());
        }
    };


    private static Cmd _add = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                if (b.type == Type.TInt)
                    p.push(new Term<Integer>(Type.TInt, a.ivalue() + b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Float>(Type.TFloat, a.ivalue() + b.fvalue()));
                else
                    throw new VException("Type error(+)");
            } else if (a.type == Type.TFloat) {
                if (b.type == Type.TInt)
                    p.push(new Term<Float>(Type.TFloat, a.fvalue() + b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Float>(Type.TFloat, a.fvalue() + b.fvalue()));
                else
                    throw new VException("Type error(add)");
            }
        }
    };

    private static Cmd _sub = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                if (b.type == Type.TInt)
                    p.push(new Term<Integer>(Type.TInt, a.ivalue() - b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Float>(Type.TFloat, a.ivalue() - b.fvalue()));
                else
                    throw new VException("Type error(+)");
            } else if (a.type == Type.TFloat) {
                if (b.type == Type.TInt)
                    p.push(new Term<Float>(Type.TFloat, a.fvalue() - b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Float>(Type.TFloat, a.fvalue() - b.fvalue()));
                else
                    throw new VException("Type error(add)");
            }
        }
    };

    private static Cmd _mul = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                if (b.type == Type.TInt)
                    p.push(new Term<Integer>(Type.TInt, a.ivalue() * b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Float>(Type.TFloat, a.ivalue() * b.fvalue()));
                else
                    throw new VException("Type error(+)");
            } else if (a.type == Type.TFloat) {
                if (b.type == Type.TInt)
                    p.push(new Term<Float>(Type.TFloat, a.fvalue() * b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Float>(Type.TFloat, a.fvalue() * b.fvalue()));
                else
                    throw new VException("Type error(add)");
            }
        }
    };

    private static Cmd _div = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                if (b.type == Type.TInt)
                    p.push(new Term<Float>(Type.TFloat, (float)(a.ivalue() / (1.0 * b.ivalue()))));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Float>(Type.TFloat, a.ivalue() / b.fvalue()));
                else
                    throw new VException("Type error(+)");
            } else if (a.type == Type.TFloat) {
                if (b.type == Type.TInt)
                    p.push(new Term<Float>(Type.TFloat, a.fvalue() / b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Float>(Type.TFloat, a.fvalue() / b.fvalue()));
                else
                    throw new VException("Type error(add)");
            }
        }
    };

    private static Cmd _gt = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                if (b.type == Type.TInt)
                    p.push(new Term<Boolean>(Type.TBool, a.ivalue() > b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Boolean>(Type.TBool, a.ivalue() > b.fvalue()));
                else
                    throw new VException("Type error(div)");
            } else if (a.type == Type.TFloat) {
                if (b.type == Type.TInt)
                    p.push(new Term<Boolean>(Type.TBool, a.fvalue() > b.ivalue()));
                else if (b.type == Type.TFloat)
                    p.push(new Term<Boolean>(Type.TBool, a.fvalue() > b.fvalue()));
                else
                    throw new VException("Type error(div)");
            }
        }
    };	

    public static void init(Quote q) {
        //meta
        q.def(".", _def);
        q.def("true", _true);
        q.def("false", _false);
        
        //control structures
        q.def("ifte", _ifte);
        q.def("if", _if);

        //io
        q.def("print", _print);
        q.def("println", _println);
        
        //others
        q.def("?", _peek);
        q.def("??", _show);

        q.def("dup", _dup);
        q.def("swap", _swap);
        q.def("lroll", _lroll);
        q.def("rroll", _rroll);
        q.def("map", _map);
        q.def("rev", _rev);
        q.def("concat", _concat);
        q.def("i", _dequote);

        //arith
        q.def("+", _add);
        q.def("-", _sub);
        q.def("*", _mul);
        q.def("/", _div);

        //bool
        q.def(">?", _gt);
    }
}

