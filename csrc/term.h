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
        Term(Type t, long v);
        Term(Type t, double v);
        Term(Type t, char* v);
        Term(Type t, char v);
        Type type() {
            return _type;
        }
        int size() {
            return 0;
        }
};
#endif
