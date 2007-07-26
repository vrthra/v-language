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
/*char* buff =
#include "std.h"
;*/

typedef std::map<char*, Token*, cmp_str> SymbolMap;
typedef std::pair<char*, Quote*> SymPair;

Shield::Shield(VStack* s, Quote* q) :quote(q), stack(s->now()), next(0) {}

SymPair splitdef(Quote* qval) {
    TokenIterator* it = qval->tokens()->iterator();
    Token* symbol = it->next();

    QuoteStream* nts = new QuoteStream();
    while(it->hasNext())
        nts->add(it->next());

    return std::make_pair<char*, Quote*>(symbol->svalue(), new CmdQuote(nts));
}

char* special(char* name) {
    int len = strlen(name);
    char* buf = new char[len + 2];
    buf[0] = '$';
    std::strcpy(buf+1, name);
    return buf;
}

void evaltmpl(TokenStream* tmpl, TokenStream* elem, SymbolMap& symbols) {
    //Take each point in tmpl, and proess elements accordingly.
    TokenIterator* tstream = tmpl->iterator();
    TokenIterator* estream = elem->iterator();
    while(tstream->hasNext()) {
        Token* t = tstream->next();
        switch (t->type()) {
            case TSymbol:
                try {
                    // _ means any one
                    // * means any including nil unnamed.
                    // *a means any including nil but named with symbol '*a'
                    char* value = t->svalue();
                    if (value[0] == '_') {
                        // eat one from estream and continue.
                        estream->next();
                        break;
                    } else if (value[0] == '*') {
                        QuoteStream* nlist = new QuoteStream();
                        // * is all. but before we slurp, check the next element
                        // in the template. If there is not any, then slurp. If there
                        // is one, then slurp until last but one, and leave it.
                        if (tstream->hasNext()) {
                            Token* tmplterm = tstream->next();
                            Token* lastelem = 0;

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
                                        throw VException("err:view:eq", "%s != %s",tmplterm->value(), lastelem->value());
                            }

                        } else {
                            // we can happily slurp now.
                            while(estream->hasNext())
                                nlist->add(estream->next());
                        }
                        if (strlen(value) > 1) { // do we have a named list?
                            symbols[value] = new Term(TQuote, new CmdQuote(nlist));
                        }
                    } else {
                        Token* e = estream->next();
                        symbols[t->value()] = e;
                    }
                    break;
                } catch (VException& e) {
                    throw e;
                } catch (...) {
                    throw VException("err:view:sym",t->value());
                }

            case TQuote:
                // evaluate this portion again in evaltmpl.
                try {
                    Token* et = estream->next();
                    evaltmpl(t->qvalue()->tokens(), et->qvalue()->tokens(), symbols);
                } catch (VException& e) {
                    throw e;
                } catch (...) {
                    throw VException("err:view:quote",t->value());
                }
                break;
            default:
                //make sure both matches.
                Token* eterm = estream->next();
                if (!strcmp(t->value(),eterm->value()))
                    break;
                else
                    throw VException("err:view:eq.1", "%s != %s" ,t->value(), eterm->value());
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
        QuoteStream* r = new QuoteStream();
        TokenIterator* rstream = res->iterator();
        while(rstream->hasNext()) {
            Token* t = rstream->next();
            switch(t->type()) {

                case TQuote:
                    QuoteStream* nq = (QuoteStream*)evalres(t->qvalue()->tokens(), symbols);
                    r->add(new Term(TQuote, new CmdQuote(nq)));
                    break;
                case TSymbol:
                    // do we have it in our symbol table? if yes, replace, else just push it in.
                    char* sym = t->svalue();
                    if (containsKey(symbols, sym)) {
                        // does it look like *xxx ?? 
                        if (sym[0] == '*') {
                            // expand it.
                            Token* star = symbols[sym];
                            QuoteIterator *tx = (QuoteIterator*)star->qvalue()->tokens()->iterator();
                            while(tx->hasNext()) {
                                r->add(tx->next());
                            }
                        } else
                            r->add(symbols[sym]);
                        break;
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
            if (b->type() != TString)
                throw VException("err:type:eq", "%s != %s (type)", a->value(), b->value());
            return !strcmp(a->svalue(), b->svalue());
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
        VStack* p = q->stack();
        p->push(new Term(TBool, true));
    }
};

struct Cfalse : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        p->push(new Term(TBool, false));
    }
};

struct Cadd : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        double dres = a->numvalue().d() + b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new Term(TInt, ires));
        else
            p->push(new Term(TDouble, dres));
    }
};

