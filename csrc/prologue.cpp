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
#include "vexception.h"
char* buff =
#include "std.h"
;

typedef std::map<char*, Token*, cmp_str> SymbolMap;

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
                                        throw VException("err:evaltmpl:eq ",tmplterm->value());
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
                    throw VException(e);
                } catch (...) {
                    throw VException("err:evaltmpl:sym ",t->value());
                }

            case TQuote:
                // evaluate this portion again in evaltmpl.
                try {
                    Token* et = estream->next();
                    evaltmpl(t->qvalue()->tokens(), et->qvalue()->tokens(), symbols);
                } catch (VException& e) {
                    throw VException(e);
                } catch (...) {
                    throw VException("err:evaltmpl:quote ",t->value());
                }
                break;
            default:
                //make sure both matches.
                Token* eterm = estream->next();
                if (!strcmp(t->value(),eterm->value()))
                    break;
                else
                    throw VException("err:evaltmpl:eq " ,t->value());
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

struct Cadd : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        Token* b = p->pop();
        double dres = a->numvalue().d() + b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new Term(TInt, ires));
        else
            p->push(new Term(TDouble, dres));
    }
};
struct Cputs : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Token* a = p->pop();
        switch(a->type()) {
            case TInt:
                printf("%d\n",a->ivalue());
                break;
            case TDouble:
                printf("%f\n",a->dvalue());
                break;
            default:
                printf(">%d\n",a->dvalue());
        }
    }
};

struct Cshow : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        p->dump();
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


void Prologue::init(VFrame* frame) {
    frame->def("+", new Cadd());
    frame->def("puts", new Cputs());
    frame->def("??", new Cshow());
    frame->def("view", new Cview());
}
