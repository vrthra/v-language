package v;

public class VException extends RuntimeException {
    /**
     * Exception thrown by Quote.
     */
    private static final long serialVersionUID = 1L;

    private String _qstr = "";
    private Quote _quote = null;
    private String _msg = null;

    public Quote quote(Quote parent) {
        if (_quote == null)
            _quote = Prologue.getdef(parent, _qstr);
        return _quote;
    }

    public String getMessage() {
        return _msg;
    }

    public VException(VException e, String msg) {
        _qstr = e._qstr;
        _msg = e.getMessage() + "\n\t|" + msg;
    }

    public VException(String quote, String msg) {
        super(msg);
        _msg = msg;
        _qstr = quote;
    }
}
