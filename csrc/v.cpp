#include <stdio.h>
#include "type.h"
#include "term.h"
#include "vstack.h"
#include "vframe.h"
#include "v.h"
#include "lexstream.h"
#include "filecharstream.h"
#include "consolecharstream.h"
#include "vexception.h"
#include "prologue.h"
#include "cmdquote.h"

const char* V::version = "0.004";
bool V::singleassign = true;

bool singleassign() {
    return V::singleassign;
}
// TODO: replace it by varargs.
void V::outln(char* var, char* c) {
    printf("%s%s\n", var,c);
}
void V::outln(char* var) {
    printf("%s\n", var);
}

void V::banner() {
    V::outln("\tV\t");
}

class PQuote : public CmdQuote {
    const bool _i;
    public:
        PQuote(LexStream* ls, bool i): CmdQuote(ls),_i(i) {}
        void dofunction(VFrame* scope) {
            if (_i) {
                try {
                    CmdQuote::dofunction(scope);
                } catch (Vx& e) {
                    V::outln(">", e.message());
                    scope->dump();
                }
            } else {
                CmdQuote::dofunction(scope);
            }
        }
};

void V::main(int argc, char** argv) {
    bool i = argc > 1 ? false : true;
    VFrame* frame = new VFrame();
    for(int i=0; i<argc; ++i)
        frame->stack()->push(new Term(TString, argv[i]));
    // setup the world quote

    Prologue::init(frame);
    try {
        // do we have any args?
        CharStream* cs = 0;
        if (argc > 1) {
            cs = new FileCharStream(argv[1]);
        } else {
            banner();
            cs = new ConsoleCharStream();
        }
        PQuote* program = new PQuote(new LexStream(cs), i);
        program->eval(frame->child());
    } catch (Vx& e) {
        V::outln(e.message());
    }
}


int main(int argc, char** argv) {
    V::main(argc, argv);
    return 0;
}
