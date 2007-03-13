package v;

public class Term <T> extends Token {
    public Type type;
    public T val;

    public Term(Type t, T v) {
        type = t;
        val = v;
    }

    public String value() {
        if (type == Type.TObject)
            return "{" + val.toString() + "}";
        if (type == Type.TChar)
            return '~' + val.toString();
        return val.toString();
    }

    public Type type() {
        return type;
    }

    public int size() {
        if (type != Type.TQuote)
            return 1;
        else
            return ((QuoteStream)qvalue().tokens()).size();
    }
}
