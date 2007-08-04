#ifndef COMMON_H
#define COMMON_H
#include <cstring>
struct cmp_str {
    bool operator()(char const *a, char const *b) {
        return std::strcmp(a, b) < 0;
    }
};

char* dup_str(char* c);

static const double Precision = 0.0000000000000001;
static const int MaxBuf = 1024;

#endif
