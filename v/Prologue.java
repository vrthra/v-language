package v;
import java.util.*;
public class Prologue {
    private static void pstack(QStack p) {
        for(Term t: p)
            V.debug("=" + t.value());
    }

    private static boolean and(Term a, Term b) {
        return a.bvalue() && b.bvalue();
    }

    private static boolean or(Term a, Term b) {
        return a.bvalue() || b.bvalue();
    }

    private static boolean isGt(Term a, Term b) {
        if (a.type == Type.TInt) {
            if (b.type == Type.TInt)
                return a.ivalue() > b.ivalue();
            else if (b.type == Type.TFloat)
                return a.ivalue() > b.fvalue();
            else
                throw new VException("Type error(>)");
        } else if (a.type == Type.TFloat) {
            if (b.type == Type.TInt)
                return a.fvalue() > b.ivalue();
            else if (b.type == Type.TFloat)
                return a.fvalue() > b.fvalue();
            else
                throw new VException("Type error(>)");
        }
        return false;
    }

    private static boolean isEq(Term a, Term b) {
        switch(a.type) {
            case TInt:
                if (b.type != Type.TInt)
                    throw new VException("Type error(=)");
                return a.ivalue() == b.ivalue();
            case TFloat:
                if (b.type != Type.TInt)
                    throw new VException("Type error(=)");
                return a.fvalue() == b.fvalue();
            case TString:
                if (b.type != Type.TString)
                    throw new VException("Type error(=)");
                return a.svalue().equals(b.svalue());
            default:
                return a.value().equals(b.value());
        }
    }

    private static boolean isLt(Term a, Term b) {
        if (isGt(a,b))
            return false;
        if (isEq(a,b))
            return false;
        return true;
    }

    public static void init(final Quote parent) {
        // accepts a quote as an argument.
        Cmd _def = new Cmd(parent) {
            @SuppressWarnings("unchecked")
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                Term t = p.pop();
                Iterator<Term> it = t.qvalue().tokens().iterator();
                Term<String> symbol = it.next();

                // copy the rest of tokens to our own stream.
                QuoteStream nts = new QuoteStream();
                while (it.hasNext())
                    nts.add(it.next());

                // we define it on the enclosing scope.
                // so our new command's parent is actually q rather than
                // _parent.
                V.debug("Def [" + symbol.val + "] @ " + q.id() + ":" + parent.id());
                q.def(symbol.val, new CmdQuote(nts, q));
            }
        };

        Cmd _true = new Cmd(parent) {
            public void eval(Quote q) {
                q.stack().push(new Term<Boolean>(Type.TBool, true));
            }
        };

        Cmd _false = new Cmd(parent) {
            public void eval(Quote q) {
                q.stack().push(new Term<Boolean>(Type.TBool, false));
            }
        };

        // Control structures
        Cmd _if = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

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

