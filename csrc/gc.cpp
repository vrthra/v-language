#include <iostream>
#include <map>
#include <list>
#include "gc.h"
using std::cout;
using std::endl;

struct Scope;
typedef std::map<void*, Scope*> GcMap;
typedef std::map<void*, Scope*>::iterator GcIter;
typedef std::list<void*> GcList;
typedef std::list<void*>::iterator GcListIter;

// The record of all mem.
GcMap __all;

GcMap __current;

int Gc::__phase = 0;
int Gc::phase() {
    return __phase;
}
int Gc::phase(int np) {
    return __phase = np;
}

long Gc::collect() {
    cout<<"Collect begin ..." << endl;
    // remove and delete gcinfo ptr.
    int garbage = phase();
    int marking = garbage? 0 : 1;

    cout<<"Garbage: "<< garbage<< " Marking: "<< marking << endl;
    cout<<"All Size: "<< __all.size()<< " Current: "<< __current.size()<< endl;

    // mark all the pointers contained in __current,
    // then iterate __all and sweep off all pointers that are not marked
    // finally update __phase to marking.
    for(GcIter i = __current.begin(); i != __current.end(); i++) {
        Scope* g = i->second;
        g->mark(marking);
    }
    GcList dead;
    for(GcIter i = __all.begin(); i != __all.end(); i++) {
        Scope* g = i->second;
        if (g->mark() == garbage)
            dead.push_back(i->first);
    }
    cout<<"Dead Size: "<< dead.size()<< endl;

    for (GcListIter i = dead.begin(); i != dead.end(); i++) {
        Scope* g = __all[*i];
        __all.erase(*i);
        cout<<"\t delete : "<< (long) g << endl;
        delete g;
    }
    
    cout<<"After Gc All Size: "<< __all.size()<< endl;

    // switch back.
    phase(marking);
}

void Gc::addptr(Scope* g, void* gcptr) {
    cout << "\tadd:"<<(int) gcptr << endl;
    // is it already registered?
    __all[g->mem()] = g;

    // who owns the scope?
    __current[gcptr] = g;
}

Scope* Gc::getptr(void* mem) {
    Scope* g = 0;
    // is it already registered?
    if (__all.find(mem) == __all.end()) {
        return 0;
    } else {
        return __all[mem];
    }
}

void Gc::rmptr(void* gcptr) {
    cout << "\tremove:"<<(int) gcptr << endl;
    __current.erase(gcptr);
}
