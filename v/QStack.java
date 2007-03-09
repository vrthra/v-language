package v;
import java.util.*;

class Node<E> {
    E data;
    Node<E> link;
    public Node(E e) {
        data = e;
    }
}

public class QStack {
    Node<Term> now = null;
    Node<Term> first = null;
    public QStack() {
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
   
    public void dump() {
        LinkedList<Term> l = new LinkedList<Term>();
        Node<Term> current = now;
        while(current.link != null) {
            l.addFirst(current.data);
            current = current.link;
        }
        V.out("[");
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
        V.outln("]");
    }

}
