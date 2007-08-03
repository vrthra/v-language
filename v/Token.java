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
            throw new VException("err:type:bool",this,"Invalid type(need bool)");
        }
    }
    public char cvalue() {
        try {
            Term<Character> v = (Term<Character>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("err:type:bool",this,"Invalid type(need bool)");
        }
    }
    public int ivalue() {
        try {
            Term<Integer> v = (Term<Integer>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("err:type:int",this,"Invalid type(need int)");
        }
    }
    public double dvalue() {
        try {
            Term<Double> v = (Term<Double>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("err:type:decimal",this,"Invalid type(need decimal)");
        }
    }
    public Number numvalue() {
        if (type() == Type.TInt)
            return new Integer(ivalue());
        if (type() == Type.TDouble)
            return new Double(dvalue());
        throw new VException("err:type:number",this,"Invalid type(NaN)");
    }
    public String svalue() {
        try {
            Term<String> v = (Term<String>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("err:type:string",this,"Invalid type(need string)");
        }
    }
    public Quote qvalue() {
        try {
            Term<Quote> v = (Term<Quote>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("err:type:quote",this,"Invalid type(need quote)");
        }
    }
    public Object ovalue() {
        try {
            Term<Object> v = (Term<Object>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("err:type:object",this,"Invalid type(need object)");
        }
    }
// meta
    public VFrame fvalue() {
        try {
            Term<VFrame> v = (Term<VFrame>)this;
            return v.val;
        } catch (Exception e) {
            throw new VException("err:type:quote",this,"Invalid type(need quote)");
        }
    }

}
