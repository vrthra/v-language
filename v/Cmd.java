package v;

import java.util.*;

public abstract class Cmd implements Quote {

    HashMap<String, Object> _store = new HashMap<String, Object>();
    public HashMap<String,Object> store() {
        return _store;
    }

    public TokenStream tokens() {
        return null;
    }	
}