struct Csub : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        double dres = a->numvalue().d() - b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new Term(TInt, ires));
        else
            p->push(new Term(TDouble, dres));
    }
};

struct Cmul : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        double dres = a->numvalue().d() * b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new Term(TInt, ires));
        else
            p->push(new Term(TDouble, dres));
    }
};

struct Cdiv : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        double dres = a->numvalue().d() / b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new Term(TInt, ires));
        else
            p->push(new Term(TDouble, dres));
    }
};

struct Cand : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        bool res = a->bvalue() && b->bvalue();
        p->push(new Term(TBool, res));
    }
};

struct Cor : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        bool res = a->bvalue() || b->bvalue();
        p->push(new Term(TBool, res));
    }
};

struct Cnot : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        bool res = !a->bvalue();
        p->push(new Term(TBool, res));
    }
};

struct Cisinteger : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TBool, a->type() == TInt));
    }
};

struct Cisdouble : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TBool, a->type() == TDouble));
    }
};

struct Cisbool : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TBool, a->type() == TBool));
    }
};

struct Cissym : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TBool, a->type() == TSymbol));
    }
};

struct Cisquote : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TBool, a->type() == TQuote));
    }
};

struct Cisstr : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TBool, a->type() == TString));
    }
};

struct Cischar : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TBool, a->type() == TChar));
    }
};

struct Cisnum : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TBool, a->type() == TInt || a->type() == TDouble));
    }
};

struct Ctostr : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        p->push(new Term(TString, a->value()));
    }
};

struct Ctoint : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        switch (a->type()) {
            case TInt:
                p->push(a);
                break;
            case TDouble:
                p->push(new Term(TInt, a->numvalue().i()));
                break;
            case TChar:
                p->push(new Term(TInt, a->cvalue()));
                break;
            case TString:
                p->push(new Term(TInt, atol(a->svalue())));
            default:
                throw VException("err:>int", "%s cant convert", a->value());
        }
    }
};

struct Ctodouble : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        switch (a->type()) {
            case TDouble:
                p->push(a);
                break;
            case TInt:
                p->push(new Term(TDouble, a->numvalue().d()));
                break;
            case TChar:
                p->push(new Term(TDouble, (double)a->cvalue()));
                break;
            case TString:
                p->push(new Term(TDouble, atof(a->svalue())));
            default:
                throw VException("err:>decimal", "%s cant convert", a->value());
        }
    }
};

struct Ctobool : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        switch (a->type()) {
            case TInt:
                p->push(new Term(TBool, a->ivalue() != 0));
                break;
            case TDouble:
                p->push(new Term(TBool, a->dvalue() != 0.0));
                break;
            case TChar:
                p->push(new Term(TBool, a->cvalue() != 'f'));
                break;
            case TString:
                p->push(new Term(TBool, (bool)strcmp(a->svalue(), "false")));
                break;
            case TQuote:
                p->push(new Term(TBool, ((Term*)a)->size() != 0));
                break;
            default:
                throw VException("err:>bool", "%s cant convert", a->value());
        }
    }
};

struct Ctochar : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        switch (a->type()) {
            case TInt:
                p->push(a);
                break;
            case TDouble:
                p->push(new Term(TInt, a->numvalue().i()));
                break;
            case TChar:
                p->push(a);
                break;
            case TString:
                p->push(new Term(TChar, a->value()[0]));
                break;
            default:
                throw VException("err:>char", "%s cant convert", a->value());
        }
    }
};

struct Cgt : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        p->push(new Term(TBool, isGt(a,b)));
    }
};

struct Clt : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        p->push(new Term(TBool, isLt(a,b)));
    }
};

struct Clteq : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        p->push(new Term(TBool, !isGt(a,b)));
    }
};

struct Cgteq : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        p->push(new Term(TBool, !isLt(a,b)));
    }
};

struct Ceq : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        p->push(new Term(TBool, isEq(a,b)));
    }
};

struct Cneq : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* a = p->pop();
        p->push(new Term(TBool, !isEq(a,b)));
    }
};

struct Cchoice : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* af = p->pop();
        Token* at = p->pop();
        Token* cond = p->pop();

        if (cond->bvalue())
            p->push(at);
        else
            p->push(af);
    }
};

