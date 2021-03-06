#ifndef NUM_H
#define NUM_H
#include <math.h>
struct Num {
    union {
        long _i;
        double _d;
    };
    int type;
    Num(long i) {
        type = 0;
        _i = i;
    }
    Num(double d) {
        type = 1;
        _d = d;
    }
    Num() {
    }
    long i() {
        switch (type) {
            case 0:
                return _i;
            default:
                return (long)_d;
        }
    }
    double d() {
        switch (type) {
            case 0:
                return _i * 1.0;
            case 1:
                return _d;
        }
        return _d;
    }
};
#endif
