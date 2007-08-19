#include <stack>
#include <map>
#include "prologue.h"
#include "token.h"
#include "term.h"
#include "cmd.h"
#include "cmdquote.h"
#include "vstack.h"
#include "vframe.h"
#include "quotestream.h"
#include "quoteiterator.h"
#include "filecharstream.h"
#include "lexstream.h"
#include "vexception.h"
#include "v.h"
#include "sym.h"
/*char* buff =
#include "std.h"
;*/
// no cmp_str since constant strings.
typedef std::map<char*, P<Token> > SymbolMap;
typedef std::pair<char*, P<Quote> > SymPair;

SymPair splitdef(Quote* qval) {
    P<TokenIterator> it = qval->tokens()->iterator();
    P<Token> symbol = it->next();

    P<QuoteStream> nts = new (collect) QuoteStream();
    while(it->hasNext())
        nts->add(it->next());

    return std::make_pair<char*, P<Quote> >(symbol->svalue(),
            new (collect) CmdQuote(nts));
}

char* special(char* name) {
    int len = strlen(name);
    P<char,true> buf = new (collect) char[len + 2];
    buf[0] = '$';
    std::strcpy(buf+1, name);
    return buf;
}

void evaltmpl(TokenStream* tmpl, TokenStream* elem, SymbolMap& symbols) {
    //Take each point in tmpl, and proess elements accordingly.
    P<TokenIterator> tstream = tmpl->iterator();
    P<TokenIterator> estream = elem->iterator();
    while(tstream->hasNext()) {
        P<Token> t = tstream->next();
        switch (t->type()) {
            case TSymbol:
                try {
                    // _ means any one
                    // * means any including nil unnamed.
                    // *a means any including nil but named with symbol '*a'
                    P<char,true> value = t->svalue();
                    if (value[0] == '_') {
                        // eat one from estream and continue.
                        estream->next();
                        break;
                    } else if (value[0] == '*') {
                        P<QuoteStream> nlist = new (collect) QuoteStream();
                        // * is all. but before we slurp, check the next element
                        // in the template. If there is not any, then slurp. If there
                        // is one, then slurp until last but one, and leave it.
                        if (tstream->hasNext()) {
                            P<Token> tmplterm = tstream->next();
                            P<Token> lastelem = 0;

                            // slurp till last but one.
                            while(estream->hasNext()) {
                                lastelem = estream->next();
                                if (estream->hasNext())
                                    nlist->add(lastelem);
                            }

                            switch (tmplterm->type()) {
                                case TSymbol:
                                    // assign value in symbols.
                                    symbols[tmplterm->svalue()] = lastelem;
                                    break;
                                case TQuote:
                                    evaltmpl(tmplterm->qvalue()->tokens(), lastelem->qvalue()->tokens(), symbols);
                                    break;
                                default:
                                    if (!strcmp(tmplterm->value(),lastelem->value()))
                                        break;
                                    else
                                        throw VException("err:view:eq", lastelem, "%s != %s",tmplterm->value(), lastelem->value());
                            }

                        } else {
                            // we can happily slurp now.
                            while(estream->hasNext())
                                nlist->add(estream->next());
                        }
                        if (strlen(value) > 1) { // do we have a named list?
                            symbols[value] = new (collect) Term(TQuote, new (collect) CmdQuote(nlist));
                        }
                    } else {
                        P<Token> e = estream->next();
                        symbols[value] = e;
                    }
                    break;
                } catch (VException& e) {
                    throw e;
                } catch (...) {
                    throw VException("err:view:sym", t,t->value());
                }

            case TQuote:
                // evaluate this portion again in evaltmpl.
                try {
                    P<Token> et = estream->next();
                    evaltmpl(t->qvalue()->tokens(), et->qvalue()->tokens(), symbols);
                } catch (VException& e) {
                    throw e;
                } catch (...) {
                    throw VException("err:view:quote", t, t->value());
                }
                break;
            default:
                //make sure both matches.
                P<Token> eterm = estream->next();
                if (!strcmp(t->value(),eterm->value()))
                    break;
                else
                    throw VException("err:view:eq", eterm, "%s != %s" ,t->value(), eterm->value());
        }
    }

}

