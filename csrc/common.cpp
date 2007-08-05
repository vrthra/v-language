#include "common.h"
#include <string.h>

char* dup_str(const char* c) {
    int i = strlen(c);
    char* out = new char[i + 1];
    strcpy(out, c);
    return out;
}
