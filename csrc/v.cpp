#include <stdio.h>
#include <stdarg.h>
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

void V::out(char *var, ...) {
    va_list argp;
    va_start(argp, var);
    vfprintf(stdout, var, argp);
    va_end(argp);

}

void V::outln(char *var, ...) {
    va_list argp;
    va_start(argp, var);
    vfprintf(stdout, var, argp);
    va_end(argp);
    fprintf(stdout, "\n");
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
                    V::outln(">%s", e.message());
                    scope->dump();
                }
            } else {
                CmdQuote::dofunction(scope);
            }
        }
        char* to_s() {
            return "<shell>";
        }
};

void V::main(int argc, char** argv) {
    bool i = argc > 1 ? false : true;
    VFrame* frame = new VFrame();
    for(int j=0; j<argc; ++j)
        frame->stack()->push(new Term(TString, argv[j]));
    // setup the world quote

    Prologue::init(frame);
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
}


int main(int argc, char** argv) {
    try {
        V::main(argc, argv);
    } catch (Vx& e) {
        V::outln(e.message());
    }
    return 0;
}