bool containsKey(SymbolMap& symbols, char* key) {
    if (symbols.find(key) != symbols.end()) {
        return true;
    }
    return false;
}

TokenStream* evalres(TokenStream* res, SymbolMap& symbols) {
        P<QuoteStream> r = new (collect) QuoteStream();
        P<TokenIterator> rstream = res->iterator();
        while(rstream->hasNext()) {
            P<Token> t = rstream->next();
            switch(t->type()) {

                case TQuote:
                    {
                        P<QuoteStream> nq = 
                            (QuoteStream*)evalres(t->qvalue()->tokens(),
                                    symbols);
                        r->add(new (collect) Term(TQuote, new (collect) CmdQuote(nq)));
                        break;
                    }
                case TSymbol:
                    {
                        // do we have it in our symbol table? if yes, replace,
                        // else just push it in.
                        P<char,true> sym = t->svalue();
                        if (containsKey(symbols, sym)) {
                            // does it look like *xxx ?? 
                            if (sym[0] == '*') {
                                // expand it.
                                P<Token> star = symbols[sym];
                                P<QuoteIterator> tx = (QuoteIterator*)
                                    star->qvalue()->tokens()->iterator();
                                while(tx->hasNext()) {
                                    r->add(tx->next());
                                }
                            } else
                                r->add(symbols[sym]);
                            break;
                        }
                    }
                default:
                    // just push it in.
                    r->add(t);
            }
        }
        return r;

}

bool isEq(Token* a, Token* b) {
    switch(a->type()) {
        case TInt:
        case TDouble:
            return fabs(b->numvalue().d() - a->numvalue().d()) < Precision;
        case TString:
            return !strcmp(a->svalue(), b->svalue());
        case TSymbol:
            return a->svalue() == b->svalue(); // constant strings.
        default:
            return !strcmp(a->value(), b->value());
    }
}

bool isGt(Token* a, Token* b) {
    return a->numvalue().d() > b->numvalue().d();
}

bool isLt(Token* a, Token* b) {
    if (isGt(a,b)) return false;
    if (isEq(a,b)) return false;
    return true;
}

struct Ctrue : public Cmd {
    void eval(VFrame* q) {
        q->stack()->push(new (collect) Term(TBool, true));
    }
    char* to_s() {return "true";}
};

struct Cfalse : public Cmd {
    void eval(VFrame* q) {
        q->stack()->push(new (collect) Term(TBool, false));
    }
    char* to_s() {return "false";}
};

struct Cadd : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        double dres = a->numvalue().d() + b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new (collect) Term(TInt, ires));
        else
            p->push(new (collect) Term(TDouble, dres));
    }
    char* to_s() {return "+";}
};

struct Csub : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        double dres = a->numvalue().d() - b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new (collect) Term(TInt, ires));
        else
            p->push(new (collect) Term(TDouble, dres));
    }
    char* to_s() {return "-";}
};

struct Cmul : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        double dres = a->numvalue().d() * b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new (collect) Term(TInt, ires));
        else
            p->push(new (collect) Term(TDouble, dres));
    }
    char* to_s() {return "*";}
};

struct Cdiv : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        double dres = a->numvalue().d() / b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new (collect) Term(TInt, ires));
        else
            p->push(new (collect) Term(TDouble, dres));
    }
    char* to_s() {return "/";}
};

struct Cand : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        bool res = a->bvalue() && b->bvalue();
        p->push(new (collect) Term(TBool, res));
    }
    char* to_s() {return "and";}
};

struct Cor : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        bool res = a->bvalue() || b->bvalue();
        p->push(new (collect) Term(TBool, res));
    }
    char* to_s() {return "or";}
};

struct Cnot : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        bool res = !a->bvalue();
        p->push(new (collect) Term(TBool, res));
    }
    char* to_s() {return "not";}
};

