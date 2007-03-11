package v;

@SuppressWarnings ({"unchecked"})
public abstract class Token {
    public abstract String value();
    public abstract Type type();

    public boolean bvalue() {
        try {
            Term<Boolean> v = (Term<Boolean>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("Invalid type(bool) for " + value());
        }
    }
    public int ivalue() {
        try {
            Term<Integer> v = (Term<Integer>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("Invalid type(int) for " + value());
        }
    }
    public float fvalue() {
        try {
            Term<Float> v = (Term<Float>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("Invalid type(float) for " + value());
        }
    }
    public String svalue() {
        try {
            Term<String> v = (Term<String>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("Invalid type(string) for " + value());
        }
    }
    public Quote qvalue() {
        try {
            Term<Quote> v = (Term<Quote>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("Invalid type(quote) for " + value());
        }
    }
}
