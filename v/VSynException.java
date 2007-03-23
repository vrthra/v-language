package v;
public class VSynException extends Vx {
    public VSynException(String s1, String s2) {
        super(s1,s2);
    }

    public VSynException(Vx e, String s) {
        super(e,s);
    }
}