struct Cif : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* action = p->pop();
        Token* cond = p->pop();

        if (cond->type() == TQuote) {
            Node* n = p->now();
            cond->qvalue()->eval(q);
            cond = p->pop();
            p->now(n);
        }
        if (cond->bvalue())     
            action->qvalue()->eval(q);
    }
};

struct Cifte : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* eaction = p->pop();
        Token* action = p->pop();
        Token* cond = p->pop();

        if (cond->type() == TQuote) {
            Node* n = p->now();
            cond->qvalue()->eval(q);
            cond = p->pop();
            p->now(n);
        }
        if (cond->bvalue())     
            action->qvalue()->eval(q);
        else
            eaction->qvalue()->eval(q);
    }
};

struct Cwhile : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* action = p->pop();
        Token* cond = p->pop();
        while(true) {
            if (cond->type() == TQuote) {
                Node* n = p->now();
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
};

struct Cputs : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        V::outln(a->value());
    }
};

struct Cput : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        V::out(a->value());
    }
};

struct Cshow : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        p->dump();
    }
};

struct Cpeek : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        if (p->empty())
            V::outln("");
        else {
            Token* t = p->peek();
            V::outln(t->value());
        }
    }
};

struct Chelp : public Cmd {
    void eval(VFrame* q) {
        V::outln(q->parent()->words()->to_s());
    }
};

struct Cvdebug : public Cmd {
    void eval(VFrame* q) {
        V::outln("Q:%d",q->parent()->id());
        VStack* p = q->stack();
        p->dump();
        V::outln(q->parent()->words()->to_s());
    }
};

struct Cdframe : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
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
};

struct Cview : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* v = p->pop();
        TokenIterator* fstream =v->qvalue()->tokens()->iterator();
        QuoteStream* tmpl = new QuoteStream();
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            if (t->type() == TSymbol && (!strcmp(t->svalue(),":")))
                break;
            tmpl->add(t);
        }

        QuoteStream* res = new QuoteStream();
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            res->add(t);
        }

        QuoteStream* elem = new QuoteStream();
        fstream = tmpl->iterator();
        std::stack<Token*> st;
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            Token* e = p->pop();
            st.push(e);
        }
        while(st.size()) {
            elem->add(st.top());
            st.pop();
        }
        SymbolMap symbols;
        evaltmpl(tmpl, elem, symbols);

        TokenStream* resstream = evalres(res, symbols);
        CmdQuote* qs = new CmdQuote(resstream);
        TokenIterator* i = qs->tokens()->iterator();
        while(i->hasNext())
            p->push(i->next());
    }
};

struct Ctrans : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* v = p->pop();
        TokenIterator* fstream =v->qvalue()->tokens()->iterator();
        TokenStream* tmpl = fstream->next()->qvalue()->tokens();
        
        QuoteStream* res = new QuoteStream();
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            res->add(t);
        }

        QuoteStream* elem = new QuoteStream();
        fstream = tmpl->iterator();
        std::stack<Token*> st;
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            Token* e = p->pop();
            st.push(e);
        }
        while(st.size()) {
            elem->add(st.top());
            st.pop();
        }
        SymbolMap symbols;
        evaltmpl(tmpl, elem, symbols);

        TokenStream* resstream = evalres(res, symbols);
        CmdQuote* qs = new CmdQuote(resstream);
        TokenIterator* i = qs->tokens()->iterator();
        while(i->hasNext())
            p->push(i->next());
    }
};

struct Cdef : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        char* symbol = entry.first;
        q->parent()->def(symbol, entry.second);
    }
};

struct Cdefenv : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* b = p->pop();
        Token* t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        char* symbol = entry.first;
        b->fvalue()->def(symbol, entry.second);
    }
};

struct Cparent : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        VFrame* t = p->pop()->fvalue();
        p->push(new Term(TFrame, t->parent()));
    }
};

struct Cme : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        p->push(new Term(TFrame, q->parent()));
    }
};

struct Cuse : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* file = p->pop();
        try {
            char* v = file->svalue();
            int len = strlen(v);
            char* val = new char[len + 3];
            std::sprintf(val,"%s%s",v,".v");

            FileCharStream* cs = new FileCharStream(val);
            CmdQuote* module = new CmdQuote(new LexStream(cs));
            module->eval(q->parent());
        } catch (VException& e) {
            e.addLine("use %s", file->value());
            throw e;
        } catch (...) {
            throw VException("err:use", file->value());
        }
    }
};

