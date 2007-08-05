#ifndef GC_H
#define GC_H

// we need to take into account two kinds of pointers.
// 1. the member pointers in objects. This can be captured by providing
// a member wrapper for member objects that gets destroyed when object
// is destroyed.
// 2. pointers allocated in the stack. To catch them, we use the same mechanism.

template <class T> class P : public GcPtr {
    T* _data;
public:
    P(T *p = 0):_data(p) {
        add(p);
    }

    P(const P<T> &p) :_data(p._data){
        add(p._data);
    }

    /* return raw pointer */
    T *operator ()() const {
        return _data;
    }

    /* convert to raw pointer. */
    operator T *() const {
        return _data;
    }

    T *operator ->() const {
        return _data;
    }

    bool operator == (const T *p) const {
        return _data == p;
    }

    bool operator != (const T *p) const {
        return _data != p;
    }

    bool operator == (const P<T> &p) const {
        return _data == p._data;
    }

    bool operator != (const P<T> &p) const {
        return _data != p._dta;
    }

    /* assignment from raw pointer.
     * register and continue.
     */
    P<T> &operator = (T *p) {
        del(_data);
        add(p);
        _data = p;
        return *this;
    }

    /* assignment from pointer object. */
    P<T> &operator = (const P<T> &p) {
        del(_data);
        add(p._data);
        _data = p._data;
        return *this;
    }

    ~P() {
        del(_data);
    }
};

#endif
