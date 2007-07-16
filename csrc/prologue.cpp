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
struct Cputs : public Cmd {
    void eval(VFrame* q) {
        VStack* p = q->stack();
        Term* a = p->pop();
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
void Prologue::init(VFrame* frame) {
    frame->def("+", new Cadd());
    frame->def("puts", new Cputs());
}
