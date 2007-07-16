#ifndef NUM_H
#define NUM_H
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
            case 1:
                return (long)_d;
        }
    }
    double d() {
        switch (type) {
            case 0:
                return (double)_i;
            case 1:
                return _d;
        }
        return _d;
    }
};
#endif
