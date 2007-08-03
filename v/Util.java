package v;
import java.util.*;
import java.io.*;


public class Util {
    public static void evaluate(String buff, VFrame q) {
        try {
            getdef(buff).eval(q);
        } catch (Exception e) {
            throw new VException("err:eval", new Term<String>(Type.TString,buff), "eval failed " + e.getMessage());
        }
    }

    static Quote compile(Quote v) {
        QuoteStream nts = new QuoteStream();
        for(Term t:  v.tokens())
            nts.add(t);

        return new CmdQuote(nts);
    }

    static Quote getdef(String buf) {
        CharStream cs = new BuffCharStream(buf);
        return compile(new CmdQuote(new LexStream(cs)));
    }

    static String getresource(String s) {
        try {
            StringBuffer buf = new StringBuffer();
            BufferedReader br = new BufferedReader(new InputStreamReader(Prologue.class.getResourceAsStream(s)));
            String line;
            while((line = br.readLine()) != null) {
                buf.append(line + '\n');
            }
            return buf.toString();
        } catch (Exception e) {
            return null;
        }
    }
}
