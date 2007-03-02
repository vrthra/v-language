package v;

public interface TokenStream extends Iterable<Term> {
    // allow setting scope for any Quotes created from
    // this stream.
    void scope(Quote q);
}