        Cmd _ifte = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

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
        Cmd _print = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term t = p.pop();
                V.out(t.value());
            }
        };

        Cmd _println = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term t = p.pop();
                V.outln(t.value());
            }
        };

        Cmd _peek = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                if (p.empty()) {
                    V.outln("");
                } else {
                    Term t = p.peek();
                    V.outln(t.value());
                }
            }
        };

        Cmd _show = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                if (p.empty()) {
                    V.outln("");
                } else {
                    for(Term t: p)
                        V.outln(t.value());
                }
            }
        };

        Cmd _qdebug = new Cmd(parent) {
            public void eval(Quote q) {
                V.outln("Quote[c:" + q.id() + "^" + q.id() + "]");
            }
        };

        Cmd _debug = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                V.debug(p.pop().bvalue());
            }
        };

        Cmd _dup = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                p.push(p.peek());
            }
        };

        Cmd _pop = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                V.debug("pop:" + q.id());
                p.pop();
            }
        };

        Cmd _swap = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term x = p.pop();
                Term y = p.pop();
                p.push(x);
                p.push(y);
            }
        };

        Cmd _lroll = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term t = p.pop();
                Iterator<Term> it = t.qvalue().tokens().iterator();
                Term first = it.next();

                // copy the rest of tokens to our own stream.
                QuoteStream nts = new QuoteStream();
                while (it.hasNext())
                    nts.add(it.next());
                nts.add(first);
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _rroll = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
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
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _map = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

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
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _fold = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term action = p.pop();
                Term init = p.pop();
                Term list = p.pop();

                Iterator<Term> fstream = list.qvalue().tokens().iterator();

                // push the init value in expectation of the next val and action.
                p.push(init);
                // copy the rest of tokens to our own stream.
                while (fstream.hasNext()) {
                    // extract the relevant element from list,
                    Term t = fstream.next();
                    // push it on our current stack
                    p.push(t);
                    // apply the action
                    action.qvalue().eval(q);
                    // pop it back into a new quote
                }
                // the result will be on the stack at the end of this cycle.
            }
        };

        Cmd _rev = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term list = p.pop();

                Iterator<Term> fstream = list.qvalue().tokens().iterator();
                Stack<Term> st = new Stack<Term>();
                // copy the rest of tokens to our own stream.
                QuoteStream nts = new QuoteStream();
                while (fstream.hasNext())
                    st.push(fstream.next());

                while(!st.empty())
                    nts.add(st.pop());
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _cons = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term next = p.pop();
                Term first = p.pop();
                // dequote both, append and push it back to stack.
                Iterator<Term> fstream = first.qvalue().tokens().iterator();

                // copy the rest of tokens to our own stream.
                QuoteStream nts = new QuoteStream();
                while (fstream.hasNext())
                    nts.add(fstream.next());
                nts.add(next);
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _dip = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term prog = p.pop();
                Term saved = p.pop();

                prog.qvalue().eval(q);
                p.push(saved);
            }
        };

        Cmd _concat = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

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
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _dequote = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term prog = p.pop();
                prog.qvalue().eval(q);
            }
        };

        Cmd _id = new Cmd(parent) {
            public void eval(Quote q) {
                // dummy. x id == x
            }
        };

        Cmd _add = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
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
                        throw new VException("Type error(+)");
                }
            }
        };

        Cmd _sub = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                if (a.type == Type.TInt) {
                    if (b.type == Type.TInt)
                        p.push(new Term<Integer>(Type.TInt, a.ivalue() - b.ivalue()));
                    else if (b.type == Type.TFloat)
                        p.push(new Term<Float>(Type.TFloat, a.ivalue() - b.fvalue()));
                    else
                        throw new VException("Type error(-)");
                } else if (a.type == Type.TFloat) {
                    if (b.type == Type.TInt)
                        p.push(new Term<Float>(Type.TFloat, a.fvalue() - b.ivalue()));
                    else if (b.type == Type.TFloat)
                        p.push(new Term<Float>(Type.TFloat, a.fvalue() - b.fvalue()));
                    else
                        throw new VException("Type error(-)");
                }
            }
        };

        Cmd _mul = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                if (a.type == Type.TInt) {
                    if (b.type == Type.TInt)
                        p.push(new Term<Integer>(Type.TInt, a.ivalue() * b.ivalue()));
                    else if (b.type == Type.TFloat)
                        p.push(new Term<Float>(Type.TFloat, a.ivalue() * b.fvalue()));
                    else
                        throw new VException("Type error(*)");
                } else if (a.type == Type.TFloat) {
                    if (b.type == Type.TInt)
                        p.push(new Term<Float>(Type.TFloat, a.fvalue() * b.ivalue()));
                    else if (b.type == Type.TFloat)
                        p.push(new Term<Float>(Type.TFloat, a.fvalue() * b.fvalue()));
                    else
                        throw new VException("Type error(*)");
                }
            }
        };

        Cmd _div = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                if (a.type == Type.TInt) {
                    if (b.type == Type.TInt)
                        p.push(new Term<Float>(Type.TFloat, (float)(a.ivalue() / (1.0 * b.ivalue()))));
                    else if (b.type == Type.TFloat)
                        p.push(new Term<Float>(Type.TFloat, a.ivalue() / b.fvalue()));
                    else
                        throw new VException("Type error(/)");
                } else if (a.type == Type.TFloat) {
                    if (b.type == Type.TInt)
                        p.push(new Term<Float>(Type.TFloat, a.fvalue() / b.ivalue()));
                    else if (b.type == Type.TFloat)
                        p.push(new Term<Float>(Type.TFloat, a.fvalue() / b.fvalue()));
                    else
                        throw new VException("Type error(/)");
                }
            }
        };

        Cmd _gt = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, isGt(a, b)));
            }
        };

        Cmd _lt = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, isLt(a, b)));
            }
        };

        Cmd _lteq = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, !isGt(a, b)));
            }
        };

        Cmd _gteq = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, !isLt(a, b)));
            }
        };


        Cmd _eq = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, isEq(a, b)));
            }
        };

        Cmd _neq = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, !isEq(a, b)));
            }
        };

        Cmd _and = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, and(a, b)));
            }
        };

        Cmd _or = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, or(a, b)));
            }
        };

        Cmd _not = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.pop();
                p.push(new Term<Boolean>(Type.TBool, !a.bvalue()));
            }
        };

        // Math
        Cmd _sqrt = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                p.push(new Term<Integer>(Type.TInt,(int)Math.sqrt(p.pop().ivalue())));
            }
        };

        //meta
        parent.def(".", _def);
        parent.def("true", _true);
        parent.def("false", _false);

        parent.def("and", _and);
        parent.def("or", _or);
        parent.def("not", _not);

        //control structures
        parent.def("ifte", _ifte);
        parent.def("if", _if);

        //io
        parent.def("put", _print);
        parent.def("puts", _println);
        parent.def("print", _print);
        parent.def("println", _println);

        //others
        parent.def("?", _peek);
        parent.def("??", _show);
        parent.def("???", _qdebug);
        parent.def("debug", _debug);

        parent.def("dup", _dup);
        parent.def("pop", _pop);
        parent.def("swap", _swap);
        parent.def("lroll", _lroll);
        parent.def("rollup", _lroll);
        parent.def("rroll", _rroll);
        parent.def("rolldown", _rroll);
        parent.def("map", _map);
        parent.def("fold", _fold);
        parent.def("rev", _rev);
        parent.def("concat", _concat);
        parent.def("cons", _cons);
        parent.def("dip", _dip);
        parent.def("i", _dequote);
        parent.def("id", _id);

        //arith
        parent.def("+", _add);
        parent.def("-", _sub);
        parent.def("*", _mul);
        parent.def("/", _div);

        //bool
        parent.def("=", _eq);
        parent.def("!=", _neq);
        parent.def(">", _gt);
        parent.def("<", _lt);
        parent.def("<=", _lteq);
        parent.def(">=", _gteq);

        //math
        parent.def("sqrt", _sqrt);
    }
}

