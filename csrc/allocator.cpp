#include <vector>
#include <iostream>
#include <stdlib.h>
#include "allocator.h"
using namespace std;

typedef std::vector<void*> Mobj;
Mobj __objects;

// leave the space in each object for reference counting.
void * operator new (size_t t, Collect) {
    void *p = malloc(t + sizeof(long));
    long* i = (long*)p;
    *i = 0;
    __objects.push_back(p);
    return (char*)p+sizeof(long);
}
void * operator new[] (size_t t, Collect) {
    void *p = malloc(t + sizeof(long));
    long* i = (long*)p;
    *i = 0;
    __objects.push_back(p);
    return (char*)p+sizeof(long);
}

void show() {
    int zeros = 0;
    int more = 0;
    int total = 0;
    for (Mobj::iterator i = __objects.begin(); i != __objects.end(); i++) {
        long* p = (long*) *i;
        switch (*p) {
            case 0:
                zeros++;
                break;
            default:
                more++;
                total += *p;
        }
    }
    cout<<"unref :"<< zeros <<" ref :"<< more << " totalref :"<< total <<endl;
}
