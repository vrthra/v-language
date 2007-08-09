#ifndef GC_H
#define GC_H

// we need to take into account two kinds of pointers.
// 1. the member pointers in objects. This can be captured by providing
// a member wrapper for member objects that gets destroyed when object
// is destroyed.
// 2. pointers allocated in the stack. To catch them, we use the same mechanism.

struct Scope;
class Gc {
    private:
        static int __phase;
    public:
        static int phase();
        static int phase(int np); 
        static long collect();
        static void addptr(Scope* ptr, void* gcptr);
        static Scope* getptr(void* ptr);
        static void rmptr(void* ptr);
        static void* bottom();
};

// (Gc)Scope holds the pointer for its life time. When GcScope is destroyed, the
// pointer associated is also deleted.
class Scope {
    protected:
        int _mark;
    public:
        int mark() {
            return _mark;
        }
        int mark(int m) {
            return _mark = m;
        }
        virtual ~Scope(){};
        virtual void* mem(){};
};

template <class T> class GcScope : public virtual Scope {
    private:
        T* _mem;
        bool _isarray;
    public:
        GcScope(T* m, bool a, int phase):_mem(m),_isarray(a) {
            mark(phase);
        }
        void* mem() {
            return (void*) _mem;
        }
        ~GcScope() {
            // other kinds not implemented yet.
            if (_mem > Gc::bottom()) {
                // if it is a true new alloc.
                if (_isarray) {
                    delete[] _mem;
                } else {
                    delete _mem;
                }
            }
        }
};

// classes P and PA holds the current ownership
template <class T, bool A=false> class P {
    protected:
        T* _data;
        
        virtual void attach(T* p, bool a=A) {
            if (!p) return;
            Scope* g = Gc::getptr(p);
            if (!g)
                g = new GcScope<T>(p, a, Gc::phase());
            Gc::addptr(g, this);
        }

        virtual void detach(T* p) {
            // we take our reference out.
            Gc::rmptr(this);
        }

    public:
        P(T* p):_data(p) {
            attach(p);
        }

        P(const P<T> &p) :_data(p._data){
            attach(p._data);
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
            detach(_data);
            attach(p);
            _data = p;
            return *this;
        }

        /* assignment from pointer object. */
        P<T> &operator = (const P<T> &p) {
            detach(_data);
            attach(p._data);
            _data = p._data;
            return *this;
        }

        ~P() {
            detach(_data);
        }
};

#endif

