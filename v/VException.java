package v;
public class VException extends Vx {
    Token _token = null;
    Quote _quote = null;
    String _err = null;
    StringBuffer _info = new StringBuffer();

    public VException(String err,Token t,String s2) {
        _err = err + ' ' + t.value();
        _info.append(s2);
        _token = t;
    }

    public void addLine(String s) {
        _info.append("\n\t" + s);
    }

    public Token token() {
        return _token;
    }

    public String message() {
        return _info.toString();
    }

    public String stack() {
        return _err;
    }
}

