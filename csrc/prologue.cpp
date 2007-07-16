#include "prologue.h"
#include "term.h"
#include "cmd.h"
#include "vstack.h"
#include "vframe.h"
struct Cadd : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Term* a = p->pop();
        Term* b = p->pop();
        double dres = a->numvalue().d() + b->numvalue().d();
        long ires = (long)dres;
        if (dres == ires)
            p->push(new Term(TInt, ires));
        else
            p->push(new Term(TDouble, dres));
    }
};

void Prologue::init(VFrame* frame) {
    frame->def("+", new Cadd());
}
