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
typedef std::map<char*, Token_ > SymbolMap;
typedef std::pair<char*, Quote_ > SymPair;

SymPair splitdef(Quote* qval) {
    TokenIterator_ it = qval->tokens()->iterator();
    Token_ symbol = it->next();

    QuoteStream_ nts = new (collect) QuoteStream();
    while(it->hasNext())
        nts->add(it->next());

    return std::make_pair<char*, Quote_ >(symbol->svalue(),
            new (collect) CmdQuote(nts));
}

char* special(char* name) {
    int len = strlen(name);
    Char_ buf = new (collect) char[len + 2];
    buf[0] = '$';
    std::strcpy(buf+1, name);
    return buf;
}

void evaltmpl(TokenStream* tmpl, TokenStream* elem, SymbolMap& symbols) {
    //Take each point in tmpl, and proess elements accordingly.
    TokenIterator_ tstream = tmpl->iterator();
    TokenIterator_ estream = elem->iterator();
    while(tstream->hasNext()) {
        Token_ t = tstream->next();
        switch (t->type()) {
            case TSymbol:
                try {
                    // _ means any one
                    // * means any including nil unnamed.
                    // *a means any including nil but named with symbol '*a'
                    Char_ value = t->svalue();
                    if (value[0] == '_') {
                        // eat one from estream and continue.
                        estream->next();
                        break;
                    } else if (value[0] == '*') {
                        QuoteStream_ nlist = new (collect) QuoteStream();
                        // * is all. but before we slurp, check the next element
                        // in the template. If there is not any, then slurp. If there
                        // is one, then slurp until last but one, and leave it.
                        if (tstream->hasNext()) {
                            Token_ tmplterm = tstream->next();
                            Token_ lastelem = 0;

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
                        Token_ e = estream->next();
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
                    Token_ et = estream->next();
                    evaltmpl(t->qvalue()->tokens(), et->qvalue()->tokens(), symbols);
                } catch (VException& e) {
                    throw e;
                } catch (...) {
                    throw VException("err:view:quote", t, t->value());
                }
                break;
            default:
                //make sure both matches.
                Token_ eterm = estream->next();
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
        QuoteStream_ r = new (collect) QuoteStream();
        TokenIterator_ rstream = res->iterator();
        while(rstream->hasNext()) {
            Token_ t = rstream->next();
            switch(t->type()) {

                case TQuote:
                    {
                        QuoteStream_ nq = 
                            (QuoteStream*)evalres(t->qvalue()->tokens(),
                                    symbols);
                        r->add(new (collect) Term(TQuote, new (collect) CmdQuote(nq)));
                        break;
                    }
                case TSymbol:
                    {
                        // do we have it in our symbol table? if yes, replace,
                        // else just push it in.
                        Char_ sym = t->svalue();
                        if (containsKey(symbols, sym)) {
                            // does it look like *xxx ?? 
                            if (sym[0] == '*') {
                                // expand it.
                                Token_ star = symbols[sym];
                                QuoteIterator_ tx = (QuoteIterator*)
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
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
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
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
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
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
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
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
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
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
        bool res = a->bvalue() && b->bvalue();
        p->push(new (collect) Term(TBool, res));
    }
    char* to_s() {return "and";}
};

struct Cor : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
        bool res = a->bvalue() || b->bvalue();
        p->push(new (collect) Term(TBool, res));
    }
    char* to_s() {return "or";}
};

struct Cnot : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        bool res = !a->bvalue();
        p->push(new (collect) Term(TBool, res));
    }
    char* to_s() {return "not";}
};

struct Cisinteger : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TInt));
    }
    char* to_s() {return "int?";}
};

struct Cisdouble : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TDouble));
    }
    char* to_s() {return "decimal?";}
};

struct Cisbool : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TBool));
    }
    char* to_s() {return "bool?";}
};

struct Cissym : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TSymbol));
    }
    char* to_s() {return "symbol?";}
};

struct Cisquote : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TQuote));
    }
    char* to_s() {return "quote?";}
};

struct Cisstr : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TString));
    }
    char* to_s() {return "string?";}
};

struct Cischar : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TChar));
    }
    char* to_s() {return "char?";}
};

struct Cisnum : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, a->type() == TInt || a->type() == TDouble));
    }
    char* to_s() {return "number?";}
};

struct Ctostr : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        p->push(new (collect) Term(TString, a->value()));
    }
    char* to_s() {return ">string";}
};

struct Ctoint : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
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
        VStack_ p = q->stack();
        Token_ a = p->pop();
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
        VStack_ p = q->stack();
        Token_ a = p->pop();
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
        VStack_ p = q->stack();
        Token_ a = p->pop();
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
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, isGt(a,b)));
    }
    char* to_s() {return ">";}
};

