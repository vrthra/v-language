#ifndef ALLOCATOR_H
#define ALLOCATOR_H
#include <new>
enum Collect {collect};
void * operator new (size_t t, Collect);
void * operator new[] (size_t t, Collect);

#endif
