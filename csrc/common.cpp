#include "common.h"
#include <string.h>

char* dup_str(const char* c) {
    int i = strlen(c);
    P<char,true> out = new (collect) char[i + 1];
    strcpy(out, c);
    return out;
}
