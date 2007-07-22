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
char* buff =
#include "std.h"
;

typedef std::map<char*, Token*, cmp_str> SymbolMap;
typedef std::pair<char*, Quote*> SymPair;

SymPair splitdef(Quote* qval) {
    TokenIterator* it = qval->tokens()->iterator();
    Token* symbol = it->next();

    QuoteStream* nts = new QuoteStream();
    while(it->hasNext())
        nts->add(it->next());

    return std::make_pair<char*, Quote*>(symbol->svalue(), new CmdQuote(nts));
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
        } catch (...) {
            throw new VException("err:use", file->value());
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
        } catch (...) {
            throw new VException("err:*use", "%s %s",env->value(), file->value());
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
        } catch (...) {
            throw new VException("err:eval", str->value());
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
        } catch (...) {
            throw new VException("err:*eval", "%s %s", env->value(), str->value());
        }
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

void Prologue::init(VFrame* frame) {
    frame->def(".", new Cdef());
    frame->def("&.", new Cdefenv());
    frame->def("&parent", new Cparent());
    frame->def("$me", new Cme());

    frame->def("puts", new Cputs());
    frame->def("put", new Cputs());

    frame->def("i", new Cdequote());
    frame->def("&i", new Cdequote());

    frame->def("view", new Cview());
    frame->def("use", new Cuse());
    frame->def("&use", new Cuseenv());
    frame->def("eval", new Ceval());
    frame->def("&eval", new Cevalenv());

    frame->def("true", new Ctrue());
    frame->def("false", new Cfalse());

    frame->def("+", new Cadd());
    
    frame->def("??", new Cshow());
/*
        iframe.def("module", _defmodule);
        iframe.def("&words", _words);

        iframe.def("trans", _trans);
        iframe.def("java", _java);

        iframe.def("shield", _shield);
        iframe.def("throw", _throw);
        iframe.def("stack", _stack);
        iframe.def("unstack", _unstack);

        iframe.def("and", _and);
        iframe.def("or", _or);
        iframe.def("not", _not);

        //control structures
        iframe.def("ifte", _ifte);
        iframe.def("if", _if);
        iframe.def("while", _while);
        iframe.def("choice", _choice);

        //others
        iframe.def("?", _peek);
        iframe.def("?debug", _vdebug);
        iframe.def("?stack", _show);
        iframe.def("?frame", _dframe);
        iframe.def("debug", _debug);

        iframe.def("abort", _abort);

        //list
        iframe.def("size", _size);
        iframe.def("in?", _isin);
        iframe.def("at", _at);
        iframe.def("drop", _drop);
        iframe.def("take", _take);


        // on list
        iframe.def("step", _step);
        iframe.def("map!", _map);
        iframe.def("map", _map_i);
        iframe.def("filter!", _filter);
        iframe.def("filter", _filter_i);
        iframe.def("split!", _split);
        iframe.def("split", _split_i);
        iframe.def("fold!", _fold);
        iframe.def("fold", _fold_i);

        //arith
        iframe.def("+", _add);
        iframe.def("-", _sub);
        iframe.def("*", _mul);
        iframe.def("/", _div);

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

        iframe.def("help", _help);

        Quote libs = Util.getdef("'std' use");
        libs.eval(iframe);
 */
}
