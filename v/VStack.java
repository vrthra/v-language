package v;
import java.util.*;

class Node<E> {
    E data;
    Node<E> link;
    public Node(E e) {
        data = e;
    }
}

public class VStack {
    Node<Term> now = null;
    Node<Term> first = null;
    public VStack() {
        now = new Node<Term>(null);
        first = now;
    }

    public Node<Term> now() {
        return now;
    }

    public Node<Term> now(Node<Term> n) {
        now = n;
        return n;
    }

    public Term push(Term t) {
        Node<Term> n = new Node<Term>(t);
        n.link = now;
        now = n;
        return now.data;
    }

    public Term pop() {
        if (now == null || now.data == null)
            throw new VException("err:stack_empty","Empty Stack.");
        Term t = now.data;
        now = now.link;
        return t;
    }

    public boolean empty() {
        return now.link == null;
    }

    public void clear() {
        now = first;
    }

    public Term peek() {
        return now.data;
    }

    private List<Term> getlist() {
        LinkedList<Term> l = new LinkedList<Term>();
        Node<Term> current = now;
        while(current != null && current.link != null) {
            l.addFirst(current.data);
            current = current.link;
        }
        return l;
    }

    public Quote quote() {
        QuoteStream qs = new QuoteStream();
        List<Term> l = getlist();
        for(Term t: l)
            qs.add(t);
        return new CmdQuote(qs);
    }
  
    public void dequote(Quote q) {
        Node<Term> current = now;
        Iterator<Term> it = (Iterator<Term>)q.tokens().iterator();

        now = new Node<Term>(null);
        first = now;

        while(it.hasNext()) {
            push(it.next());
        }
    }

    public void dump() {
        List<Term> l = getlist();
        V.out("(");
        boolean first = true;
        for(Term t: l) {
            if (first) first = false;
            else V.out(" ");
            switch (t.type) {
                case TString:
                    V.out("'" + t.svalue() + "'");
                    break;
                case TChar:
                    V.out("~" + t.svalue());
                    break;
                default:
                    V.out(t.value());
            }
        }
        V.outln(")");
    }
}
