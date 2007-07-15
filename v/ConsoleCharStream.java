package v;
import java.io.*;

public class ConsoleCharStream implements CharStream {
    BufferedReader _reader = null;

    public ConsoleCharStream() {
        _reader = new BufferedReader(new InputStreamReader(System.in));
    }

    Lexer _lexer = null;
    public void lexer(Lexer l) {
        _lexer = l;
    }

    public BufferedReader reader() {
        return _reader;
    }

    String read_nobuf() {
        try {
            if (_lexer.closed())
                System.out.print("|");
            else
                System.out.print("");
            String out = reader().readLine();
            if (out == null) //^D
                System.exit(0);
            return out;
        } catch (Exception e) {
            throw new RuntimeException(e);
        }
    }

    char _current = ' ';
    public char read() {
        if (_buf.length() > (_index + 1)) {
            ++_index;
            _current = _buf.charAt(_index);
        } else {
            _buf.setLength(0);
            _index = 0;
            String b = read_nobuf();
            if (b.length() > 0) {
                _buf.append(b);
            } else {
                _buf.append('\n');
            }
            _current = _buf.charAt(_index);
        }
        return _current;
    }

    public char peek() {
        if (_buf.length() > (_index + 1)) {
            // can read with out reading.... :)
            return _buf.charAt(_index + 1);
        } else {
            return 0;
        }
    }

    public char current() {
        return _current;
    }

    StringBuffer _buf = new StringBuffer();
    int _index = 0;

    public int index() {
        return _index;
    }
}