struct Clt : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, isLt(a,b)));
    }
    char* to_s() {return "<";}
};

struct Clteq : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, !isGt(a,b)));
    }
    char* to_s() {return "<=";}
};

struct Cgteq : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, !isLt(a,b)));
    }
    char* to_s() {return ">=";}
};

struct Ceq : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, isEq(a,b)));
    }
    char* to_s() {return "=";}
};

struct Cneq : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ a = p->pop();
        p->push(new (collect) Term(TBool, !isEq(a,b)));
    }
    char* to_s() {return "!=";}
};

struct Cchoice : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ af = p->pop();
        Token_ at = p->pop();
        Token_ cond = p->pop();

        if (cond->bvalue())
            p->push(at);
        else
            p->push(af);
    }
    char* to_s() {return "choice";}
};

struct Cif : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ action = p->pop();
        Token_ cond = p->pop();

        if (cond->type() == TQuote) {
            Node_ n = p->now();
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
        VStack_ p = q->stack();
        Token_ eaction = p->pop();
        Token_ action = p->pop();
        Token_ cond = p->pop();

        if (cond->type() == TQuote) {
            Node_ n = p->now();
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
        VStack_ p = q->stack();
        Token_ action = p->pop();
        Token_ cond = p->pop();
        while(true) {
            if (cond->type() == TQuote) {
                Node_ n = p->now();
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
        VStack_ p = q->stack();
        Token_ a = p->pop();
        V::outln(a->value());
    }
    char* to_s() {return "puts";}
};

struct Cput : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ a = p->pop();
        V::out(a->value());
    }
    char* to_s() {return "put";}
};

struct Cshow : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        p->dump();
    }
    char* to_s() {return "?stack";}
};

struct Cpeek : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        if (p->empty())
            V::outln("");
        else {
            Token_ t = p->peek();
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
        VStack_ p = q->stack();
        p->dump();
        V::outln(q->parent()->words()->to_s());
    }
    char* to_s() {return "?debug";}
};

struct Cdframe : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
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
        VStack_ p = q->stack();
        Token_ v = p->pop();
        TokenIterator_ fstream =v->qvalue()->tokens()->iterator();
        QuoteStream_ tmpl = new (collect) QuoteStream();
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            if (t->type() == TSymbol && (t->svalue() == Sym::lookup(":")))
                break;
            tmpl->add(t);
        }

        QuoteStream_ res = new (collect) QuoteStream();
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            res->add(t);
        }

        QuoteStream_ elem = new (collect) QuoteStream();
        fstream = tmpl->iterator();
        std::stack<Token_ > st;
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            Token_ e = p->pop();
            st.push(e);
        }
        while(st.size()) {
            elem->add(st.top());
            st.pop();
        }
        SymbolMap symbols;
        evaltmpl(tmpl, elem, symbols);

        TokenStream_ resstream = evalres(res, symbols);
        CmdQuote_ qs = new (collect) CmdQuote(resstream);
        TokenIterator_ i = qs->tokens()->iterator();
        while(i->hasNext())
            p->push(i->next());
    }
    char* to_s() {return "view";}
};

struct Ctrans : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ v = p->pop();
        TokenIterator_ fstream =v->qvalue()->tokens()->iterator();
        TokenStream_ tmpl = fstream->next()->qvalue()->tokens();
        
        QuoteStream_ res = new (collect) QuoteStream();
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            res->add(t);
        }

        QuoteStream_ elem = new (collect) QuoteStream();
        fstream = tmpl->iterator();
        std::stack<Token_ > st;
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            Token_ e = p->pop();
            st.push(e);
        }
        while(st.size()) {
            elem->add(st.top());
            st.pop();
        }
        SymbolMap symbols;
        evaltmpl(tmpl, elem, symbols);

        TokenStream_ resstream = evalres(res, symbols);
        CmdQuote_ qs = new (collect) CmdQuote(resstream);
        TokenIterator_ i = qs->tokens()->iterator();
        while(i->hasNext())
            p->push(i->next());
    }
    char* to_s() {return "trans";}
};

struct Cdef : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        q->parent()->def(entry.first, entry.second);
    }
    char* to_s() {return ".";}
};

struct Cdefenv : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ b = p->pop();
        Token_ t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        b->fvalue()->def(entry.first, entry.second);
    }
    char* to_s() {return "&.";}
};

struct Cparent : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        VFrame_ t = p->pop()->fvalue();
        p->push(new (collect) Term(TFrame, t->parent()));
    }
    char* to_s() {return "&parent";}
};

struct Cme : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        p->push(new (collect) Term(TFrame, q->parent()));
    }
    char* to_s() {return "$me";}
};

