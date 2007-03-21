package v;

import java.util.*;

public abstract class Cmd implements Quote {
    public Quote _parent = null;

    public Cmd(Quote parent) {
        _parent = parent;
        _idcount++;
        _id = _idcount;
        V.debug("Creating " + id() + " parent is " + _parent.id());
    }

    public Quote clone() {
        return this; // there is no danger of saving state for cmd.
    }

    HashMap<String, Object> _store = new HashMap<String, Object>();
    public HashMap<String,Object> store() {
        return _store;
    }


    static int _idcount = 0;
    int _id;
    public String id() {
        return "Cmd[" + _id + "]";
    }

    public HashMap<String,Quote> bindings() {
        throw new VException("err:internal:cmd:bindings","Commands does not have bindings.");
    }

    public Quote lookup(String key) {
        return _parent.lookup(key);
    }

    public QStack stack() {
        throw new VException("err:internal:cmd:stack","Commands does not have a stack.");
    }

    public Quote parent() {
        throw new VException("err:internal:cmd:parent","Commands does not have a parent?.");
    }

    public void def(String sym, Quote q) {
        throw new VException("err:internal:cmd:def","Commands can not define internal commands.");
    }

    public TokenStream tokens() {
        throw new VException("err:internal:cmd:tokens","Commands can not have tokens.");
    }	

    public void eval(Quote scope, boolean on_parent) {
        throw new VException("err:internal:cmd:eval","Commands can not apply on parents.");
    }
}
