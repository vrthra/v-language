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
        String s = Sym.lookup(sym);
        if (V.singleassign)
            if (_dict.containsKey(s))
                throw new VException("err:symbol_already_bound", new Term<String>(Type.TString, s),s);
        _dict.put(s,q);
    }
    private VFrame getDefinedScope(String sym) {
        if (_dict.containsKey(sym))
            return this;
        if (_parent == null)
            return null;
        return _parent.getDefinedScope(sym);
    }
    public void set(String sym, Quote q) {
        VFrame dframe = getDefinedScope(sym);
        if (dframe != null)
            dframe.def(sym, q);
        else
            throw new VException("err:symbol_not_defined", new Term<String>(Type.TString, sym),sym);
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
