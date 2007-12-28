#include <map>
#include "sym.h"
#include "common.h"
#include "defs.h"
#include <string.h>

struct cmp_str {
    bool const operator()(char const *a, char const* b) const {return strcmp(a,b) < 0; }
};

typedef std::map<char*, Char_, cmp_str> SymbolTable;

SymbolTable __symbols;

char* Sym::lookup(char* key) {
    SymbolTable::iterator i = __symbols.find(key);
    if (i != __symbols.end()) {
        return i->second;
    } else {
        char* w = dup_str(key);
        __symbols[w] = w;
        return w;
    }
}
