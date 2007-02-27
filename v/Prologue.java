package v;
import java.util.*;
public class Prologue {

    // accepts a quote as an argument.
    private static Cmd _def = new Cmd() {
        @SuppressWarnings("unchecked")
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.pop();
            if (t.type != Type.TQuote)
                throw new VException("Invalid use of '.' on " + t.value());
            Term<Quote> tq = t;
            Quote local = tq.val;
            Iterator<Term> it = local.tokens().iterator();
            Term<String> symbol = it.next();

            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            while (it.hasNext())
                nts.add(it.next());
            // we define it on the parent.
            q.parent().def(symbol.val, new CmdQuote(nts));
        }
    };


    private static Cmd _true = new Cmd() {
        public void eval(Quote p) {
            Term t = new Term<Boolean>(Type.TBool, true);
            p.stack().push(t);
        }
    };

    private static Cmd _false = new Cmd() {
        public void eval(Quote p) {
            Term t = new Term<Boolean>(Type.TBool, false);
            p.stack().push(t);
        }
    };

    // Control structures
    private static Cmd _if = new Cmd() {
        @SuppressWarnings({"unchecked"})
        public void eval(Quote q) {
            Stack<Term> p = q.stack();

            Term action = p.pop();
            Term cond = p.pop();

            // execute the cond on the stack first.
            if (cond.type == Type.TQuote) {
                Term<Quote> tq = cond;
                Quote qv = tq.val;
                qv.eval(q);
            }

            // and get it back from stack.
            cond = p.pop();
            if (cond.type != Type.TBool)
                throw new VException("Type error if condition not a boolean");
            Term<Boolean> bc = cond;
            if (bc.val) {
                // dequote the action and push it to stack.
                Term<Quote> tq = action;
                Quote qa = tq.val;
                qa.eval(q);
            }
        }
    };

    private static Cmd _ifte = new Cmd() {
        @SuppressWarnings({"unchecked"})
        public void eval(Quote q) {
            Stack<Term> p = q.stack();

            Term action = p.pop();
            Term eaction = p.pop();
            Term cond = p.pop();

            // execute the cond on the stack first.
            if (cond.type == Type.TQuote) {
                Term<Quote> tq = cond;
                Quote qv = tq.val;
                qv.eval(q);
            }

            // and get it back from stack.
            cond = p.pop();
            if (cond.type != Type.TBool)
                throw new VException("Type error if condition not a boolean");
            Term<Boolean> bc = cond;
            if (bc.val) {
                // dequote the action and push it to stack.
                Term<Quote> tq = action;
                Quote qa = tq.val;
                qa.eval(q);
            } else {
                // dequote the action and push it to stack.
                Term<Quote> tq = eaction;
                Quote qa = tq.val;
                qa.eval(q);
            }
        }		
    };

    // Libraries
    private static Cmd _print = new Cmd() {
        @SuppressWarnings("unchecked")
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.pop();
            V.out(t.value());
        }
    };

    private static Cmd _println = new Cmd() {
        @SuppressWarnings("unchecked")
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.pop();
            V.outln(t.value());
        }
    };

    private static Cmd _dup = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term t = p.peek();
            p.push(t);
        }
    };

    @SuppressWarnings("unchecked")
    private static Integer getInt(Term t) {
        Term<Integer> v = t;
        return v.val;
    }

    @SuppressWarnings("unchecked")
    private static Float getFloat(Term t) {
        Term<Float> v = t;
        return v.val;
    }


    private static Cmd _add = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                int a1 = getInt(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Integer>(Type.TInt, a1 + getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Float>(Type.TFloat, a1 + getFloat(b)));
                } else
                    throw new VException("Type error(add)");
            } else if (a.type == Type.TFloat) {
                float a1 = getFloat(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Float>(Type.TFloat, a1 + getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Float>(Type.TFloat, a1 + getFloat(b)));
                } else
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
                int a1 = getInt(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Integer>(Type.TInt, a1 - getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Float>(Type.TFloat, a1 - getFloat(b)));
                } else
                    throw new VException("Type error(sub)");
            } else if (a.type == Type.TFloat) {
                float a1 = getFloat(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Float>(Type.TFloat, a1 - getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Float>(Type.TFloat, a1 - getFloat(b)));
                } else
                    throw new VException("Type error(sub)");
            }
        }
    };	

    private static Cmd _mul = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                int a1 = getInt(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Integer>(Type.TInt, a1 * getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Float>(Type.TFloat, a1 * getFloat(b)));
                } else
                    throw new VException("Type error(mul)");
            } else if (a.type == Type.TFloat) {
                float a1 = getFloat(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Float>(Type.TFloat, a1 * getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Float>(Type.TFloat, a1 * getFloat(b)));
                } else
                    throw new VException("Type error(mul)");
            }
        }
    };	

    private static Cmd _div = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                float a1 = getInt(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Float>(Type.TFloat, a1 / (float)getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Float>(Type.TFloat, a1 / getFloat(b)));
                } else
                    throw new VException("Type error(div)");
            } else if (a.type == Type.TFloat) {
                float a1 = getFloat(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Float>(Type.TFloat, a1 / (float)getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Float>(Type.TFloat, a1 / getFloat(b)));
                } else
                    throw new VException("Type error(div)");
            }
        }
    };	

    private static Cmd _gt = new Cmd() {
        public void eval(Quote q) {
            Stack<Term> p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            if (a.type == Type.TInt) {
                int a1 = getInt(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Boolean>(Type.TBool, a1 > getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Boolean>(Type.TBool, a1 > getFloat(b)));
                } else
                    throw new VException("Type error(div)");
            } else if (a.type == Type.TFloat) {
                float a1 = getFloat(a);
                if (b.type == Type.TInt) {
                    p.push(new Term<Boolean>(Type.TBool, a1 > getInt(b)));
                } else if (b.type == Type.TFloat) {
                    p.push(new Term<Boolean>(Type.TBool, a1 > getFloat(b)));
                } else
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
        q.def("dup", _dup);
        q.def("+", _add);
        q.def("-", _sub);
        q.def("*", _mul);
        q.def("/", _div);

        //binary
        q.def(">?", _gt);
    }
}

