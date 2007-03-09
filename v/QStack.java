package v;
import java.util.*;

public class QStack implements Iterable<Term> {
    Stack<Term> _stack = null;
    public QStack() {
        _stack = new Stack<Term>();
    }

    public Term push(Term t) {
        return _stack.push(t);
    }

    public Term pop() {
        return _stack.pop();
    }

    public boolean empty() {
        return _stack.empty();
    }

    public void clear() {
        _stack.clear();
    }

    public Term peek() {
        return _stack.peek();
    }

    public Iterator<Term> iterator() {
        return _stack.iterator();
    }
}
