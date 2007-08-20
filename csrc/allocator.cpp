#include <list>
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "allocator.h"
#include "vx.h"
using namespace std;

class Vxcrit : public Vx {
    char* _msg;
    public:
    Vxcrit(char* m) : _msg(m) {}
    void addLine(char* v,...) {}
    char* message() {
        return _msg;
    }
};

int GcInterval = 1024*1024;
typedef std::list<void*> Mobj;
Mobj __objects;
Mobj __prim;

int __objdiff= 0;

int L_SIZE = sizeof(long);

void* operator new(size_t t, Collect) {
    void *p = malloc(t + L_SIZE);
    if (!p)
        throw Vxcrit("Not enough memory");
    long* i = (long*)p;
    *i = 0;
    __prim.push_back(p);
    __objdiff++;
    //cout <<setbase(16) << (long)p + L_SIZE <<setbase(10) << endl;
    return (char*)p+L_SIZE;
}

void* operator new[](size_t t, Collect) {
    void *p = malloc(t + L_SIZE);
    if (!p)
        throw Vxcrit("Not enough memory");
    long* i = (long*)p;
    *i = 0;
    __prim.push_back(p);
    __objdiff++;
    //cout <<setbase(16) << (long)p + L_SIZE <<setbase(10) << endl;
    return (char*)p+L_SIZE;
}

// leave the space in each object for reference counting.
void* Obj::operator new(size_t t, Collect) {
    void *p = malloc(t + L_SIZE);
    if (!p)
        throw Vxcrit("Not enough memory");
    long* i = (long*)p;
    *i = 0;
    __objects.push_back(p);
    __objdiff++;
    //cout <<setbase(16) << (long)p + L_SIZE <<setbase(10) << endl;
    return (char*)p+L_SIZE;
}

void* Obj::operator new[](size_t t, Collect) {
    void *p = malloc(t + L_SIZE);
    if (!p)
        throw Vxcrit("Not enough memory");
    long* i = (long*)p;
    *i = 0;
    __objects.push_back(p);
    __objdiff++;
    //cout <<setbase(16) << (long)p + L_SIZE <<setbase(10) << endl;
    return (char*)p+L_SIZE;
}

Obj::~Obj() {
    //cout<<"Obj Destroy called ++++++++++" <<endl;
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
                if (*p < 0)
                    throw Vxcrit("Bad reference count.");
                more++;
                total += *p;
        }
    }
    cout<<"\n Obj unref :"<< zeros <<" ref :"<< more << " totalref :"<< total <<endl;

    for (Mobj::iterator i = __prim.begin(); i != __prim.end(); i++) {
        long* p = (long*) *i;
        switch (*p) {
            case 0:
                zeros++;
                break;
            default:
                if (*p < 0)
                    throw Vxcrit("Bad reference count.");
                more++;
                total += *p;
        }
    }
    cout<<"\n Prim unref :"<< zeros <<" ref :"<< more << " totalref :"<< total <<endl;

}

// problems:
// Dont know how to handle objects in stack.
void gc() {
    if (__objdiff < GcInterval)
        return;
    __objdiff = 0;
    for (Mobj::iterator i = __objects.begin(); i != __objects.end(); i++) {
        void* v = *i;
        long* p = (long*) v;
        if (!*p) {
            Obj* o = (Obj*) ((char*)v + L_SIZE);
            if (o) {
                o->~Obj();
                free(v);
                *i = 0;
            }
        }
    }
    __objects.remove(0);

    // do the same for primaries
    for (Mobj::iterator i = __prim.begin(); i != __prim.end(); i++) {
        void* v = *i;
        long* p = (long*) v;
        if (!*p) {
            void* o = (char*)v + L_SIZE;
            if (o) {
                free(v);
                *i = 0;
            }
        }
    }
    __prim.remove(0);
}
