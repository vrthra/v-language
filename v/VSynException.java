package v;
public class VSynException extends Vx {
    StringBuffer _info = new StringBuffer();
    String _err = null;
    public VSynException(String err, String info) {
        _err = err;
        _info.append(info);
    }
    String message() {
        return _err;
    }
    String stack() {
        return _info.toString();
    }
    public void addLine(String s) {
        _info.append("\n\t" + s);
    }
}
