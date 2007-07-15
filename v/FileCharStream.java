package v;
import java.io.*;

public class FileCharStream extends BuffCharStream {
    public FileCharStream(String filename) throws FileNotFoundException {
        super("");
        _reader = new BufferedReader(new FileReader(filename));
    }
}