struct Cisinteger : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TInt));
    }
    char* to_s() {return "int?";}
};

struct Cisdouble : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TDouble));
    }
    char* to_s() {return "decimal?";}
};

struct Cisbool : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TBool));
    }
    char* to_s() {return "bool?";}
};

struct Cissym : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TSymbol));
    }
    char* to_s() {return "symbol?";}
};

struct Cisquote : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TQuote));
    }
    char* to_s() {return "quote?";}
};

struct Cisstr : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TString));
    }
    char* to_s() {return "string?";}
};

struct Cischar : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TChar));
    }
    char* to_s() {return "char?";}
};

struct Cisnum : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TInt || a->type() == TDouble));
    }
    char* to_s() {return "number?";}
};

struct Ctostr : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TString, a->value()));
    }
    char* to_s() {return ">string";}
};

struct Ctoint : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        switch (a->type()) {
            case TInt:
                p->push(a);
                break;
            case TDouble:
                p->push(new (collect) Term(TInt, a->numvalue().i()));
                break;
            case TChar:
                p->push(new (collect) Term(TInt, a->cvalue()));
                break;
            case TString:
                p->push(new (collect) Term(TInt, atol(a->svalue())));
            default:
                throw VException("err:>int", a,"%s cant convert", a->value());
        }
    }
    char* to_s() {return ">int";}
};

struct Ctodouble : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        switch (a->type()) {
            case TDouble:
                p->push(a);
                break;
            case TInt:
                p->push(new (collect) Term(TDouble, a->numvalue().d()));
                break;
            case TChar:
                p->push(new (collect) Term(TDouble, (double)a->cvalue()));
                break;
            case TString:
                p->push(new (collect) Term(TDouble, atof(a->svalue())));
            default:
                throw VException("err:>decimal", a,"%s cant convert", a->value());
        }
    }
    char* to_s() {return ">decimal";}
};

struct Ctobool : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        switch (a->type()) {
            case TInt:
                p->push(new (collect) Term(TBool, a->ivalue() != 0));
                break;
            case TDouble:
                p->push(new (collect) Term(TBool, a->dvalue() != 0.0));
                break;
            case TChar:
                p->push(new (collect) Term(TBool, a->cvalue() != 'f'));
                break;
            case TString:
                p->push(new (collect) Term(TBool, a->svalue() != Sym::lookup("false")));
                break;
            case TQuote:
                p->push(new (collect) Term(TBool, ((Term*)a.val)->size() != 0));
                break;
            default:
                throw VException("err:>bool", a,"%s cant convert", a->value());
        }
    }
    char* to_s() {return ">bool";}
};

struct Ctochar : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        switch (a->type()) {
            case TInt:
                p->push(a);
                break;
            case TDouble:
                p->push(new (collect) Term(TInt, a->numvalue().i()));
                break;
            case TChar:
                p->push(a);
                break;
            case TString:
                p->push(new (collect) Term(TChar, a->value()[0]));
                break;
            default:
                throw VException("err:>char", a,"%s cant convert", a->value());
        }
    }
    char* to_s() {return ">char";}
};

struct Cgt : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, isGt(a,b)));
    }
    char* to_s() {return ">";}
};

struct Clt : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, isLt(a,b)));
    }
    char* to_s() {return "<";}
};

struct Clteq : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, !isGt(a,b)));
    }
    char* to_s() {return "<=";}
};

struct Cgteq : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, !isLt(a,b)));
    }
    char* to_s() {return ">=";}
};

struct Ceq : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, isEq(a,b)));
    }
    char* to_s() {return "=";}
};

struct Cneq : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> a = p->pop();
        p->push(new (collect) Term(TBool, !isEq(a,b)));
    }
    char* to_s() {return "!=";}
};

struct Cchoice : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> af = p->pop();
        P<Token> at = p->pop();
        P<Token> cond = p->pop();

        if (cond->bvalue())
            p->push(at);
        else
            p->push(af);
    }
    char* to_s() {return "choice";}
};

