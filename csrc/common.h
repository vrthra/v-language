#ifndef COMMON_H
#define COMMON_H

#if HAVE_CONFIG_H
#   include <config.h>
#endif

static const double Precision = 0.0000000000000001;
static const int MaxBuf = 1024;

char* dup_str(const char* c, bool b=true);

#include "allocator.h"
#endif
