package v;
import java.util.*;

public class Sym {
    private static HashMap<String,String> __symbols = new HashMap<String,String>();
    public static String lookup(String key) {
        if (!__symbols.containsKey(key)) {
            __symbols.put(key, key);
        }
        return __symbols.get(key);
    }
}
