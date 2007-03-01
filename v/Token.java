package v;

@SuppressWarnings ({"unchecked"})
public abstract class Token {
    public abstract String value();
    public abstract Type type();

    public boolean bvalue() {
        Term<Boolean> v = (Term<Boolean>)this;
        return v.val;
    }
    public int ivalue() {
        Term<Integer> v = (Term<Integer>)this;
        return v.val;
    }
    public float fvalue() {
        Term<Float> v = (Term<Float>)this;
        return v.val;
    }
    public String svalue() {
        Term<String> v = (Term<String>)this;
        return v.val;
    }
    public Quote qvalue() {
        Term<Quote> v = (Term<Quote>)this;
        return v.val;
    }
}
