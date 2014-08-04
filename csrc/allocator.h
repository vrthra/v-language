#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <new>
#include <cstddef>

enum Collect {collect};
void show();
void gc();

void * operator new (std::size_t t, Collect);
void * operator new[] (std::size_t t, Collect);
struct Obj {
    void * operator new (std::size_t t, Collect);
    void * operator new[] (std::size_t t, Collect);
    virtual ~Obj();
};

#endif
