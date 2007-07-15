#ifndef TERM_H
#define TERM_H
#include "type.h"
union UToken {
    char* cval;
};

class Term {
        Type _type;
        UToken _val;
    public:
        Term(Type t, int v);
        Term(Type t, char* v);
        Type type() {
            return _type;
        }
        int size() {
            return 0;
        }
};
#endif