struct Cif : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> action = p->pop();
        P<Token> cond = p->pop();

        if (cond->type() == TQuote) {
            P<Node> n = p->now();
            cond->qvalue()->eval(q);
            cond = p->pop();
            p->now(n);
        }
        if (cond->bvalue())     
            action->qvalue()->eval(q);
    }
    char* to_s() {return "if";}
};

struct Cifte : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> eaction = p->pop();
        P<Token> action = p->pop();
        P<Token> cond = p->pop();

        if (cond->type() == TQuote) {
            P<Node> n = p->now();
            cond->qvalue()->eval(q);
            cond = p->pop();
            p->now(n);
        }
        if (cond->bvalue())     
            action->qvalue()->eval(q);
        else
            eaction->qvalue()->eval(q);
    }
    char* to_s() {return "ifte";}
};

struct Cwhile : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> action = p->pop();
        P<Token> cond = p->pop();
        while(true) {
            if (cond->type() == TQuote) {
                P<Node> n = p->now();
                cond->qvalue()->eval(q);
                cond = p->pop();
                p->now(n);
            }
            if (cond->bvalue())     
                action->qvalue()->eval(q);
            else
                break;
        }
    }
    char* to_s() {return "while";}
};

struct Cputs : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        V::outln(a->value());
    }
    char* to_s() {return "puts";}
};

struct Cput : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> a = p->pop();
        V::out(a->value());
    }
    char* to_s() {return "put";}
};

struct Cshow : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        p->dump();
    }
    char* to_s() {return "?stack";}
};

struct Cpeek : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        if (p->empty())
            V::outln("");
        else {
            P<Token> t = p->peek();
            V::outln(t->value());
        }
    }
    char* to_s() {return "?";}
};

struct Chelp : public Cmd {
    void eval(VFrame* q) {
        V::outln(q->parent()->words()->to_s());
    }
    char* to_s() {return "help";}
};

struct Cvdebug : public Cmd {
    void eval(VFrame* q) {
        V::outln("Q:%d",q->parent()->id());
        P<VStack> p = q->stack();
        p->dump();
        V::outln(q->parent()->words()->to_s());
    }
    char* to_s() {return "?debug";}
};

struct Cdframe : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        p->dump();
        q = q->parent();
        while(q) {
            dumpframe(q);
            q = q->parent();
        }
    }
    void dumpframe(VFrame* q) {
        V::outln("Q:%d", q->id());
        V::outln(q->words()->to_s());
    }
    char* to_s() {return "?frame";}
};

struct Cview : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> v = p->pop();
        P<TokenIterator> fstream =v->qvalue()->tokens()->iterator();
        P<QuoteStream> tmpl = new (collect) QuoteStream();
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            if (t->type() == TSymbol && (t->svalue() == Sym::lookup(":")))
                break;
            tmpl->add(t);
        }

        P<QuoteStream> res = new (collect) QuoteStream();
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            res->add(t);
        }

        P<QuoteStream> elem = new (collect) QuoteStream();
        fstream = tmpl->iterator();
        std::stack<P<Token> > st;
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            P<Token> e = p->pop();
            st.push(e);
        }
        while(st.size()) {
            elem->add(st.top());
            st.pop();
        }
        SymbolMap symbols;
        evaltmpl(tmpl, elem, symbols);

        P<TokenStream> resstream = evalres(res, symbols);
        P<CmdQuote> qs = new (collect) CmdQuote(resstream);
        P<TokenIterator> i = qs->tokens()->iterator();
        while(i->hasNext())
            p->push(i->next());
    }
    char* to_s() {return "view";}
};

