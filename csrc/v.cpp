#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
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

bool V::singleassign = false;
bool V::showtime = false;
static char* _libpath = LIBPATH;

bool singleassign() {
    return V::singleassign;
}

void V::out(char *var, ...) {
    va_list argp;
    va_start(argp, var);
    vfprintf(stdout, var, argp);
    va_end(argp);

}

char* V::libpath() {
    return _libpath;
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

void usage() {
    V::outln("usage: v [-l libpath] [-h] [-v] [source.v]");
    exit(0);
}
void version() {
    V::outln("v 0.005");
    exit(0);
}

void V::main(int argc, char** argv) {
    int ch;
    bool interactive = true;
    VFrame_ frame = new (collect) VFrame();
    while((ch = getopt(argc, argv, "vhl:")) != -1) {
        switch(ch) {
            case 'v':
                version();
            case 'h':
                usage();
            case 'l':
                _libpath = optarg;
                break;
        }
    }
    argc -= optind;
    argv += optind;
    interactive = !argc;
    for(int j=0; j<argc; ++j)
        frame->stack()->push(new (collect) Term(TString, dup_str(argv[j])));
    // setup the world quote

    Prologue::init(frame);
    // do we have any args?
    CharStream_ cs = 0;
    if (!interactive) {
        cs = new (collect) FileCharStream(dup_str(argv[0]));
    } else {
        banner();
        cs = new (collect) ConsoleCharStream();
    }
    PQuote_ program = new (collect) PQuote(new (collect) LexStream(cs), interactive);
    program->eval(frame->child());
}


int main(int argc, char** argv) {
    time_t seconds = time(0);
    try {
        V::main(argc, argv);
    } catch (Vx& e) {
        V::outln(e.message());
    }
    if (V::showtime) {
        printf("time: %ld\n", (long)(time(0) - seconds));
        printf("clock: %ld\n", clock());
    }
    return 0;
}
