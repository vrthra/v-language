package v;
public enum Type {
    TSymbol,
    TQuote,
    TString,
    TInt,
    TFloat,
    TChar,
    TBool,
    TObject, // only for java.

    // ----------- Used only in lexer
    TOpen,
    TClose
};

