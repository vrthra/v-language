#ifndef TYPE_H
#define TYPE_H
enum Type {
    TSymbol,
    TQuote,
    TFrame,
    TString,
    TInt,
    TDouble,
    TChar,
    TBool,
    TObject,

    // ----------- Used only in lexer
    TOpen,
    TClose
};
#endif
