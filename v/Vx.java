package v;

public abstract class Vx extends RuntimeException {
    abstract String message();
    abstract String stack();
    abstract void addLine(String var);
}
