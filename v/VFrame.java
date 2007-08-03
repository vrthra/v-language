package v;

import java.util.*;

public class VFrame {
    VStack _stack = null;
    HashMap<String, Quote> _dict = new HashMap<String,Quote>();
    VFrame _parent = null;

    public HashMap<String,Quote> dict() {
        return _dict;
    }

    static int _idcount = 0;
    int _id;

    public String id() {
        return "Frame[" + _id + "]";
    }

    public VFrame() {
        _parent = null;
        _stack = new VStack();
        _idcount++;
        _id = _idcount;
    }
    private VFrame(VFrame parent) {
        _parent = parent;
        _stack = parent.stack();
        _idcount++;
        _id = _idcount;
    }
    public Quote lookup(String key) {
        if (_dict.containsKey(key))
            return _dict.get(key);
        if (_parent != null)
            return _parent.lookup(key);
        return null;
    }
    public void def(String sym, Quote q) {
        if (V.singleassign)
            if (_dict.containsKey(sym))
                throw new VException("err:symbol_already_bound", new Term<String>(Type.TString, sym),sym);
        _dict.put(sym,q);
    }
    public VFrame parent() {
        return _parent;
    }
    public VFrame child() {
        return new VFrame(this);
    }
    public VStack stack() {
        return _stack;
    }
    public void dump() {
        _stack.dump();
    }
    public void reinit() {
        _stack.clear();
    }
}
