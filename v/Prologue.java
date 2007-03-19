package v;
import java.util.*;
import java.io.*;
import v.java.*;

class Shield {
    // current stack
    Node<Term> stack;
    Quote quote;
    Shield(QStack s, Quote q) {
        stack = s.now;
        quote = q;
    }
};

public class Prologue {
    private static boolean and(Term a, Term b) {
        return a.bvalue() && b.bvalue();
    }

    private static boolean or(Term a, Term b) {
        return a.bvalue() || b.bvalue();
    }

    private static boolean isGt(Term a, Term b) {
        return a.numvalue().doubleValue()> b.numvalue().doubleValue();
    }

    private static boolean isEq(Term a, Term b) {
        switch(a.type) {
            case TInt:
            case TDouble:
                return a.numvalue().doubleValue() ==  b.numvalue().doubleValue();
            case TString:
                if (b.type != Type.TString)
                    throw new VException("Type error(=)\n\t|" + a.value() + " " + b.value());
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

    public static void evaluate(Quote q, String buff) {
        getdef(q, buff).eval(q, true);
    }

    static Quote compile(Quote q, Quote v) {
        QuoteStream nts = new QuoteStream();
        for(Term t:  v.tokens())
            nts.add(t);

        return new CmdQuote(nts, q);
    }

    static Quote getdef(Quote q, String buf) {
        CharStream cs = new BuffCharStream(buf);
        return compile(q, new CmdQuote(new LexStream(cs), q));
    }
    
    static String getresource(String s) {
        try {
            StringBuffer buf = new StringBuffer();
            BufferedReader br = new BufferedReader(new InputStreamReader(Prologue.class.getResourceAsStream(s)));
            String line;
            while((line = br.readLine()) != null) {
                buf.append(line);
            }
            return buf.toString();
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
    @SuppressWarnings("unchecked")
    static Map.Entry<String, CmdQuote> splitdef(Quote qval, Quote q) {
        HashMap<String, CmdQuote> map = new HashMap<String, CmdQuote>();
        Iterator<Term> it = (Iterator<Term>)qval.tokens().iterator();
        Term<String> symbol = it.next();

        /*Quote check = q.lookup(symbol.svalue());
          if (check != null)
          throw new VException("Attempt to redefine (" + symbol.value() + ") -- we are pure.");*/

        // copy the rest of tokens to our own stream.
        QuoteStream nts = new QuoteStream();
        while (it.hasNext())
            nts.add(it.next());

        // we define it on the enclosing scope.
        // so our new command's parent is actually q rather than
        // _parent.
        map.put(symbol.val, new CmdQuote(nts, q)); 
        return map.entrySet().iterator().next();
    }

    static QuoteStream evalres(TokenStream res, HashMap<String, Term> symbols, Quote q) {
        QuoteStream r = new QuoteStream();
        Iterator<Term> rstream = res.iterator();
        while(rstream.hasNext()) {
            Term t = rstream.next();
            switch(t.type) {

                case TQuote:
                    QuoteStream nq = evalres(t.qvalue().tokens(), symbols, q);
                    r.add(new Term<Quote>(Type.TQuote, new CmdQuote(nq, q)));
                    break;
                case TSymbol:
                    // do we have it in our symbol table? if yes, replace, else just push it in.
                    /*V.outln("?" + t.svalue());
                    for(String s: symbols.keySet()) {
                        V.outln("-" + s + "+" + symbols.get(s).value());
                    }*/
                    String sym = t.svalue();
                    if (symbols.containsKey(sym)) {
                        // does it look like *xxx ?? 
                        if (sym.charAt(0) == '*') {
                            // expand it.
                            Term star = symbols.get(sym);
                            for(Term x : star.qvalue().tokens()) {
                                r.add(x);
                            }
                        } else
                            r.add(symbols.get(sym));
                        break;
                    }
                default:
                    // just push it in.
                    r.add(t);
            }
        }
        return r;
    }

    static void evaltmpl(TokenStream tmpl, TokenStream elem, HashMap<String, Term> symbols, Quote q) {
        //Take each point in tmpl, and proess elements accordingly.
        Iterator<Term> tstream = tmpl.iterator();
        Iterator<Term> estream = elem.iterator();
        while(tstream.hasNext()) {
            Term t = tstream.next();
            switch (t.type) {
                case TSymbol:
                    try {
                        // _ means any one
                        // * means any including nil unnamed.
                        // *a means any including nil but named with symbol '*a'
                        String value = t.value();
                        if (value.charAt(0) == '_') {
                            // eat one from estream and continue.
                            estream.next();
                            break;
                        } else if (value.charAt(0) == '*') {
                            QuoteStream nlist = new QuoteStream();
                            // * is all. but before we slurp, check the next element
                            // in the template. If there is not any, then slurp. If there
                            // is one, then slurp until last but one, and leave it.
                            if (tstream.hasNext()) {
                                Term tmplterm = tstream.next();
                                Term lastelem = null;

                                // slurp till last but one.
                                while(estream.hasNext()) {
                                    lastelem = estream.next();
                                    if (estream.hasNext())
                                        nlist.add(lastelem);
                                }

                                switch (tmplterm.type) {
                                    case TSymbol:
                                        // assign value in symbols.
                                        symbols.put(tmplterm.svalue(), lastelem);
                                        break;
                                    case TQuote:
                                        evaltmpl(tmplterm.qvalue().tokens(), lastelem.qvalue().tokens(), symbols, q);
                                        break;
                                    default:
                                        if (tmplterm.value().equals(lastelem.value()))
                                            break;
                                        else
                                            throw new VException("V(evaltmpl:assert) "
                                                    + tmplterm.value() + "<>" + lastelem.value() );
                                }

                            } else {
                                // we can happily slurp now.
                                while(estream.hasNext())
                                    nlist.add(estream.next());
                            }
                            if (value.length() > 1) { // do we have a named list?
                                symbols.put(value, new Term<Quote>(Type.TQuote, new CmdQuote(nlist, q)));
                            }
                        } else {
                            Term e = estream.next();
                            symbols.put(t.value(), e);
                        }
                        break;
                    } catch (VException e) {
                        throw new VException(e.getMessage() + "\n\t|" + t.value());
                    } catch (Exception e) {
                        throw new VException(e.getMessage() + "\n\tat V(evaltemplate:sym)\n\t|" + t.value());
                    }

                case TQuote:
                    // evaluate this portion again in evaltmpl.
                    try {
                        Term et = estream.next();
                        evaltmpl(t.qvalue().tokens(), et.qvalue().tokens(), symbols, q);
                    } catch (VException e) {
                        throw new VException(e.getMessage() + "\n\t|" + t.value());
                    } catch (Exception e) {
                        throw new VException(e.getMessage() + "\n\tat V(evaltemplate:quote)\n\t|" + t.value());
                    }
                    break;
                default:
                    //make sure both matches.
                    Term eterm = estream.next();
                    if (t.value().equals(eterm.value()))
                        break;
                    else
                        throw new VException("V(evaltmpl:assert) " + t.value() + "<>" + eterm.value() 
                                + "\n\t|V(evaltemplate:default)\n\t|" + eterm.value() + "\n\t|" + t.value());

            }
        }
    }

    public static void init(final Quote parent) {
        // accepts a quote as an argument.
        Cmd _def = new Cmd(parent) {
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                Term t = p.pop();
                Map.Entry<String, CmdQuote> entry = splitdef(t.qvalue(), q);
                String symbol = entry.getKey();
                
                // we define it on the enclosing scope.
                // so our new command's parent is actually q rather than
                // _parent.
                V.debug("Def [" + symbol + "] @ " + q.id() + ":" + parent.id());
                q.def(symbol, entry.getValue());
            }
        };

        Cmd _me = new Cmd(parent) {
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                p.push(new Term<Quote>(Type.TQuote, q));
            }
        };

        Cmd _parent = new Cmd(parent) {
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                Term t = p.pop();
                p.push(new Term<Quote>(Type.TQuote, t.qvalue().parent()));
            }
        };

        Cmd _definenv = new Cmd(parent) {
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                Term b = p.pop();
                Term t = p.pop();
                
                Map.Entry<String, CmdQuote> entry = splitdef(t.qvalue(), q);
                String symbol = entry.getKey();

                V.debug("DefP [" + symbol + "] @ " + q.id() + ":" + parent.id());
                b.qvalue().def(symbol, entry.getValue());
            }
        };

        // [a b c obj method] java
        Cmd _java = new Cmd(parent) {
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                Term v = p.pop();
                LinkedList<Term> st = new LinkedList<Term>();
                for(Term t: v.qvalue().tokens())
                    st.addFirst(t);

                Iterator<Term> i = st.iterator();
                Term method = i.next();
                Term object = i.next();
                QuoteStream qs = new QuoteStream();
                while(i.hasNext())
                    qs.add(i.next());
                Term res = Helper.invoke(object, method, new CmdQuote(qs, q));
                p.push(res);
            }
        };
 
