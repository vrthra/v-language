package v.java;

import v.*;
import java.util.*;
import java.lang.reflect.*;

public class Helper {

    // extract the class array from this that will serve as the method param.
    public static Class[] getParamType(Quote q) {
        LinkedList<Class> arr = new LinkedList<Class>();
        for (Term t: q.tokens())
            arr.add(getJavaType(t));
        return arr.toArray(new Class[0]);
    }

    @SuppressWarnings ("unchecked")
    public static Class getArrayElementType(Term tq) {
        Quote q = tq.qvalue();
        Stack<Class> st = new Stack<Class>();
        st.push(Object.class);
        for(Term t: q.tokens()) {
            Class c = getJavaType(t);
            if (c.isPrimitive() && !st.peek().isAssignableFrom(c))
                st.push(c);
            else {
                while(st.size() > 0) {
                    Class p = st.peek();
                    if (!p.isAssignableFrom(c))
                        st.pop();
                    else{
                        if (!p.equals(c))
                            st.push(c);
                        break;
                    }
                }
            }
        }
        return st.pop();
    }

    public static Class getArrayType(Term tq) {
        return Array.newInstance(getArrayElementType(tq), 0).getClass();
    }

    public static Object getArrayObj(Term tq) {
        Class c = getArrayElementType(tq);
        Object arr = Array.newInstance(c, tq.size());
        int i = 0;

        for(Term t: tq.qvalue().tokens()) {
            Array.set(arr, i, getJavaObj(t));
            i++;
        }
        return arr;
    }

    public static Class getJavaType(Term t) {
        switch (t.type) {
            case TBool:
                return Boolean.TYPE;
            case TChar:
                return Character.TYPE;
            case TInt:
                return Integer.TYPE;
            case TDouble:
                return Double.TYPE;
            case TString:
                return String.class;
            case TObject:
                return t.ovalue().getClass();
            case TQuote:
                return getArrayType(t);
            default:
                throw new VException("err:java:type",t,"Unable to convert type :" + t.value());
        }
    }

    public static Object getJavaObj(Term t) {
        switch (t.type) {
            case TBool:
                return new Boolean(t.bvalue());
            case TChar:
                return new Character(t.cvalue());
            case TInt:
                return new Integer(t.ivalue());
            case TDouble:
                return new Double(t.dvalue());
            case TString:
                return new String(t.svalue());
            case TObject:
                return t.ovalue();
            case TQuote:
                return getArrayObj(t);
            default:
                throw new VException("err:java:obj",t, "Unable to convert value :" + t.value() + " " + t.type);
        }
    }

    // Convert a term to the most closest java object.
    // for arrays, convert to the LCM of the available classes
    public static Object[] getParams(Quote q) {
        LinkedList<Object> arr = new LinkedList<Object>();
        for (Term t: q.tokens())
            arr.add(getJavaObj(t));
        return arr.toArray(new Object[0]);
    }

    public static Term getResult(Object o) {
        String c = o.getClass().getName();
        if (c.equals("int") || c.equals("java.lang.Integer"))
            return new Term<Integer>(v.Type.TInt, (Integer)o);
        if (c.equals("long") || c.equals("java.lang.Long"))
            return new Term<Integer>(v.Type.TInt, new Integer(((Long)o).toString())); // cant help it loss of bits
        if (c.equals("double") || c.equals("java.lang.Double"))
            return new Term<Double>(v.Type.TDouble, (Double)o);
        if (c.equals("double") || c.equals("java.lang.Float"))
            return new Term<Double>(v.Type.TDouble, new Double(((Float)o).toString())); // cant help it loss of bits
        if (c.equals("char") || c.equals("java.lang.Character"))
            return new Term<Character>(v.Type.TChar, (Character)o);
        if (c.equals("boolean") || c.equals("java.lang.Boolean"))
            return new Term<Boolean>(v.Type.TBool, (Boolean)o);
        if (c.equals("java.lang.String"))
            return new Term<String>(v.Type.TString, (String)o);
        return new Term<Object>(v.Type.TObject,o);
    }

    public static Class getClass(String c) {
        try {
            return Class.forName(c);
        } catch (ClassNotFoundException e) {
            throw new VException("err:java:class_not_found", new Term<String>(v.Type.TString,c), "Java Exception : " + e.getMessage());
        }
    }

    public static Method getMethod(String c, Term m, Quote params) {
        String method = m.svalue();
        try {
            Class cls = getClass(c);
            Class[] ptypes = getParamType(params);
            return cls.getMethod(method, ptypes);
        } catch (NoSuchMethodException e) {
            throw new VException("err:java:no_such_method", new Term<String>(v.Type.TString,method), "Java Exception : " + e.getMessage());
        }
    }

    public static Constructor getConstructor(String c, Quote params) {
        try {
            Class cls = getClass(c);
            Class[] ptypes = getParamType(params);
            return cls.getConstructor(ptypes);
        } catch (NoSuchMethodException e) {
            throw new VException("err:java:no_such_constructor",new Term<String>(v.Type.TString,c), "Java Exception : " + e.getMessage());
        }
    }

    public static Field getField(String c, String method) {
        try {
            Class cls = getClass(c);
            return cls.getField(method.substring(0, method.length() - 1));
        } catch (NoSuchFieldException e) {
            throw new VException("err:java:no_such_field",new Term<String>(v.Type.TString,method), "Java Exception : " + e.getMessage());
        }
    }

    public static Term invoke(Term c, Term m, Quote params) {
        String cname = null;
        Object o = null;
        String method = m.svalue();
        if (c.type != v.Type.TSymbol) {
            o = getJavaObj(c);
            cname = o.getClass().getName();
        } else {
            cname = c.svalue();
            o = null; // static invocation.
        }

        try {
            if (method.equals("new")) {
                if (o != null)
                    throw new VException("err:java:invalid_class_name",c, "Java Exception invalid class");
                Constructor cons = getConstructor(cname, params);
                Object[] args = getParams(params);
                return getResult(cons.newInstance(args));
            } else if (method.endsWith("$")) {
                // TODO: static field.
                // Field.
                Field fld = getField(cname, method);
                // is it get or set?
                int size = ((QuoteStream)params.tokens()).size();

                if (size > 0) { // set
                    Object no = getJavaObj(((QuoteStream)params.tokens()).get(0));
                    fld.set(o, no);
                    return getResult(no);
                } else { // get
                    return getResult(fld.get(o));
                }
            } else {
                Method mtd = getMethod(cname, m, params);
                Object[] args = getParams(params);
                return getResult(mtd.invoke(o, args));
            }
        } catch (IllegalAccessException e) {
            throw new VException("err:java:invoke:illegalaccess",c,cname+"."+method + " Java Exception : " + e.getMessage());
        } catch (InvocationTargetException e) {
            throw new VException("err:java:invoke:invocationtarget", c,cname+"."+method+" Java Exception : " + e.getMessage());
        } catch (InstantiationException e) {
            throw new VException("err:java:invoke:instantiation", c,cname+"."+method+" Java Exception : " + e.getMessage());
        }
    }
}
