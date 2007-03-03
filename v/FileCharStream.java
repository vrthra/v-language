package v;
import java.io.*;

public class FileCharStream implements CharStream {
    BufferedReader _reader = null;

    public FileCharStream(String filename) throws FileNotFoundException {
        _reader = new BufferedReader(new FileReader(filename));
    }

    Lexer _lexer = null;
    public void lexer(Lexer l) {
        _lexer = l;
    }

    public BufferedReader reader() {
        return _reader;
    }

    char _current = 0;
    public char read() {
        if (_next != 0) {
            _current = _next;
            _next = 0;
            return _current;
        } else {
            _current = next();
            return _current;
        }
    }

    public char peek() {
        if (_next == 0)
            _next = next();
        return _next;
    }

    char _next = 0;
    private char next() {
        int i = 0;
        try {
            i = reader().read();
            if (i == -1)
                return 0;
        } catch (Exception e) {
            e.printStackTrace();
            return 0;
        }
        return (char)i;
    }

    public char current() {
        return _current;
    }
}
