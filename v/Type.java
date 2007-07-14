package v;
public enum Type {
    TSymbol,
    TQuote,
    TFrame,
    TString,
    TInt,
    TDouble,
    TChar,
    TBool,
    TObject, // only for java.

    // ----------- Used only in lexer
    TOpen,
    TClose
};