struct Ctrans : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> v = p->pop();
        P<TokenIterator> fstream =v->qvalue()->tokens()->iterator();
        P<TokenStream> tmpl = fstream->next()->qvalue()->tokens();
        
        P<QuoteStream> res = new (collect) QuoteStream();
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            res->add(t);
        }

        P<QuoteStream> elem = new (collect) QuoteStream();
        fstream = tmpl->iterator();
        std::stack<P<Token> > st;
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            P<Token> e = p->pop();
            st.push(e);
        }
        while(st.size()) {
            elem->add(st.top());
            st.pop();
        }
        SymbolMap symbols;
        evaltmpl(tmpl, elem, symbols);

        P<TokenStream> resstream = evalres(res, symbols);
        P<CmdQuote> qs = new (collect) CmdQuote(resstream);
        P<TokenIterator> i = qs->tokens()->iterator();
        while(i->hasNext())
            p->push(i->next());
    }
    char* to_s() {return "trans";}
};

struct Cdef : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        q->parent()->def(entry.first, entry.second);
    }
    char* to_s() {return ".";}
};

struct Cdefenv : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> b = p->pop();
        P<Token> t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        b->fvalue()->def(entry.first, entry.second);
    }
    char* to_s() {return "&.";}
};

struct Cparent : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<VFrame> t = p->pop()->fvalue();
        p->push(new (collect) Term(TFrame, t->parent()));
    }
    char* to_s() {return "&parent";}
};

struct Cme : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        p->push(new (collect) Term(TFrame, q->parent()));
    }
    char* to_s() {return "$me";}
};

struct Cuse : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> file = p->pop();
        try {
            P<char,true> v = file->svalue();
            int len = strlen(v);
            P<char,true> val = new (collect) char[len + 3];
            std::sprintf(val,"%s%s",v.val,".v");
            P<CmdQuote> module = new (collect) CmdQuote(new (collect) LexStream(new (collect) FileCharStream(val)));
            module->eval(q->parent());
        } catch (VException& e) {
            e.addLine("use %s", file->value());
            throw e;
        } catch (...) {
            throw VException("err:use", file,file->value());
        }
    }
    char* to_s() {return "use";}
};

struct Cuseenv : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> env = p->pop();
        P<Token> file = p->pop();
        try {
            P<char,true> v = file->svalue();
            int len = strlen(v);
            P<char,true> val = new (collect) char[len + 3];
            std::sprintf(val,"%s%s",v.val,".v");
            P<CmdQuote> module = new (collect) CmdQuote(new (collect) LexStream(new (collect) FileCharStream(val)));
            module->eval(env->fvalue());
        } catch (VException& e) {
            e.addLine("*use %s", file->value());
        } catch (...) {
            throw VException("err:*use", file, "%s %s",env->value(), file->value());
        }
    }
    char* to_s() {return "&use";}
};

struct Ceval : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> str = p->pop();
        try {
            P<char,true> v = str->svalue();
            P<CmdQuote> module = new (collect) CmdQuote(new (collect) LexStream(new (collect) BuffCharStream(v)));
            module->eval(q->parent());
        } catch (VException& e) {
            e.addLine("eval %s", str->value());
        } catch (...) {
            throw VException("err:eval", str, str->value());
        }
    }
    char* to_s() {return "eval";}
};

struct Cevalenv : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> env = p->pop();
        P<Token> str = p->pop();
        try {
            P<char,true> v = str->svalue();
            P<CmdQuote> module = new (collect) CmdQuote(new (collect) LexStream(new (collect) BuffCharStream(v)));
            module->eval(env->fvalue());
        } catch (VException& e) {
            e.addLine("*eval %s", str->value());
        } catch (...) {
            throw VException("err:*eval", str,"%s %s", env->value(), str->value());
        }
    }
    char* to_s() {return "&eval";}
};

