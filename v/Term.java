package v;

enum Type {
    TSymbol,
    TQuote,
    TString,
    TInt,
    TFloat,
    TChar,
    TBool,

    // ----------- Used only in lexer
    TOpen,
    TClose
};

class Term <T> implements Token {
    public Type type;
    public T val;

    public Term(Type t, T v) {
        type = t;
        val = v;
    }

    public String value() {
        return val.toString();
    }

    public Type type() {
        return type;
    }
}