char* usefile(VFrame* q, char* v, bool lib=false) {
    Char_ val = 0;
    char* l = LIBPATH;
    if (lib) {
        int len = strlen(v) + 1 + strlen(l);
        val = new (collect) char[len + 3];
        std::sprintf(val,"%s/%s%s",l,v,".v");
    } else {
        val = dup_str(v);
    }
    return val;
}

struct Cuse : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ file = p->pop();
        try {
            CmdQuote_ module = 0;
            if (file->type() == TQuote) {
                TokenIterator_ files =file->qvalue()->tokens()->iterator();
                while(files->hasNext()) {
                    Token_ f = files->next();
                    Char_ val = usefile(q, f->svalue());
                    module = new (collect) CmdQuote(new (collect) LexStream(new (collect) FileCharStream(val)));
                    module->eval(q->parent());
                }
            } else {
                Char_ val = usefile(q, file->svalue(), true);
                module = new (collect) CmdQuote(new (collect) LexStream(new (collect) FileCharStream(val)));
                module->eval(q->parent());
            }
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
        VStack_ p = q->stack();
        Token_ env = p->pop();
        Token_ file = p->pop();
        try {
            CmdQuote_ module = 0;
            if (file->type() == TQuote) {
                TokenIterator_ files =file->qvalue()->tokens()->iterator();
                while(files->hasNext()) {
                    Token_ f = files->next();
                    Char_ val = usefile(q, f->svalue());
                    module = new (collect) CmdQuote(new (collect) LexStream(new (collect) FileCharStream(val)));
                    module->eval(env->fvalue());
                }
            } else {
                Char_ val = usefile(q, file->svalue(), true);
                module = new (collect) CmdQuote(new (collect) LexStream(new (collect) FileCharStream(val)));
                module->eval(env->fvalue());
            }

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
        VStack_ p = q->stack();
        Token_ str = p->pop();
        try {
            Char_ v = str->svalue();
            CmdQuote_ module = new (collect) CmdQuote(new (collect) LexStream(new (collect) BuffCharStream(v)));
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
        VStack_ p = q->stack();
        Token_ env = p->pop();
        Token_ str = p->pop();
        try {
            Char_ v = str->svalue();
            CmdQuote_ module = new (collect) CmdQuote(new (collect) LexStream(new (collect) BuffCharStream(v)));
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
        VStack_ p = q->stack();
        Token_ t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        Char_ module = entry.first;
        Quote_ qfull = entry.second;

        TokenIterator_ it = qfull->tokens()->iterator();
        Quote_ pub = it->next()->qvalue();

        QuoteStream_ nts = new (collect) QuoteStream();
        while(it->hasNext())
            nts->add(it->next());

        CmdQuote_ qval = new (collect) CmdQuote(nts);
        qval->eval(q);

        Term_ f = new (collect) Term(TFrame, q);

        QuoteStream_ fts = new (collect) QuoteStream();
        fts->add(f);
        q->parent()->def(special(module), new (collect) CmdQuote(fts));

        // bind all published tokens to parent namespace.
        TokenIterator_ i = pub->tokens()->iterator();
        while(i->hasNext()) {
            Char_ s = i->next()->svalue();
            Char_ def = new (collect) char[strlen(s) + strlen(module) + 9]; // sizeof("$ [ ] &i");
            sprintf(def, "$%s[%s] &i", module.val, s.val);
            Quote_ libs = CmdQuote::getdef(def);
            sprintf(def, "%s:%s", module.val, s.val);
            q->parent()->def(def, libs);
        }
    }
    char* to_s() {return "module";}
};

struct Cwords : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        VFrame_ b = p->pop()->fvalue();
        p->push(new (collect) Term(TQuote, b->words())); 
    }
    char* to_s() {return "&words";}
};

struct Cdequote : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ prog = p->pop();
        prog->qvalue()->eval(q);
    }
    char* to_s() {return "i";}
};

struct Cdequoteenv : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ prog = p->pop();
        Token_ env = p->pop();
        prog->qvalue()->eval(env->fvalue());
    }
    char* to_s() {return "&i";}
};

struct Cstack : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        q->stack()->push(new (collect) Term(TQuote, p->quote()));
    }
    char* to_s() {return "stack";}
};

struct Cunstack : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ t = p->pop();
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
        VStack_ p = q->stack();
        Token_ t = p->pop();
        q->stack()->push(new (collect) Term(TInt, (long)((Term*)t.val)->size()));
    }
    char* to_s() {return "size";}
};

