package v;

public class Vx extends RuntimeException {
    /**
     * Exception thrown by Quote.
     */
    private static final long serialVersionUID = 1L;

    private String _qstr = "";
    private Quote _quote = null;
    private String _msg = null;

    public Quote quote() {
        if (_quote == null)
            _quote = Util.getdef(_qstr);
        return _quote;
    }

    public String getMessage() {
        return _msg;
    }

    public Vx(Vx e, String msg) {
        _qstr = e._qstr;
        _msg = e.getMessage() + "\n\t|" + msg;
    }

    public Vx(String quote, String msg) {
        super(msg);
        _msg = "[" + quote + "] " + msg;
        _qstr = quote;
    }
}
