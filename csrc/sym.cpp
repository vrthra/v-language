#include <map>
#include "sym.h"
#include "common.h"
#include <string.h>

struct cmp_str {
    bool operator()(char const *a, char const* b) {return strcmp(a,b) < 0; }
};

typedef std::map<char*, char*, cmp_str> SymbolTable;

SymbolTable __symbols;

char* Sym::lookup(char* key) {
    SymbolTable::iterator i = __symbols.find(key);
    if (i == __symbols.end()) {
        char* w = dup_str(key);
        __symbols[w] = w;
    }
    return i->second;
}