struct Cin : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ i = p->pop();
        Token_ list = p->pop();
        TokenIterator_ ti = list->qvalue()->tokens()->iterator();
        while(ti->hasNext()) {
            Token_ t = ti->next();
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
        VStack_ p = q->stack();
        Token_ i = p->pop();
        int idx = i->ivalue();
        Token_ list = p->pop();
        TokenIterator_ ti = list->qvalue()->tokens()->iterator();
        int count = 0;
        while(ti->hasNext()) {
            Token_ t = ti->next();
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
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();

        QuoteStream_ nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            Token_ res = p->pop();
            nts->add(res);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "map!";}
};

struct Cmap_i : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();

        QuoteStream_ nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            Node_ n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            Token_ res = p->pop();
            p->now(n);
            nts->add(res);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "map";}
};

struct Csplit : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();

        QuoteStream_ nts1 = new (collect) QuoteStream(); 
        QuoteStream_ nts2 = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            Token_ res = p->pop();
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
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();

        QuoteStream_ nts1 = new (collect) QuoteStream(); 
        QuoteStream_ nts2 = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            Node_ n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            Token_ res = p->pop();
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
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();

        QuoteStream_ nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            Token_ res = p->pop();
            if (res->bvalue())
                nts->add(t);
        }
        p->push(new (collect) Term(TQuote, new (collect) CmdQuote(nts)));
    }
    char* to_s() {return "filter!";}
};

struct Cfilter_i : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();

        QuoteStream_ nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            Node_ n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            Token_ res = p->pop();
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
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ init = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();
        p->push(init);
        QuoteStream_ nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
        // The result will be on the stack at the end of the cycle.
    }
    char* to_s() {return "fold!";}
};

struct Cfold_i : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ init = p->pop();
        Token_ list = p->pop();

        Node_ n = p->now();
        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();
        p->push(init);
        QuoteStream_ nts = new (collect) QuoteStream(); 
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
        // The result will be on the stack at the end of the cycle.
        Token_ res = p->pop();
        p->now(n);
        p->push(res);
    }
    char* to_s() {return "fold";}
};

struct Cstep : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
    }
    char* to_s() {return "step!";}
};

struct Cstep_i : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();

        Token_ action = p->pop();
        Token_ list = p->pop();

        TokenIterator_ fstream = list->qvalue()->tokens()->iterator();
        while(fstream->hasNext()) {
            Token_ t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
    }
    char* to_s() {return "step";}
};

struct Cdrop : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();

        int num = p->pop()->ivalue();
        Token_ list = p->pop();

        QuoteStream_ nts = new (collect) QuoteStream();
        TokenIterator_ i = list->qvalue()->tokens()->iterator();
        while(i->hasNext()) {
            Token_ t = i->next();
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
        VStack_ p = q->stack();

        int num = p->pop()->ivalue();
        Token_ list = p->pop();
        int count = 0;

        QuoteStream_ nts = new (collect) QuoteStream();
        TokenIterator_ i = list->qvalue()->tokens()->iterator();
        while(i->hasNext()) {
            Token_ t = i->next();
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
        VStack_ p = q->stack();
        Token_ c = p->pop();
        Token_ expr = p->pop();
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
        Token_ t = q->stack()->pop();
        throw VException("err:throw", t, t->value());
    }
    char* to_s() {return "throw";}
};

struct Cgetdef : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ sym = p->pop();
        Char_ symbol = sym->qvalue()->tokens()->iterator()->next()->svalue();
        
        Quote_ qv = q->lookup(symbol);
        TokenIterator_ it = qv->tokens()->iterator();

        QuoteStream_ nts = new (collect) QuoteStream();
        nts->add(new (collect) Term(TSymbol, symbol));
        while(it->hasNext())
            nts->add(it->next());
        CmdQuote_ res = new (collect) CmdQuote(nts);

        p->push(new (collect) Term(TQuote, res));
    }
    char* to_s() {return ">def";}
};

struct Cenv : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        p->push(new (collect) Term(TQuote, CmdQuote::getdef("platform native")));
    }
    char* to_s() {return "env";}
};

struct Csqrt : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ t = p->pop();
        double num = t->numvalue().d();
        if (num != fabs(num))
            throw VException("err:sqrt:negetive", t,t->value());
        p->push(new (collect) Term(TDouble, sqrt(num)));
    }
    char* to_s() {return "sqrt";}
};

struct Cstime : public Cmd {
    void eval(VFrame* q) {
        VStack_ p = q->stack();
        Token_ t = p->pop();
        bool val = t->bvalue();
        V::showtime = val;
    }
    char* to_s() {return ".time!";}
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
    frame->def("$stack", new (collect) Cstack());
    frame->def("stack!", new (collect) Cunstack());

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

    frame->def(".time", new (collect) Cstime());

    Quote_ libs = CmdQuote::getdef("'std' use");
    libs->eval(frame);
}
