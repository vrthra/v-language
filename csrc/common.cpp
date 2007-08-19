#include "common.h"
#include "defs.h"
#include <string.h>

char* dup_str(const char* c,bool b) {
    int i = strlen(c);
    if (b) {
        Char_ out = new (collect) char[i + 1];
        strcpy(out, c);
        return out;
    } else {
        char* out = new (collect) char[i + 1];
        strcpy(out, c);
        return out;
    }
}

void* __bottom_ptr = new char[0];
bool invalid(void* p) {
    return p <= __bottom_ptr;
}