struct Cmodule : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        P<char,true> module = entry.first;
        P<Quote> qfull = entry.second;

        P<TokenIterator> it = qfull->tokens()->iterator();
        P<Quote> pub = it->next()->qvalue();

        P<QuoteStream> nts = new (collect) QuoteStream();
        while(it->hasNext())
            nts->add(it->next());

        P<CmdQuote> qval = new (collect) CmdQuote(nts);
        qval->eval(q);

        P<Term> f = new (collect) Term(TFrame, q);

        P<QuoteStream> fts = new (collect) QuoteStream();
        fts->add(f);
        q->parent()->def(special(module), new (collect) CmdQuote(fts));

        // bind all published tokens to parent namespace.
        P<TokenIterator> i = pub->tokens()->iterator();
        while(i->hasNext()) {
            P<char,true> s = i->next()->svalue();
            P<char,true> def = new (collect) char[strlen(s) + strlen(module) + 9]; // sizeof("$ [ ] &i");
            sprintf(def, "$%s[%s] &i", module.val, s.val);
            P<Quote> libs = CmdQuote::getdef(def);
            sprintf(def, "%s:%s", module.val, s.val);
            q->parent()->def(def, libs);
        }
    }
    char* to_s() {return "module";}
};

struct Cwords : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<VFrame> b = p->pop()->fvalue();
        p->push(new (collect) Term(TQuote, b->words())); 
    }
    char* to_s() {return "&words";}
};

struct Cdequote : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> prog = p->pop();
        prog->qvalue()->eval(q);
    }
    char* to_s() {return "i";}
};

struct Cdequoteenv : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> prog = p->pop();
        P<Token> env = p->pop();
        prog->qvalue()->eval(env->fvalue());
    }
    char* to_s() {return "&i";}
};

struct Cstack : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        q->stack()->push(new (collect) Term(TQuote, p->quote()));
    }
    char* to_s() {return "stack";}
};

struct Cunstack : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> t = p->pop();
        p->dequote(t->qvalue());
    }
    char* to_s() {return "unstack";}
};

struct Cabort : public Cmd {
    void eval(VFrame* q) {
        q->stack()->clear();
    }
    char* to_s() {return "abort";}
};

struct Csize : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> t = p->pop();
        q->stack()->push(new (collect) Term(TInt, (long)((Term*)t.val)->size()));
    }
    char* to_s() {return "size";}
};

struct Cin : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> i = p->pop();
        P<Token> list = p->pop();
        P<TokenIterator> ti = list->qvalue()->tokens()->iterator();
        while(ti->hasNext()) {
            P<Token> t = ti->next();
            if (t->type() == i->type() && isEq(t, i)) {
                p->push(new (collect) Term(TBool, true));
                return;
            }
        }
        p->push(new (collect) Term(TBool, false));
    }
    char* to_s() {return "in?";}
};

struct Cat : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> i = p->pop();
        int idx = i->ivalue();
        P<Token> list = p->pop();
        P<TokenIterator> ti = list->qvalue()->tokens()->iterator();
        int count = 0;
        while(ti->hasNext()) {
            P<Token> t = ti->next();
            if (count == idx) {
                p->push(t);
                return;
            }
            ++count;
        }
        throw VException("err:at:overflow", i,"[%s]:%d",list->value(), idx);
    }
    char* to_s() {return "at";}
};

struct Cmap : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();

        P<QuoteStream> nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            P<Token> res = p->pop();
            nts->add(res);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "map!";}
};

struct Cmap_i : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();

        P<QuoteStream> nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            P<Node> n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            P<Token> res = p->pop();
            p->now(n);
            nts->add(res);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "map";}
};

struct Csplit : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();

        P<QuoteStream> nts1 = new (collect) QuoteStream(); 
        P<QuoteStream> nts2 = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            P<Token> res = p->pop();
            if (res->bvalue())
                nts1->add(t);
            else
                nts2->add(t);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts1)));
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts2)));
    }
    char* to_s() {return "split!";}
};

struct Csplit_i : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();

        P<QuoteStream> nts1 = new (collect) QuoteStream(); 
        P<QuoteStream> nts2 = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            P<Node> n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            P<Token> res = p->pop();
            p->now(n);
            if (res->bvalue())
                nts1->add(t);
            else
                nts2->add(t);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts1)));
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts2)));
    }
    char* to_s() {return "split";}
};

