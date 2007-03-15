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
            throw new VException(value() + ">Invalid type(need bool)");
        }
    }
    public char cvalue() {
        try {
            Term<Character> v = (Term<Character>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException(value() + ">Invalid type(need bool)");
        }
    }
    public int ivalue() {
        try {
            Term<Integer> v = (Term<Integer>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException(value() + ">Invalid type(need int)");
        }
    }
    public double dvalue() {
        try {
            Term<Double> v = (Term<Double>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException(value() + ">Invalid type(need double)");
        }
    }
    public String svalue() {
        try {
            Term<String> v = (Term<String>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException(value() + ">Invalid type(need string)");
        }
    }
    public Quote qvalue() {
        try {
            Term<Quote> v = (Term<Quote>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException(value() + ">Invalid type(need quote)");
        }
    }
    public Object ovalue() {
        try {
            Term<Object> v = (Term<Object>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException(value() + ">Invalid type(need object)");
        }
    }
}