struct Cuseenv : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* env = p->pop();
        Token* file = p->pop();
        try {
            char* v = file->svalue();
            int len = strlen(v);
            char* val = new char[len + 3];
            std::sprintf(val,"%s%s",v,".v");

            FileCharStream* cs = new FileCharStream(val);
            CmdQuote* module = new CmdQuote(new LexStream(cs));
            module->eval(env->fvalue());
        } catch (VException& e) {
            e.addLine("*use %s", file->value());
        } catch (...) {
            throw VException("err:*use", "%s %s",env->value(), file->value());
        }
    }
};

struct Ceval : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* str = p->pop();
        try {
            char* v = str->svalue();

            BuffCharStream* cs = new BuffCharStream(v);
            CmdQuote* module = new CmdQuote(new LexStream(cs));
            module->eval(q->parent());
        } catch (VException& e) {
            e.addLine("eval %s", str->value());
        } catch (...) {
            throw VException("err:eval", str->value());
        }
    }
};

struct Cevalenv : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* env = p->pop();
        Token* str = p->pop();
        try {
            char* v = str->svalue();

            BuffCharStream* cs = new BuffCharStream(v);
            CmdQuote* module = new CmdQuote(new LexStream(cs));
            module->eval(env->fvalue());
        } catch (VException& e) {
            e.addLine("*eval %s", str->value());
        } catch (...) {
            throw VException("err:*eval", "%s %s", env->value(), str->value());
        }
    }
};

struct Cmodule : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* t = p->pop();
        SymPair entry = splitdef(t->qvalue());
        char* module = entry.first;
        Quote* qfull = entry.second;

        TokenIterator* it = qfull->tokens()->iterator();
        Quote* pub = it->next()->qvalue();

        QuoteStream* nts = new QuoteStream();
        while(it->hasNext())
            nts->add(it->next());

        CmdQuote* qval = new CmdQuote(nts);
        qval->eval(q);

        QuoteStream* fts = new QuoteStream();
        fts->add(new Term(TFrame, q));
        q->parent()->def(special(module), new CmdQuote(fts));

        // bind all published tokens to parent namespace.
        TokenIterator* i = pub->tokens()->iterator();
        while(i->hasNext()) {
            char* s = i->next()->svalue();
            char* def = new char[strlen(s) + strlen(module) + 9]; // sizeof("$ [ ] &i");
            sprintf(def, "$%s[%s] &i", module, s);
            Quote* libs = CmdQuote::getdef(def);
            sprintf(def, "%s:%s", module, s);
            q->parent()->def(def, libs);
        }
    }
};

struct Cwords : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        VFrame* b = p->pop()->fvalue();
        p->push(new Term(TQuote, b->words())); 
    }
};

struct Cdequote : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* prog = p->pop();
        prog->qvalue()->eval(q->parent());
    }
};

struct Cdequoteenv : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* prog = p->pop();
        Token* env = p->pop();
        prog->qvalue()->eval(env->fvalue());
    }
};

struct Cstack : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        q->stack()->push(new Term(TQuote, p->quote()));
    }
};

struct Cunstack : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* t = p->pop();
        p->dequote(t->qvalue());
    }
};

struct Cabort : public Cmd {
    void eval(VFrame* q) {
        q->stack()->clear();
    }
};

struct Csize : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* t = p->pop();
        q->stack()->push(new Term(TInt, (long)((Term*)t)->size()));
    }
};

struct Cin : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* list = p->pop();
        Token* i = p->pop();
        TokenIterator* ti = list->qvalue()->tokens()->iterator();
        while(ti->hasNext()) {
            Token* t = ti->next();
            if (t->type() == i->type() && !strcmp(t->value(), i->value())) {
                p->push(new Term(TBool, true));
                return;
            }
        }
        p->push(new Term(TBool, false));
    }
};

struct Cat : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* i = p->pop();
        int idx = i->ivalue();
        Token* list = p->pop();
        TokenIterator* ti = list->qvalue()->tokens()->iterator();
        int count = 0;
        while(ti->hasNext()) {
            Token* t = ti->next();
            if (count == idx) {
                p->push(t);
                return;
            }
            ++count;
        }
        throw VException("err:at:overflow", "[%s]:%d",list->value(), idx);
    }
};