        // a b c [a b c : [a b c]] V
        // [a b c] [[a b c] : a b c] V
        // [a b c] [[a _] : [a a]] V -- _ indicates any value.
        // [a b c] [[a *b] : [a a]] V -- * indicates an addressible list.
        //
        // a b c d e f [a *b : [a b]] V => a b c d [e f] -- we ignore the
        // *x on the first level and treat it as just an element.

        Cmd _shuffle = new Cmd(parent) {
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                Term v = p.pop();
                Iterator<Term> fstream = v.qvalue().tokens().iterator();

                // iterate through the quote, and find where ':' is then split it
                // into two half and analyze the first.
                QuoteStream tmpl = new QuoteStream();
                while (fstream.hasNext()) {
                    Term t = fstream.next();
                    if (t.type == Type.TSymbol && t.value().equals(":"))
                        break;
                    tmpl.add(t);
                }
                
                QuoteStream res = new QuoteStream();
                while (fstream.hasNext()) {
                    Term t = fstream.next();
                    res.add(t);
                }

                // collect as much params as there is from stack as there is in the template
                // first level.
                QuoteStream elem = new QuoteStream();
                fstream = tmpl.iterator();
                LinkedList<Term> st = new LinkedList<Term>();
                while (fstream.hasNext()) {
                    Term t = fstream.next();
                    Term e = p.pop();
                    st.addFirst(e);
                }
                for (Term e: st)
                    elem.add(e);
                
                HashMap<String, Term> symbols = new HashMap<String, Term>();
                /*V.outln(".......tmpl....");
                for(Term t: tmpl)
                    V.outln(t.value());
                V.outln(".......res....");
                for(Term t: res)
                    V.outln(t.value());
                V.outln(".......elem....");
                for(Term t: elem)
                    V.outln(t.value());*/
                //Now take each elem and its pair templ and extract the symbols and their meanings.
                evaltmpl(tmpl, elem, symbols, q);
                /*for (String s : symbols.keySet()) {
                    V.outln(s + ":" + symbols.get(s).value());
                }*/

                // now go over the quote we were just passed and replace each symbol with what we
                // have if we do have a definition.
                QuoteStream resstream = evalres(res, symbols, q);
                CmdQuote qs = new CmdQuote(resstream, q);

                Iterator<Term> i = qs.tokens().iterator();
                while (i.hasNext())
                    p.push(i.next());
                //qs.eval(q);
            }
        };

        Cmd _words = new Cmd(parent) {
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                Term t = p.pop();

                // copy the rest of tokens to our own stream.
                QuoteStream nts = new QuoteStream();
                for(String s: t.qvalue().bindings().keySet())
                    nts.add(new Term<String>(Type.TSymbol,s));
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _shield = new Cmd(parent) {
            @SuppressWarnings("unchecked")
            public void eval(Quote q) {
                // eval is passed in the quote representing the current scope.
                QStack p = q.stack();
                Term t = p.pop();
                // save the stack.
                // we can also have multiple shields
                // Try and get the $shield if any
                Cmd shield = (Cmd)q.bindings().get("$shield");
                Shield s = new Shield(p,t.qvalue());
                if (shield == null) {
                    shield = new Cmd(q){public void eval(Quote q){}};
                    shield.store().put("$info", new Stack<Shield>());
                    q.bindings().put("$shield", shield);
                }
                Stack<Shield> stack = (Stack<Shield>)shield.store().get("$info");
                stack.push(s);
                V.debug("Shield @ " + q.id() + ":" + parent.id());
            }
        };

        Cmd _throw = new Cmd(parent) {
            public void eval(Quote q) {
                throw new VException("Error(" + q.stack().peek().value() + ")" );
            }
        };

        Cmd _abort = new Cmd(parent) {
            public void eval(Quote q) {
                q.stack().clear();
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

                if (cond.type == Type.TQuote) {
                    Node<Term> n = p.now;
                    cond.qvalue().eval(q);
                    // and get it back from stack.
                    cond = p.pop();
                    p.now = n;
                }

                // dequote the action and push it to stack.
                if (cond.bvalue())
                    action.qvalue().eval(q, true);
            }
        };

        Cmd _choice = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term af = p.pop();
                Term at = p.pop();
                Term cond = p.pop();

                if (cond.bvalue())
                    p.push(at);
                else
                    p.push(af);
            }
        };


        Cmd _ifte = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term eaction = p.pop();
                Term action = p.pop();
                Term cond = p.pop();

                if (cond.type == Type.TQuote) {
                    Node<Term> n = p.now;
                    cond.qvalue().eval(q);
                    // and get it back from stack.
                    cond = p.pop();
                    p.now = n;
                }
                // dequote the action and push it to stack.
                if (cond.bvalue())
                    action.qvalue().eval(q, true);
                else
                    eaction.qvalue().eval(q, true);
            }
        };

        Cmd _while = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term action = p.pop();
                Term cond = p.pop();
                while(true) {
                    if (cond.type == Type.TQuote) {
                        Node<Term> n = p.now;
                        cond.qvalue().eval(q);
                        // and get it back from stack.
                        cond = p.pop();
                        p.now = n;
                    }
                    // dequote the action and push it to stack.
                    if (cond.bvalue())
                        action.qvalue().eval(q, true);
                    else
                        break;
                }
            }
        };

        /* The genrec combinator takes four program parameters in addition to
         * whatever data parameters it needs. Fourth from the top is an
         * if-part, followed by a then-part. If the if-part yields true, then
         * the then-part is executed and the combinator terminates. The other
         * two parameters are the rec1-part and the rec2part. If the if-part
         * yields false, the rec1-part is executed. Following that the four
         * program parameters and the combinator are again pushed onto the
         * stack bundled up in a quoted form. Then the rec2-part is executed,
         * where it will find the bundled form. Typically it will then execute
         * the bundled form, either with i or with app2, or some other combinator.*/
        Cmd _genrec = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term rec2 = p.pop();
                Term rec1 = p.pop();
                Term thenp = p.pop();
                Term tifp = p.pop();

                // evaluate if and pop it back.
                Node<Term> n = p.now;
                tifp.qvalue().eval(q);
                Term ifp = p.pop();
                p.now = n;

                // dequote the action and push it to stack.
                if (ifp.bvalue()) {
                    thenp.qvalue().eval(q, true);
                    return;
                } else {
                    rec1.qvalue().eval(q, true);
                    QuoteStream nts = new QuoteStream();
                    nts.add(tifp);
                    nts.add(thenp);
                    nts.add(rec1);
                    nts.add(rec2);
                    nts.add(new Term<String>(Type.TSymbol, "genrec"));
                    p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
                    rec2.qvalue().eval(q, true);
                }
            }
        };

        Cmd _linrec = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term rec2 = p.pop();
                Term rec1 = p.pop();
                Term thenp = p.pop();
                Term tifp = p.pop();

                // evaluate if and pop it back.
                Node<Term> n = p.now;
                tifp.qvalue().eval(q);
                Term ifp = p.pop();
                p.now = n;

                // dequote the action and push it to stack.
                if (ifp.bvalue()) {
                    thenp.qvalue().eval(q, true);
                    return;
                } else {
                    rec1.qvalue().eval(q, true);
                    QuoteStream nts = new QuoteStream();
                    nts.add(tifp);
                    nts.add(thenp);
                    nts.add(rec1);
                    nts.add(rec2);
                    nts.add(new Term<String>(Type.TSymbol, "linrec"));
                    (new CmdQuote(nts, q)).eval(q, true);
                    rec2.qvalue().eval(q, true);
                }
            }
        };

        Cmd _binrec = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term rec2 = p.pop();
                Term rec1 = p.pop();
                Term thenp = p.pop();
                Term tifp = p.pop();

                // evaluate if and pop it back.
                Node<Term> n = p.now;
                tifp.qvalue().eval(q);
                Term ifp = p.pop();
                p.now = n;

                // dequote the action and push it to stack.
                if (ifp.bvalue()) {
                    thenp.qvalue().eval(q, true);
                } else {
                    rec1.qvalue().eval(q, true);
                    Term nvl = p.pop();
                    QuoteStream nts = new QuoteStream();
                    nts.add(tifp);
                    nts.add(thenp);
                    nts.add(rec1);
                    nts.add(rec2);
                    nts.add(new Term<String>(Type.TSymbol, "binrec"));
                    Quote nq = new CmdQuote(nts, q);
                    nq.eval(q, true);
                    p.push(nvl);
                    nq.eval(q, true);
                    rec2.qvalue().eval(q, true);
                }
            }
        };

        Cmd _tailrec = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term rec = p.pop();
                Term thenp = p.pop();
                Term tifp = p.pop();

                // evaluate if and pop it back.
                Node<Term> n = p.now;
                tifp.qvalue().eval(q);
                Term ifp = p.pop();
                p.now = n;

                // dequote the action and push it to stack.
                if (ifp.bvalue()) {
                    thenp.qvalue().eval(q, true);
                } else {
                    rec.qvalue().eval(q, true);
                    QuoteStream nts = new QuoteStream();
                    nts.add(tifp);
                    nts.add(thenp);
                    nts.add(rec);
                    nts.add(new Term<String>(Type.TSymbol, "tailrec"));
                    Quote nq = new CmdQuote(nts, q);
                    nq.eval(q, true);
                }
            }
        };

        Cmd _primrec = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term rec = p.pop();
                Term thenp = p.pop();
                Term param = p.peek();

                QuoteStream nts = new QuoteStream();
                switch (param.type) {
                    case TInt:
                        // evaluate if and pop it back.
                        if (param.ivalue() == 0) {
                            p.pop();
                            thenp.qvalue().eval(q, true);
                            return;
                        } else {
                            nts.add(new Term<String>(Type.TSymbol, "dup"));
                            nts.add(new Term<String>(Type.TSymbol, "pred"));
                        }
                        break;
                    case TDouble:
                        // evaluate if and pop it back.
                        if (param.dvalue() == 0) {
                            p.pop();
                            thenp.qvalue().eval(q, true);
                            return;
                        } else {
                            nts.add(new Term<String>(Type.TSymbol, "dup"));
                            nts.add(new Term<String>(Type.TSymbol, "pred"));
                        }
                        break;
                    case TQuote:
                        int i = 0;
                        for(Term t: param.qvalue().tokens())
                            ++i;
                        if (i == 0) {
                            p.pop();
                            thenp.qvalue().eval(q, true);
                            return;
                        } else {
                            nts.add(new Term<String>(Type.TSymbol, "dup"));
                            nts.add(new Term<String>(Type.TSymbol, "rest"));
                        }
                        break;
                    default:
                        throw new VException("wrong datatype for primrec\n\t|" + param.value());
                }
                Quote nq = new CmdQuote(nts, q);
                nq.eval(q, true);
                // have the next param on stack now. apply primrec on it.
                QuoteStream n = new QuoteStream();
                n.add(thenp);
                n.add(rec);
                n.add(new Term<String>(Type.TSymbol, "primrec"));
                nq = new CmdQuote(n, q);
                nq.eval(q, true);
                rec.qvalue().eval(q, true);

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
                p.dump();
            }
        };

        Cmd _qdebug = new Cmd(parent) {
            public void eval(Quote q) {
                V.outln("Quote[c:" + q.id() + "^" + q.id() + "]");
                q.stack().dump();                QStack p = q.stack();

                for(String s: q.bindings().keySet())
                    V.out(":" + s + " ");
                V.outln("\n________________");
 
            }
        };

        Cmd _debug = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                V.debug(p.pop().bvalue());
            }
        };

        Cmd _step = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term action = p.pop();
                Term list = p.pop();

                Iterator<Term> fstream = list.qvalue().tokens().iterator();

                while (fstream.hasNext()) {
                    // extract the relevant element from list,
                    Term t = fstream.next();
                    // push it on our current stack
                    p.push(t);

                    // apply the action
                    // We dont do the walk here since the action is in the form of a quote.
                    // we will have to dequote it, and walk one by one if we are to do this.
                    action.qvalue().eval(q, true);
                }
            }
        };

        // this map is not a stack invariant. specifically 
        // 1 2 3 4  [a b c d] [[] cons cons] map => [[4 a] [3 b] [2 c] [1 d]]
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
                    // We dont do the walk here since the action is in the form of a quote.
                    // we will have to dequote it, and walk one by one if we are to do this.
                    action.qvalue().eval(q, true);
                    // pop it back into a new quote
                    Term res = p.pop();
                    nts.add(res);
                }
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        // map is a stack invariant. specifically 
        // 1 2 3 4  [a b c d] [[] cons cons] map => [[4 a] [4 b] [4 c] [4 d]]
        Cmd _map_i = new Cmd(parent) {
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
                    Node<Term> n = p.now;
                    // push it on our current stack
                    p.push(t);

                    // apply the action
                    // We dont do the walk here since the action is in the form of a quote.
                    // we will have to dequote it, and walk one by one if we are to do this.
                    action.qvalue().eval(q, true);
                    // pop it back into a new quote
                    Term res = p.pop();
                    p.now = n;
                    nts.add(res);
                }
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _split = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term action = p.pop();
                Term list = p.pop();

                Iterator<Term> fstream = list.qvalue().tokens().iterator();

                // copy the rest of tokens to our own stream.
                QuoteStream nts1 = new QuoteStream();
                QuoteStream nts2 = new QuoteStream();
                while (fstream.hasNext()) {
                    // extract the relevant element from list,
                    Term t = fstream.next();
                    // push it on our current stack
                    p.push(t);

                    // apply the action
                    // We dont do the walk here since the action is in the form of a quote.
                    // we will have to dequote it, and walk one by one if we are to do this.
                    action.qvalue().eval(q, true);
                    // pop it back into a new quote
                    Term res = p.pop();
                    if (res.bvalue())
                        nts1.add(t);
                    else
                        nts2.add(t);
                }
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts1, q)));
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts2, q)));
            }
        };

        Cmd _split_i = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term action = p.pop();
                Term list = p.pop();

                Iterator<Term> fstream = list.qvalue().tokens().iterator();

                // copy the rest of tokens to our own stream.
                QuoteStream nts1 = new QuoteStream();
                QuoteStream nts2 = new QuoteStream();
                while (fstream.hasNext()) {
                    // extract the relevant element from list,
                    Term t = fstream.next();
                    // push it on our current stack
                    Node<Term> n = p.now;
                    p.push(t);

                    // apply the action
                    // We dont do the walk here since the action is in the form of a quote.
                    // we will have to dequote it, and walk one by one if we are to do this.
                    action.qvalue().eval(q, true);
                    // pop it back into a new quote
                    Term res = p.pop();
                    p.now = n;
                    if (res.bvalue())
                        nts1.add(t);
                    else
                        nts2.add(t);
                }
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts1, q)));
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts2, q)));
            }
        };

        Cmd _filter = new Cmd(parent) {
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
                    // We dont do the walk here since the action is in the form of a quote.
                    // we will have to dequote it, and walk one by one if we are to do this.
                    action.qvalue().eval(q, true);
                    // pop it back into a new quote
                    Term res = p.pop();
                    if (res.bvalue())
                        nts.add(t);
                }
                p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts, q)));
            }
        };

        Cmd _filter_i = new Cmd(parent) {
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
                    Node<Term> n = p.now;
                    p.push(t);

                    // apply the action
                    // We dont do the walk here since the action is in the form of a quote.
                    // we will have to dequote it, and walk one by one if we are to do this.
                    action.qvalue().eval(q, true);
                    // pop it back into a new quote
                    Term res = p.pop();
                    p.now = n;
                    if (res.bvalue())
                        nts.add(t);
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
                    action.qvalue().eval(q, true);
                }
                Term res = p.pop();
                p.push(res);
                // the result will be on the stack at the end of this cycle.
            }
        };

        Cmd _fold_i = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term action = p.pop();
                Term init = p.pop();
                Term list = p.pop();

                Node<Term> n = p.now;
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
                    action.qvalue().eval(q, true);
                }
                Term res = p.pop();
                p.now = n;
                p.push(res);
                // the result will be on the stack at the end of this cycle.
            }
        };

        Cmd _reverse = new Cmd(parent) {
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

        Cmd _size = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term list = p.pop();
                int count = 0;
                for(Term t: list.qvalue().tokens())
                    ++count;

                p.push(new Term<Integer>(Type.TInt , count));
            }
        };

        Cmd _dequote = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term prog = p.pop();
                V.debug("Dequote @ " + q.id() + ":" + parent.id() + " prog " + prog.qvalue().id());
                prog.qvalue().eval(q, true); // apply on parent
            }
        };

        Cmd _dequoteenv = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();

                Term env = p.pop();
                Term prog = p.pop();
                V.debug("Dequote @ " + q.id() + ":" + parent.id() + " prog " + prog.qvalue().id());
                prog.qvalue().eval(env.qvalue(), true); // apply on parent
            }
        };

        Cmd _add = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                double dres = a.numvalue().doubleValue() + b.numvalue().doubleValue();
                int ires  = (int)dres;
                if (dres == ires)
                    p.push(new Term<Integer>(Type.TInt, ires));
                else
                    p.push(new Term<Double>(Type.TDouble, dres));
            }
        };

        Cmd _sub = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                double dres = a.numvalue().doubleValue() - b.numvalue().doubleValue();
                int ires  = (int)dres;
                if (dres == ires)
                    p.push(new Term<Integer>(Type.TInt, ires));
                else
                    p.push(new Term<Double>(Type.TDouble, dres));
            }
        };

        Cmd _mul = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                double dres = a.numvalue().doubleValue() * b.numvalue().doubleValue();
                int ires  = (int)dres;
                if (dres == ires)
                    p.push(new Term<Integer>(Type.TInt, ires));
                else
                    p.push(new Term<Double>(Type.TDouble, dres));
            }
        };

        Cmd _div = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term b = p.pop();
                Term a = p.pop();
                double dres = a.numvalue().doubleValue() / b.numvalue().doubleValue();
                int ires  = (int)dres;
                if (dres == ires)
                    p.push(new Term<Integer>(Type.TInt, ires));
                else
                    p.push(new Term<Double>(Type.TDouble, dres));
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


        // Predicates do not consume the element. 
        Cmd _isbool = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.peek();
                p.push(new Term<Boolean>(Type.TBool, a.type == Type.TBool));
            }
        };

        Cmd _isinteger = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.peek();
                p.push(new Term<Boolean>(Type.TBool, a.type == Type.TInt));
            }
        };

        Cmd _isdouble = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.peek();
                p.push(new Term<Boolean>(Type.TBool, a.type == Type.TDouble));
            }
        };

        Cmd _issym = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.peek();
                p.push(new Term<Boolean>(Type.TBool, a.type == Type.TSymbol));
            }
        };

        Cmd _islist = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.peek();
                p.push(new Term<Boolean>(Type.TBool, a.type == Type.TQuote));
            }
        };

        Cmd _isstr = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.peek();
                p.push(new Term<Boolean>(Type.TBool, a.type == Type.TString));
            }
        };

        Cmd _isnum = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.peek();
                p.push(new Term<Boolean>(Type.TBool,
                            a.type == Type.TInt || a.type == Type.TDouble));
            }
        };

        Cmd _ischar = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.peek();
                p.push(new Term<Boolean>(Type.TBool, a.type == Type.TChar));
            }
        };

        Cmd _tostring = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.pop();
                p.push(new Term<String>(Type.TString, a.value()));
            }
        };

        Cmd _toint = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.pop();
                p.push(new Term<Integer>(Type.TInt, (new Double(a.value())).intValue()));
            }
        };

        Cmd _todecimal = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term a = p.pop();
                p.push(new Term<Double>(Type.TDouble, new Double(a.value())));
            }
        };


        /* stdlib.v
         * [stdlib 
         *      [qsort  xxx yyy].
         *      [binsearch aaa bbb].
         * ]
         *
         * 'stdlib' use
         * [1 7 3 2 2] stdlib:qsort
         * */
        Cmd _use = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term file = p.pop();
                String val = file.svalue() + ".v";
                try {
                    // Try and see if the file requested is any of the standard defined
                    String chars = getresource(val);
                    CharStream cs = chars == null? new FileCharStream(val) : new BuffCharStream(chars);

                    CmdQuote module = new CmdQuote(new LexStream(cs), q);
                    module.eval(q,true);
                    V.debug("use @ " + q.id());
                } catch (Exception e) {
                    throw new VException(">use failed \n\t|" + file.value());
                }
            }
        };

        Cmd _useenv = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term env = p.pop();
                Term file = p.pop();
                String val = file.svalue() + ".v";
                try {
                    // Try and see if the file requested is any of the standard defined
                    String chars = getresource(val);
                    CharStream cs = chars == null? new FileCharStream(val) : new BuffCharStream(chars);

                    CmdQuote module = new CmdQuote(new LexStream(cs), env.qvalue());
                    module.eval(env.qvalue(),true);
                    V.debug("use @ " + q.id());
                } catch (Exception e) {
                    throw new VException(">*use failed \n\t|" + file.value());
                }
            }
        };

        Cmd _eval = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term buff = p.pop();
                try {
                    evaluate(q, buff.svalue());
                    V.debug("eval @ " + q.id());
                } catch (Exception e) {
                    throw new VException(">eval failed \n\t|" + buff.value());
                }
            }
        };

        Cmd _evalenv = new Cmd(parent) {
            public void eval(Quote q) {
                QStack p = q.stack();
                Term env = p.pop();
                Term buff = p.pop();
                try {
                    evaluate(env.qvalue(), buff.svalue());
                    V.debug("eval @ " + env.qvalue().id());
                } catch (Exception e) {
                    throw new VException(">*eval failed \n\t|" + buff.value());
                }
            }
        };

        Cmd _help = new Cmd(parent) {
            public void eval(Quote q) {
                HashMap <String,Quote> bind = parent.bindings();
                for(String s : new TreeSet<String>(bind.keySet()))
                    V.outln(s);
            }
        };

        //meta
        parent.def(".", _def);
        parent.def("@", _definenv);
        parent.def("$me", _me);
        parent.def("$parent", _parent);
        parent.def("$words", _words);

        parent.def("*i", _dequoteenv);
        parent.def("i", _dequote);

        parent.def("V", _shuffle);
        parent.def("java", _java);

        parent.def("true", _true);
        parent.def("false", _false);
        parent.def("shield", _shield);
        parent.def("throw", _throw);

        parent.def("and", _and);
        parent.def("or", _or);
        parent.def("not", _not);

        //control structures
        parent.def("ifte", _ifte);
        parent.def("if", _if);
        parent.def("while", _while);
        parent.def("choice", _choice);

        //io
        parent.def("put", _print);
        parent.def("puts", _println);

        //others
        parent.def("?", _peek);
        parent.def("??", _show);
        parent.def("???", _qdebug);
        parent.def("debug", _debug);

        parent.def("abort", _abort);

        //list
        parent.def("reverse", _reverse);
        parent.def("size", _size);
       

        // on list
        parent.def("step", _step);
        parent.def("map", _map);
        parent.def("map&", _map_i);
        parent.def("filter", _filter);
        parent.def("filter&", _filter_i);
        parent.def("split", _split);
        parent.def("split&", _split_i);
        parent.def("fold", _fold);
        parent.def("fold&", _fold_i);

        //arith
        parent.def("+", _add);
        parent.def("-", _sub);
        parent.def("*", _mul);
        parent.def("/", _div);

        //bool
        parent.def("=", _eq);
        parent.def("==", _eq);
        parent.def("!=", _neq);
        parent.def(">", _gt);
        parent.def("<", _lt);
        parent.def("<=", _lteq);
        parent.def(">=", _gteq);

        //predicates
        //The predicates do not consume stuff off the stack. They just
        //peek and push the result. the reason for this is that we generally
        //do -> if (x is yyy) then {do some thing with x} so it is more
        //natural to let x be there in the stack than to pop it off.
        parent.def("integer?", _isinteger);
        parent.def("double?", _isdouble);
        parent.def("boolean?", _isbool);
        parent.def("symbol?", _issym);
        parent.def("list?", _islist);
        parent.def("char?", _ischar);
        parent.def("number?", _isnum);
        parent.def("string?", _isstr);

        parent.def(">string", _tostring);
        parent.def(">int", _toint);
        parent.def(">decimal", _todecimal);

        // recursion
        parent.def("genrec", _genrec);
        parent.def("linrec", _linrec);
        parent.def("binrec", _binrec);
        parent.def("tailrec", _tailrec);
        parent.def("primrec", _primrec);

        //modules
        parent.def("use", _use);
        parent.def("*use", _useenv);
        parent.def("eval", _eval);
        parent.def("*eval", _evalenv);

        parent.def("help", _help);
        
        Quote libs = getdef(parent, "'std' use");
        libs.eval(parent, true);
    }
}

