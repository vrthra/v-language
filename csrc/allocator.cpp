#include <vector>
#include <stdlib.h>
#include "allocator.h"

std::vector<void*> __objects;

// leave the space in each object for reference counting.
void * operator new (size_t t, Collect) {
    void *p = malloc(t + sizeof(long));
    __objects.push_back(p);
    return (char*)p+sizeof(long);
}
void * operator new[] (size_t t, Collect) {
    void *p = malloc(t + sizeof(long));
    __objects.push_back(p);
    return (char*)p+sizeof(long);
}

