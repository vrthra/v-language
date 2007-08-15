#ifndef PTR_H
#define PTR_H
// classes P and PA holds the current ownership
template <class T, bool A=false> class P {
    public:
        T* val;
        P(T* p):val(p) {
            //attach(p);
        }

        P(const P<T,A> &p) :val(p.val){
            //attach(p.val);
        }

        /* return raw pointer */
        T *operator ()() const {
            return val;
        }

        /* convert to raw pointer. */
        operator T *() const {
            return val;
        }

        T *operator ->() const {
            return val;
        }

        bool operator == (const T *p) const {
            return val == p;
        }

        bool operator != (const T *p) const {
            return val != p;
        }

        bool operator == (const P<T,A> &p) const {
            return val == p.val;
        }

        bool operator != (const P<T,A> &p) const {
            return val != p.val;
        }

        /* assignment from raw pointer.
         * register and continue.
         */
        P<T,A> &operator = (T *p) {
            //detach(val);
            //attach(p);
            val = p;
            return *this;
        }

        /* assignment from pointer object. */
        P<T,A> &operator = (const P<T,A> &p) {
            //detach(val);
            //attach(p.val);
            val = p.val;
            return *this;
        }

        ~P() {
            //detach(val);
        }
};
#endif