struct Cfilter : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();

        P<QuoteStream> nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            P<Token> res = p->pop();
            if (res->bvalue())
                nts->add(t);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "filter!";}
};

struct Cfilter_i : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();

        P<QuoteStream> nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            P<Node> n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            P<Token> res = p->pop();
            p->now(n);
            if (res->bvalue())
                nts->add(t);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "filter";}
};

struct Cfold : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> init = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();
        p->push(init);
        P<QuoteStream> nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
        // The result will be on the stack at the end of the cycle.
    }
    char* to_s() {return "fold!";}
};

struct Cfold_i : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> init = p->pop();
        P<Token> list = p->pop();

        P<Node> n = p->now();
        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();
        p->push(init);
        P<QuoteStream> nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
        // The result will be on the stack at the end of the cycle.
        P<Token> res = p->pop();
        p->now(n);
        p->push(res);
    }
    char* to_s() {return "fold";}
};

struct Cstep : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
    }
    char* to_s() {return "step!";}
};

struct Cstep_i : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        P<Token> action = p->pop();
        P<Token> list = p->pop();

        P<TokenIterator> fstream = list->qvalue()->tokens()->iterator();
        while(fstream->hasNext()) {
            P<Token> t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
    }
    char* to_s() {return "step";}
};

struct Cdrop : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        int num = p->pop()->ivalue();
        P<Token> list = p->pop();

        P<QuoteStream> nts = new (collect) QuoteStream();
        P<TokenIterator> i = list->qvalue()->tokens()->iterator();
        while(i->hasNext()) {
            P<Token> t = i->next();
            if (num <= 0)
                nts->add(t);
            --num;
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "drop";}
};

struct Ctake : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();

        int num = p->pop()->ivalue();
        P<Token> list = p->pop();
        int count = 0;

        P<QuoteStream> nts = new (collect) QuoteStream();
        P<TokenIterator> i = list->qvalue()->tokens()->iterator();
        while(i->hasNext()) {
            P<Token> t = i->next();
            if (count >= num)
                break;
            ++count;
            nts->add(t);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "take";}
};

struct Ccatch : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> c = p->pop();
        P<Token> expr = p->pop();
        try {
            expr->qvalue()->eval(q);
        } catch (VException &ve) {
            p->push(ve.token());
            c->qvalue()->eval(q);
        }
    }
    char* to_s() {return "catch";}
};

struct Cthrow : public Cmd {
    void eval(VFrame* q) {
        P<Token> t = q->stack()->pop();
        throw VException("err:throw", t, t->value());
    }
    char* to_s() {return "throw";}
};

struct Cgetdef : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> sym = p->pop();
        P<char,true> symbol = sym->qvalue()->tokens()->iterator()->next()->svalue();
        
        P<Quote> qv = q->lookup(symbol);
        P<TokenIterator> it = qv->tokens()->iterator();

        P<QuoteStream> nts = new (collect) QuoteStream();
        nts->add(new (collect) Term(TSymbol, symbol));
        while(it->hasNext())
            nts->add(it->next());
        P<CmdQuote> res = new (collect) CmdQuote(nts);

        p->push(new (collect) Term(TQuote, res));
    }
    char* to_s() {return ">def";}
};

struct Cenv : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        p->push(new (collect) Term(TQuote, CmdQuote::getdef("platform native")));
    }
    char* to_s() {return "env";}
};

struct Csqrt : public Cmd {
    void eval(VFrame* q) {
        P<VStack> p = q->stack();
        P<Token> t = p->pop();
        double num = t->numvalue().d();
        if (num != fabs(num))
            throw VException("err:sqrt:negetive", t,t->value());
        p->push(new (collect) Term(TDouble, sqrt(num)));
    }
    char* to_s() {return "sqrt";}
};
 
