package v;
public class VException extends Vx {
    public VException(String s1, String s2) {
        super(s1,s2);
    }

    public VException(Vx e, String s) {
        super(e,s);
    }
}