struct Cmap : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* list = p->pop();

        TokenIterator* fstream = list->qvalue()->tokens()->iterator();

        QuoteStream* nts = new QuoteStream(); 
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            Token* res = p->pop();
            nts->add(res);
        }
        p->push(new Term(TQuote, new CmdQuote(nts)));
    }
};

struct Cmap_i : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* list = p->pop();

        TokenIterator* fstream = list->qvalue()->tokens()->iterator();

        QuoteStream* nts = new QuoteStream(); 
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            Node* n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            Token* res = p->pop();
            p->now(n);
            nts->add(res);
        }
        p->push(new Term(TQuote, new CmdQuote(nts)));
    }
};

struct Csplit : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* list = p->pop();

        TokenIterator* fstream = list->qvalue()->tokens()->iterator();

        QuoteStream* nts1 = new QuoteStream(); 
        QuoteStream* nts2 = new QuoteStream(); 
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            Token* res = p->pop();
            if (res->bvalue())
                nts1->add(t);
            else
                nts2->add(t);
        }
        p->push(new Term(TQuote, new CmdQuote(nts1)));
        p->push(new Term(TQuote, new CmdQuote(nts2)));
    }
};

struct Csplit_i : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* list = p->pop();

        TokenIterator* fstream = list->qvalue()->tokens()->iterator();

        QuoteStream* nts1 = new QuoteStream(); 
        QuoteStream* nts2 = new QuoteStream(); 
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            Node* n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            Token* res = p->pop();
            p->now(n);
            if (res->bvalue())
                nts1->add(t);
            else
                nts2->add(t);
        }
        p->push(new Term(TQuote, new CmdQuote(nts1)));
        p->push(new Term(TQuote, new CmdQuote(nts2)));
    }
};

struct Cfilter : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* list = p->pop();

        TokenIterator* fstream = list->qvalue()->tokens()->iterator();

        QuoteStream* nts = new QuoteStream(); 
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
            Token* res = p->pop();
            if (res->bvalue())
                nts->add(t);
        }
        p->push(new Term(TQuote, new CmdQuote(nts)));
    }
};

struct Cfilter_i : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* list = p->pop();

        TokenIterator* fstream = list->qvalue()->tokens()->iterator();

        QuoteStream* nts = new QuoteStream(); 
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            Node* n = p->now();
            p->push(t);

            action->qvalue()->eval(q);
            Token* res = p->pop();
            p->now(n);
            if (res->bvalue())
                nts->add(t);
        }
        p->push(new Term(TQuote, new CmdQuote(nts)));
    }
};

struct Cfold : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* init = p->pop();
        Token* list = p->pop();

        TokenIterator* fstream = list->qvalue()->tokens()->iterator();
        p->push(init);
        QuoteStream* nts = new QuoteStream(); 
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
        // The result will be on the stack at the end of the cycle.
        //Term* res = p->pop();
        //p->push(res);
    }
};

struct Cfold_i : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* init = p->pop();
        Token* list = p->pop();

        Node* n = p->now();
        TokenIterator* fstream = list->qvalue()->tokens()->iterator();
        p->push(init);
        QuoteStream* nts = new QuoteStream(); 
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
        // The result will be on the stack at the end of the cycle.
        Token* res = p->pop();
        p->now(n);
        p->push(res);
    }
};

struct Cstep : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        Token* action = p->pop();
        Token* list = p->pop();

        TokenIterator* fstream = list->qvalue()->tokens()->iterator();
        while(fstream->hasNext()) {
            Token* t = fstream->next();
            p->push(t);

            action->qvalue()->eval(q);
        }
    }
};

struct Cdrop : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        int num = p->pop()->ivalue();
        Token* list = p->pop();

        QuoteStream* nts = new QuoteStream();
        TokenIterator* i = list->qvalue()->tokens()->iterator();
        while(i->hasNext()) {
            Token* t = i->next();
            if (num <= 0)
                nts->add(t);
            --num;
        }
        p->push(new Term(TQuote, new CmdQuote(nts)));
    }
};

struct Ctake : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();

        int num = p->pop()->ivalue();
        Token* list = p->pop();
        int count = 0;

        QuoteStream* nts = new QuoteStream();
        TokenIterator* i = list->qvalue()->tokens()->iterator();
        while(i->hasNext()) {
            Token* t = i->next();
            if (count >= num)
                break;
            ++count;
            nts->add(t);
        }
        p->push(new Term(TQuote, new CmdQuote(nts)));
    }
};

