package v;
import java.util.*;
import java.io.*;


public class Util {
    public static void evaluate(Quote q, String buff) {
        try {
            getdef(q, buff).eval(q, true);
        } catch (Exception e) {
            throw new VException("err:eval " + buff, "eval failed " + e.getMessage());
        }
    }

    static Quote compile(Quote q, Quote v) {
        QuoteStream nts = new QuoteStream();
        for(Term t:  v.tokens())
            nts.add(t);

        return new CmdQuote(nts, q);
    }

    static Quote getdef(Quote q, String buf) {
        CharStream cs = new BuffCharStream(buf);
        return compile(q, new CmdQuote(new LexStream(cs), q));
    }

    static String getresource(String s) {
        try {
            StringBuffer buf = new StringBuffer();
            BufferedReader br = new BufferedReader(new InputStreamReader(Prologue.class.getResourceAsStream(s)));
            String line;
            while((line = br.readLine()) != null) {
                buf.append(line);
            }
            return buf.toString();
        } catch (Exception e) {
            return null;
        }
    }
}