void Prologue::init(VFrame* frame) {

    frame->def(".", new (collect) Cdef());
    frame->def("&.", new (collect) Cdefenv());
    frame->def("&parent", new (collect) Cparent());
    frame->def("$me", new (collect) Cme());
    frame->def("module", new (collect) Cmodule());
    frame->def("&words", new (collect) Cwords());

    frame->def("puts", new (collect) Cputs());
    frame->def("put", new (collect) Cput());

    frame->def("i", new (collect) Cdequote());
    frame->def("&i", new (collect) Cdequoteenv());

    frame->def("view", new (collect) Cview());
    frame->def("trans", new (collect) Ctrans());
    frame->def("use", new (collect) Cuse());
    frame->def("&use", new (collect) Cuseenv());
    frame->def("eval", new (collect) Ceval());
    frame->def("&eval", new (collect) Cevalenv());
    frame->def("stack", new (collect) Cstack());
    frame->def("unstack", new (collect) Cunstack());

    frame->def("true", new (collect) Ctrue());
    frame->def("false", new (collect) Cfalse());

    frame->def("+", new (collect) Cadd());
    frame->def("-", new (collect) Csub());
    frame->def("*", new (collect) Cmul());
    frame->def("/", new (collect) Cdiv());
    
    frame->def("and", new (collect) Cand());
    frame->def("or", new (collect) Cor());
    frame->def("not", new (collect) Cnot());

    frame->def("choice", new (collect) Cchoice());
    frame->def("ifte", new (collect) Cifte());
    frame->def("if", new (collect) Cif());
    frame->def("while", new (collect) Cwhile());
    
    frame->def("=", new (collect) Ceq());
    frame->def("==", new (collect) Ceq());
    frame->def("!=", new (collect) Cneq());
    frame->def(">", new (collect) Cgt());
    frame->def("<", new (collect) Clt());
    frame->def("<=", new (collect) Clteq());
    frame->def(">=", new (collect) Cgteq());

    frame->def("??", new (collect) Cshow());
    frame->def("?stack", new (collect) Cshow());
    frame->def("?", new (collect) Cpeek());
    frame->def("?debug", new (collect) Cvdebug());
    frame->def("?frame", new (collect) Cdframe());
    
    frame->def("int?", new (collect) Cisinteger);
    frame->def("decimal?", new (collect) Cisdouble);
    frame->def("bool?", new (collect) Cisbool());
    frame->def("symbol?", new (collect) Cissym());
    frame->def("list?", new (collect) Cisquote());
    frame->def("char?", new (collect) Cischar());
    frame->def("number?", new (collect) Cisnum());
    frame->def("string?", new (collect) Cisstr());

    frame->def(">string", new (collect) Ctostr());
    frame->def(">int", new (collect) Ctoint());
    frame->def(">decimal", new (collect) Ctodouble());
    frame->def(">bool", new (collect) Ctobool());
    frame->def(">char", new (collect) Ctochar());

    frame->def("abort", new (collect) Cabort());
    frame->def("size", new (collect) Csize());
    frame->def("in?", new (collect) Cin());
    frame->def("at", new (collect) Cat());

    frame->def("step!", new (collect) Cstep);
    frame->def("step", new (collect) Cstep_i);
    frame->def("map!", new (collect) Cmap);
    frame->def("map", new (collect) Cmap_i);
    frame->def("filter!", new (collect) Cfilter);
    frame->def("filter", new (collect) Cfilter_i);
    frame->def("split!", new (collect) Csplit);
    frame->def("split", new (collect) Csplit_i);
    frame->def("fold!", new (collect) Cfold);
    frame->def("fold", new (collect) Cfold_i);

    frame->def("drop", new (collect) Cdrop);
    frame->def("take", new (collect) Ctake);
    
    frame->def("help", new (collect) Chelp);
    frame->def("throw", new (collect) Cthrow);
    frame->def("catch", new (collect) Ccatch);
    frame->def(">def", new (collect) Cgetdef);
    frame->def("env", new (collect) Cenv);

    // math
    frame->def("sqrt", new (collect) Csqrt);

    P<Quote> libs = CmdQuote::getdef("'std' use");
    libs->eval(frame);
}
