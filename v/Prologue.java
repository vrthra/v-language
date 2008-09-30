package v;
import java.util.*;
import java.io.*;
import v.java.*;

class Shield {
    // current stack
    Node<Term> stack;
    Quote quote;
    Shield(VStack s, Quote q) {
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
                return a.svalue().equals(b.svalue());
            case TSymbol:
                return a.svalue() == b.svalue(); // constant strings
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

    @SuppressWarnings("unchecked")
        static Map.Entry<String, CmdQuote> splitdef(Quote qval) {
            HashMap<String, CmdQuote> map = new HashMap<String, CmdQuote>();
            Iterator<Term> it = (Iterator<Term>)qval.tokens().iterator();
            Term<String> symbol = it.next();

            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            while (it.hasNext())
                nts.add(it.next());

            // we define it on the enclosing scope.
            // so our new command's parent is actually q rather than
            // parent.
            map.put(symbol.val, new CmdQuote(nts)); 
            return map.entrySet().iterator().next();
        }

    static QuoteStream evalres(TokenStream res, HashMap<String, Term> symbols) {
        QuoteStream r = new QuoteStream();
        Iterator<Term> rstream = res.iterator();
        while(rstream.hasNext()) {
            Term t = rstream.next();
            switch(t.type) {

                case TQuote:
                    QuoteStream nq = evalres(t.qvalue().tokens(), symbols);
                    r.add(new Term<Quote>(Type.TQuote, new CmdQuote(nq)));
                    break;
                case TSymbol:
                    // do we have it in our symbol table? if yes, replace, else just push it in.
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

    static void evaltmpl(TokenStream tmpl, TokenStream elem, HashMap<String, Term> symbols) {
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
                                        evaltmpl(tmplterm.qvalue().tokens(), lastelem.qvalue().tokens(), symbols);
                                        break;
                                    default:
                                        if (tmplterm.value().equals(lastelem.value()))
                                            break;
                                        else
                                            throw new VException("err:view:eq",lastelem,tmplterm.value() + " != "+lastelem.value());
                                }

                            } else {
                                // we can happily slurp now.
                                while(estream.hasNext())
                                    nlist.add(estream.next());
                            }
                            if (value.length() > 1) { // do we have a named list?
                                symbols.put(value, new Term<Quote>(Type.TQuote, new CmdQuote(nlist)));
                            }
                        } else {
                            Term e = estream.next();
                            symbols.put(t.value(), e);
                        }
                        break;
                    } catch (VException e) {
                        e.addLine(t.value());
                        throw e;
                    } catch (Exception e) {
                        throw new VException("err:view:sym",t,t.value() + " " + e.getMessage());
                    }

                case TQuote:
                    // evaluate this portion again in evaltmpl.
                    try {
                        Term et = estream.next();
                        evaltmpl(t.qvalue().tokens(), et.qvalue().tokens(), symbols);
                    } catch (VException e) {
                        e.addLine(t.value());
                        throw e;
                    } catch (Exception e) {
                        throw new VException("err:view:quote",t,t.value() + " " + e.getMessage());
                    }
                    break;
                default:
                    //make sure both matches.
                    Term eterm = estream.next();
                    if (t.value().equals(eterm.value()))
                        break;
                    else
                        throw new VException("err:view:eq",eterm, t.value() + " != " +eterm.value());
            }
        }
        if (estream.hasNext()) {
            Term t = estream.next();
            throw new VException("err:view:more",t, t.value() + " is not allowed.");
        }
    }

    static Cmd _def = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
            Term t = p.pop();
            Map.Entry<String, CmdQuote> entry = splitdef(t.qvalue());
            String symbol = entry.getKey();

            // we define it on the enclosing scope. because the evaluation
            // is done on child scope.
            V.debug("Def [" + symbol + "] @ " + q.id());
            q.parent().def(symbol, entry.getValue());
        }
    };

    static Cmd _set = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
            Term t = p.pop();
            Map.Entry<String, CmdQuote> entry = splitdef(t.qvalue());
            String symbol = entry.getKey();

            // we define it on the enclosing scope. because the evaluation
            // is done on child scope.
            V.debug("Set! [" + symbol + "] @ " + q.id());
            q.parent().set(symbol, entry.getValue());
        }
    };

    static Cmd _me = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            p.push(new Term<VFrame>(Type.TFrame, q.parent()));
        }
    };

    static Cmd _parent = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
            VFrame t = p.pop().fvalue();
            p.push(new Term<VFrame>(Type.TFrame, t.parent()));
        }
    };

    static Cmd _defenv = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
            Term b = p.pop();
            Term t = p.pop();

            Map.Entry<String, CmdQuote> entry = splitdef(t.qvalue());
            String symbol = entry.getKey();
            b.fvalue().def(symbol, entry.getValue());
        }
    };

    static Cmd _defmodule = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
            Term t = p.pop();

            Map.Entry<String, CmdQuote> entry = splitdef(t.qvalue());
            String module = entry.getKey();
            CmdQuote qfull = entry.getValue();

            // split it again to get exported defs. 
            Iterator<Term> it = (Iterator<Term>)qfull.tokens().iterator();
            Quote pub = it.next().qvalue();

            QuoteStream nts = new QuoteStream();
            while (it.hasNext())
                nts.add(it.next());

            // we define it on the enclosing scope.
            // so our new command's parent is actually q rather than
            // parent.
            CmdQuote qval = new CmdQuote(nts); 

            // now evaluate the entire thing on the current env.
            qval.eval(q);

            // and save the frame in our parents namespace.
            Term<VFrame> f = new Term<VFrame>(Type.TFrame, q);
            QuoteStream fts = new QuoteStream();
            fts.add(f);
            V.debug("Def :" + module + "@" + q.parent().id());
            q.parent().def('$' + module, new CmdQuote(fts));

            // now bind all the published tokens to our parent namespace.
            Iterator <Term> i = pub.tokens().iterator();
            while(i.hasNext()) {
                // look up their bindings and rebind it to parents.
                String s = i.next().value();
                Quote libs = Util.getdef('$' + module + '[' + s + "] &i");
                q.parent().def(module + ':' + s ,libs);
            }

        }
    };

    // [a b c obj method] java
    static Cmd _java = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
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
            Term res = Helper.invoke(object, method, new CmdQuote(qs));
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

    static Cmd _view = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
            Term v = p.pop();
            Iterator<Term> fstream = v.qvalue().tokens().iterator();

            // iterate through the quote, and find where ':' is then split it
            // into two half and analyze the first.
            QuoteStream tmpl = new QuoteStream();
            while (fstream.hasNext()) {
                Term t = fstream.next();
                if (t.type == Type.TSymbol && t.value() == Sym.lookup(":"))
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
            //Now take each elem and its pair templ and extract the symbols and their meanings.
            evaltmpl(tmpl, elem, symbols);

            // now go over the quote we were just passed and replace each symbol with what we
            // have if we do have a definition.
            QuoteStream resstream = evalres(res, symbols);
            CmdQuote qs = new CmdQuote(resstream);

            Iterator<Term> i = qs.tokens().iterator();
            while (i.hasNext())
                p.push(i.next());
        }
    };

    // trans looks for a [[xxx] [yyy]] instead of splitting with [xxx : yyy]
    static Cmd _trans = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
            Term v = p.pop();
            Iterator<Term> fstream = v.qvalue().tokens().iterator();

            QuoteStream tmpl = (QuoteStream)fstream.next().qvalue().tokens();

            QuoteStream res = new QuoteStream();
            while (fstream.hasNext())
                res.add(fstream.next());

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
            //Now take each elem and its pair templ and extract the symbols and their meanings.
            evaltmpl(tmpl, elem, symbols);

            // now go over the quote we were just passed and replace each symbol with what we
            // have if we do have a definition.
            QuoteStream resstream = evalres(res, symbols);
            CmdQuote qs = new CmdQuote(resstream);

            Iterator<Term> i = qs.tokens().iterator();
            while (i.hasNext())
                p.push(i.next());
        }
    };

    static Cmd _words = new Cmd() {
        public void eval(VFrame q) {
            // eval is passed in the quote representing the current scope.
            VStack p = q.stack();
            VFrame f = p.pop().fvalue();

            // copy the rest of tokens to our own stream.
            QuoteStream nts = new QuoteStream();
            for(String s: sort(f.dict().keySet()))
                nts.add(new Term<String>(Type.TSymbol,s));
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };

    static Cmd _catch = new Cmd() {
        @SuppressWarnings("unchecked")
            public void eval(VFrame q) {
                // eval is passed in the quote representing the current scope.
                VStack p = q.stack();
                Term c = p.pop();
                Term expr = p.pop();
                try {
                    expr.qvalue().eval(q);
                } catch (VException ve) {
                    p.push((Term)ve.token());
                    c.qvalue().eval(q);
                }
            }
    };

    static Cmd _throw = new Cmd() {
        public void eval(VFrame q) {
            Term t = q.stack().pop();
            throw new VException("err:throw", t, t.value());
        }
    };

    static Cmd _stack = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            q.stack().push(new Term<Quote>(Type.TQuote, p.quote()));
        }
    };

    static Cmd _unstack = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term t = p.pop();
            p.dequote(t.qvalue());
        }
    };


    static Cmd _abort = new Cmd() {
        public void eval(VFrame q) {
            q.stack().clear();
        }
    };

    static Cmd _true = new Cmd() {
        public void eval(VFrame q) {
            q.stack().push(new Term<Boolean>(Type.TBool, true));
        }
    };

    static Cmd _false = new Cmd() {
        public void eval(VFrame q) {
            q.stack().push(new Term<Boolean>(Type.TBool, false));
        }
    };

    // Control structures
    static Cmd _if = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

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
                action.qvalue().eval(q);
        }
    };

    static Cmd _when = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

            Term wquote = p.pop();

            Iterator<Term> fstream = wquote.qvalue().tokens().iterator();

            while (fstream.hasNext()) {
                // extract the relevant element from list,
                Term cond = fstream.next();
                Term action = fstream.next();

                if (cond.type == Type.TQuote) {
                    Node<Term> n = p.now;
                    cond.qvalue().eval(q);
                    // and get it back from stack.
                    cond = p.pop();
                    p.now = n;
                }

                // apply the action
                if (cond.bvalue()) {
                    action.qvalue().eval(q);
                    break;
                }
            }
        }
    };

    static Cmd _choice = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

            Term af = p.pop();
            Term at = p.pop();
            Term cond = p.pop();

            if (cond.bvalue())
                p.push(at);
            else
                p.push(af);
        }
    };


    static Cmd _ifte = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

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
                action.qvalue().eval(q);
            else
                eaction.qvalue().eval(q);
        }
    };

    static Cmd _while = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

            Term action = p.pop();
            Term cond = p.pop();
            while(true) {
                Term ncond = cond;
                if (cond.type == Type.TQuote) {
                    Node<Term> n = p.now;
                    cond.qvalue().eval(q);
                    // and get it back from stack.
                    ncond = p.pop();
                    p.now = n;
                }
                // dequote the action and push it to stack.
                if (ncond.bvalue())
                    action.qvalue().eval(q);
                else
                    break;
            }
        }
    };

    // Libraries
    static Cmd _print = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term t = p.pop();
            V.out(t.value());
        }
    };

    static Cmd _println = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term t = p.pop();
            V.outln(t.value());
        }
    };

    static Cmd _gets = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            p.push(new Term<String>(Type.TString, V.gets()));
        }
    };

    static Cmd _peek = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            if (p.empty()) {
                V.outln("");
            } else {
                Term t = p.peek();
                V.outln(t.value());
            }
        }
    };

    static Cmd _show = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            p.dump();
        }
    };

    static Collection<String> sort(Set<String> ks) {
        LinkedList<String> ll = new LinkedList(ks);
        Collections.sort(ll);
        return ll;
    }

    static Cmd _vdebug = new Cmd() {
        public void eval(VFrame q) {
            V.outln(q.parent().id());
              q.stack().dump();
              for(String s: sort(q.parent().dict().keySet())) V.out(s + " ");
              V.outln("\n________________");
        }
    };

    static Cmd _dframe = new Cmd() {
        public void eval(VFrame q) {
            q.stack().dump();
            q = q.parent(); // remove the current child frame.
            while(q != null) {
                dumpframe(q);
                q = q.parent();
            }
        }
        public void dumpframe(VFrame q) {
            V.outln(q.id());
            for(String s: sort(q.dict().keySet())) V.out(s + " ");
            V.outln("\n________________");
        }
    };

    static Cmd _debug = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            V.debug(p.pop().bvalue());
        }
    };

    // this map is not a stack invariant. specifically 
    // 1 2 3 4  [a b c d] [[] cons cons] map => [[4 a] [3 b] [2 c] [1 d]]
    static Cmd _map = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

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
                action.qvalue().eval(q);
                // pop it back into a new quote
                Term res = p.pop();
                nts.add(res);
            }
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };

    // map is a stack invariant. specifically 
    // 1 2 3 4  [a b c d] [[] cons cons] map => [[4 a] [4 b] [4 c] [4 d]]
    static Cmd _map_i = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

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
                action.qvalue().eval(q);
                // pop it back into a new quote
                Term res = p.pop();
                p.now = n;
                nts.add(res);
            }
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };

    static Cmd _split = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

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
                action.qvalue().eval(q);
                // pop it back into a new quote
                Term res = p.pop();
                if (res.bvalue())
                    nts1.add(t);
                else
                    nts2.add(t);
            }
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts1)));
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts2)));
        }
    };

    static Cmd _split_i = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

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
                action.qvalue().eval(q);
                // pop it back into a new quote
                Term res = p.pop();
                p.now = n;
                if (res.bvalue())
                    nts1.add(t);
                else
                    nts2.add(t);
            }
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts1)));
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts2)));
        }
    };

    static Cmd _fold = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

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
            }
            Term res = p.pop();
            p.push(res);
            // the result will be on the stack at the end of this cycle.
        }
    };

    static Cmd _fold_i = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

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
                action.qvalue().eval(q);
            }
            Term res = p.pop();
            p.now = n;
            p.push(res);
            // the result will be on the stack at the end of this cycle.
        }
    };

    static Cmd _drop = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term i = p.pop();
            int num = i.ivalue();
            Term list = p.pop();

            QuoteStream nts = new QuoteStream();

            for(Term t: list.qvalue().tokens()) {
                if (num <= 0)
                    nts.add(t);
                --num;
            }
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };

    static Cmd _take = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term i = p.pop();
            int num = i.ivalue();
            Term list = p.pop();
            int count = 0;

            QuoteStream nts = new QuoteStream();

            for(Term t: list.qvalue().tokens()) {
                if (count >= num)
                    break;
                ++count;
                nts.add(t);
            }
            p.push(new Term<Quote>(Type.TQuote, new CmdQuote(nts)));
        }
    };



    static Cmd _dequote = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

            Term prog = p.pop();
            prog.qvalue().eval(q); // apply on parent
        }
    };

    static Cmd _dequoteenv = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();

            Term prog = p.pop();
            Term env = p.pop();
            prog.qvalue().eval(env.fvalue()); // apply on parent
        }
    };

    static Cmd _add = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
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

    static Cmd _sub = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
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

    static Cmd _mul = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
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

    static Cmd _div = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
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

    static Cmd _mod = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            int ires = a.numvalue().intValue() % b.numvalue().intValue();
            p.push(new Term<Integer>(Type.TInt, ires));
        }
    };

    static Cmd _gt = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, isGt(a, b)));
        }
    };

    static Cmd _lt = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, isLt(a, b)));
        }
    };

    static Cmd _lteq = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, !isGt(a, b)));
        }
    };

    static Cmd _gteq = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, !isLt(a, b)));
        }
    };

    static Cmd _eq = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, isEq(a, b)));
        }
    };

    static Cmd _neq = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, !isEq(a, b)));
        }
    };

    static Cmd _and = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, and(a, b)));
        }
    };

    static Cmd _or = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term b = p.pop();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, or(a, b)));
        }
    };

    static Cmd _not = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, !a.bvalue()));
        }
    };


    // Predicates do not consume the element. 
    static Cmd _isbool = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, a.type == Type.TBool));
        }
    };

    static Cmd _isinteger = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, a.type == Type.TInt));
        }
    };

    static Cmd _isdouble = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, a.type == Type.TDouble));
        }
    };

    static Cmd _issym = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, a.type == Type.TSymbol));
        }
    };

    static Cmd _islist = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, a.type == Type.TQuote));
        }
    };

    static Cmd _isstr = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, a.type == Type.TString));
        }
    };

    static Cmd _isnum = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool,
                        a.type == Type.TInt || a.type == Type.TDouble));
        }
    };

    static Cmd _ischar = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Boolean>(Type.TBool, a.type == Type.TChar));
        }
    };

    static Cmd _tostring = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<String>(Type.TString, a.value()));
        }
    };

    static Cmd _toint = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Integer>(Type.TInt, (new Double(a.value())).intValue()));
        }
    };

    static Cmd _todecimal = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term a = p.pop();
            p.push(new Term<Double>(Type.TDouble, new Double(a.value())));
        }
    };

    private static String getFileName(String s) {
        if (s.endsWith(".v")) return s;
        return s + ".v";
    }
    /* stdlib.v
     * [stdlib 
     *      [qsort  xxx yyy].
     *      [binsearch aaa bbb].
     * ]
     *
     * 'stdlib' use
     * [1 7 3 2 2] stdlib:qsort
     * */
    static Cmd _use = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term file = p.pop();
            try {
                if (file.type == Type.TQuote) {
                    Iterator<Term> files = (Iterator<Term>)file.qvalue().tokens().iterator();
                    while(files.hasNext()) {
                        Term f = files.next();
                        String val = getFileName(f.svalue());
                        // Try and see if the file requested is any of the standard defined
                        String chars = Util.getresource(val);
                        CharStream cs = chars == null? new FileCharStream(val) : new BuffCharStream(chars);
                        CmdQuote module = new CmdQuote(new LexStream(cs));
                        module.eval(q.parent());
                    }
                } else {
                    String val = getFileName(file.svalue());
                    // Try and see if the file requested is any of the standard defined
                    String chars = Util.getresource(val);
                    CharStream cs = chars == null? new FileCharStream(val) : new BuffCharStream(chars);
                    CmdQuote module = new CmdQuote(new LexStream(cs));
                    module.eval(q.parent());
                }
                V.debug("use @ " + q.id());
            } catch (VException e) {
                e.addLine("use " + file.value());
                throw e;
            } catch (Exception e) {
                throw new VException("err:use",file, file.value());
            }
        }
    };

    static Cmd _useenv = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term env = p.pop();
            Term file = p.pop();
            try {
                if (file.type == Type.TQuote) {
                    Iterator<Term> files = (Iterator<Term>)file.qvalue().tokens().iterator();
                    while(files.hasNext()) {
                        Term f = files.next();
                        String val = getFileName(f.svalue());
                        // Try and see if the file requested is any of the standard defined
                        String chars = Util.getresource(val);
                        CharStream cs = chars == null? new FileCharStream(val) : new BuffCharStream(chars);
                        CmdQuote module = new CmdQuote(new LexStream(cs));
                        module.eval(env.fvalue());
                    }
                } else {
                    String val = getFileName(file.svalue());
                    // Try and see if the file requested is any of the standard defined
                    String chars = Util.getresource(val);
                    CharStream cs = chars == null? new FileCharStream(val) : new BuffCharStream(chars);
                    CmdQuote module = new CmdQuote(new LexStream(cs));
                    module.eval(env.fvalue());
                }
                V.debug("use @ " + q.id());
            } catch (VException e) {
                e.addLine("use " + file.value());
                throw e;
            } catch (Exception e) {
                throw new VException("err:*use",file,file.value());
            }
        }
    };

    static Cmd _eval = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term buff = p.pop();
            try {
                Util.evaluate(buff.svalue(), q.parent());
                V.debug("eval @ " + q.id());
            } catch (VException e) {
                e.addLine("eval " + buff.value());
                throw e;
            } catch (Exception e) {
                throw new VException("err:eval",buff, buff.value());
            }
        }
    };

    static Cmd _evalenv = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Quote qenv = p.pop().qvalue();
            VFrame env = qenv.tokens().iterator().next().fvalue();
            Term buff = p.pop();
            try {
                Util.evaluate(buff.svalue(), env);
            } catch (VException e) {
                e.addLine("*eval " + buff.value());
                throw e;
            } catch (Exception e) {
                throw new VException("err:*eval",buff, buff.value());
            }
        }
    };

    static Cmd _help = new Cmd() {
        public void eval(VFrame q) {
            HashMap <String,Quote> bind = q.dict();
            for(String s : new TreeSet<String>(bind.keySet()))
                V.outln(s);
        }
    };

    static Cmd _env = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Quote env = Util.getdef("[platform java]");
            p.push(new Term<Quote>(Type.TQuote, env));
        }
    };

    static Cmd _time = new Cmd() {
        public void eval(VFrame q) {
            VStack p = q.stack();
            Term t = p.pop();
            boolean val = t.bvalue();
            V.showtime(val);
        }
    };

    public static void init(final VFrame iframe) {
        // accepts a quote as an argument.
        //meta
        iframe.def(".", _def);
        iframe.def("&.", _defenv);
        iframe.def(".!", _set);
        iframe.def("module", _defmodule);
        iframe.def("&words", _words);
        iframe.def("&parent", _parent);
        iframe.def("$me", _me);

        iframe.def("&i", _dequoteenv);
        iframe.def("i", _dequote);

        iframe.def("view", _view);
        iframe.def("trans", _trans);
        iframe.def("java", _java);

        iframe.def("true", _true);
        iframe.def("false", _false);
        iframe.def("catch", _catch);
        iframe.def("throw", _throw);
        iframe.def("$stack", _stack);
        iframe.def("stack!", _unstack);

        iframe.def("and", _and);
        iframe.def("or", _or);
        iframe.def("not", _not);

        //control structures
        iframe.def("ifte", _ifte);
        iframe.def("if", _if);
        iframe.def("when", _when);
        iframe.def("while", _while);
        iframe.def("choice", _choice);

        //io
        iframe.def("put", _print);
        iframe.def("puts", _println);

        iframe.def("gets", _gets);

        //others
        iframe.def("?", _peek);
        iframe.def("??", _show);
        iframe.def("?debug", _vdebug);
        iframe.def("?stack", _show);
        iframe.def("?frame", _dframe);
        iframe.def(".debug", _debug);

        iframe.def("abort", _abort);

        //list
        iframe.def("drop", _drop);
        iframe.def("take", _take);


        // on list
        iframe.def("map!", _map);
        iframe.def("map", _map_i);
        iframe.def("split!", _split);
        iframe.def("split", _split_i);
        iframe.def("fold!", _fold);
        iframe.def("fold", _fold_i);

        //arith
        iframe.def("+", _add);
        iframe.def("-", _sub);
        iframe.def("*", _mul);
        iframe.def("/", _div);
        iframe.def("%", _mod);

        //bool
        iframe.def("=", _eq);
        iframe.def("==", _eq);
        iframe.def("!=", _neq);
        iframe.def(">", _gt);
        iframe.def("<", _lt);
        iframe.def("<=", _lteq);
        iframe.def(">=", _gteq);

        //predicates
        iframe.def("integer?", _isinteger);
        iframe.def("double?", _isdouble);
        iframe.def("boolean?", _isbool);
        iframe.def("symbol?", _issym);
        iframe.def("list?", _islist);
        iframe.def("char?", _ischar);
        iframe.def("number?", _isnum);
        iframe.def("string?", _isstr);

        iframe.def(">string", _tostring);
        iframe.def(">int", _toint);
        iframe.def(">decimal", _todecimal);

        //modules
        iframe.def("use", _use);
        iframe.def("&use", _useenv);
        iframe.def("eval", _eval);
        iframe.def("&eval", _evalenv);

        iframe.def("help", _help);
        iframe.def("env", _env);
        
        iframe.def(".time!", _time);

        Quote libs = Util.getdef("'std' use");
        libs.eval(iframe);
    }
}

