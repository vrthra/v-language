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
    public char cvalue() {
        try {
            Term<Character> v = (Term<Character>)this;
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
    public double dvalue() {
        try {
            Term<Double> v = (Term<Double>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("Invalid type(double) for " + value());
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
    public Object ovalue() {
        try {
            Term<Object> v = (Term<Object>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("Invalid type(object) for " + value());
        }
    }
}