struct CmdShield : public Cmd {
    void eval(VFrame* q) {
        // dummy
    }
};

struct Cshield : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* t = p->pop();
        
        Cmd* shield = 0;
        Shield* s = new Shield(p, t->qvalue());
        if(q->parent()->dict().find("$shield") == q->dict().end()) {
            shield = new CmdShield();
            shield->store()["$info"] = s;
            q->parent()->def("$shield",shield);
        } else {
            shield = (Cmd*)q->dict()["$shield"];
            Shield* os = shield->store()["$info"];
            s->next = os;  
            shield->store()["$info"] = s;
        }
    }
};

struct Cthrow : public Cmd {
    void eval(VFrame* q) {
        throw VException("err:throw", q->stack()->peek()->value());
    }
};

struct Csqrt : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* t = p->pop();
        double num = t->numvalue().d();
        if (num != fabs(num))
            throw VException("err:sqrt:negetive",t->value());
        p->push(new Term(TDouble, sqrt(num)));
    }
};
 
void Prologue::init(VFrame* frame) {
    frame->def(".", new Cdef());
    frame->def("&.", new Cdefenv());
    frame->def("&parent", new Cparent());
    frame->def("$me", new Cme());
    frame->def("module", new Cmodule());
    frame->def("&words", new Cwords());

    frame->def("puts", new Cputs());
    frame->def("put", new Cput());

    frame->def("i", new Cdequote());
    frame->def("&i", new Cdequote());

    frame->def("view", new Cview());
    frame->def("trans", new Ctrans());
    frame->def("use", new Cuse());
    frame->def("&use", new Cuseenv());
    frame->def("eval", new Ceval());
    frame->def("&eval", new Cevalenv());
    frame->def("stack", new Cstack());
    frame->def("unstack", new Cunstack());

    frame->def("true", new Ctrue());
    frame->def("false", new Cfalse());

    frame->def("+", new Cadd());
    frame->def("-", new Csub());
    frame->def("*", new Cmul());
    frame->def("/", new Cdiv());
    
    frame->def("and", new Cand());
    frame->def("or", new Cor());
    frame->def("not", new Cnot());

    frame->def("choice", new Cchoice());
    frame->def("ifte", new Cifte());
    frame->def("if", new Cif());
    frame->def("while", new Cwhile());
    
    frame->def("=", new Ceq());
    frame->def("==", new Ceq());
    frame->def("!=", new Cneq());
    frame->def(">", new Cgt());
    frame->def("<", new Clt());
    frame->def("<=", new Clteq());
    frame->def(">=", new Cgteq());

    frame->def("??", new Cshow());
    frame->def("?stack", new Cshow());
    frame->def("?", new Cpeek());
    frame->def("?debug", new Cvdebug());
    frame->def("?frame", new Cdframe());
    
    frame->def("int?", new Cisinteger);
    frame->def("decimal?", new Cisdouble);
    frame->def("bool?", new Cisbool());
    frame->def("symbol?", new Cissym());
    frame->def("list?", new Cisquote());
    frame->def("char?", new Cischar());
    frame->def("number?", new Cisnum());
    frame->def("string?", new Cisstr());

    frame->def(">string", new Ctostr());
    frame->def(">int", new Ctoint());
    frame->def(">decimal", new Ctodouble());
    frame->def(">bool", new Ctobool());
    frame->def(">char", new Ctochar());

    frame->def("abort", new Cabort());
    frame->def("size", new Csize());
    frame->def("in?", new Cin());
    frame->def("at", new Cat());

    frame->def("step", new Cstep);
    frame->def("map!", new Cmap);
    frame->def("map", new Cmap_i);
    frame->def("filter!", new Cfilter);
    frame->def("filter", new Cfilter_i);
    frame->def("split!", new Csplit);
    frame->def("split", new Csplit_i);
    frame->def("fold!", new Cfold);
    frame->def("fold", new Cfold_i);

    frame->def("drop", new Cdrop);
    frame->def("take", new Ctake);
    
    frame->def("help", new Chelp);
    frame->def("throw", new Cthrow);
    frame->def("shield", new Cshield);

    // math
    frame->def("sqrt", new Csqrt);

    Quote* libs = CmdQuote::getdef("'std' use");
    libs->eval(frame);
}
