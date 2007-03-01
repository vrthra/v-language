package v;

public interface CharStream {
    public char read();
    public char current();
    public char peek();
    public void lexer(Lexer l);
}
