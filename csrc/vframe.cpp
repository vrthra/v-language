#include "vframe.h"
#include "sym.h"
#include "vstack.h"
#include "vexception.h"
#include "quotestream.h"
#include "term.h"
#include "cmdquote.h"

bool singleassign(); // defined in v.cpp
int VFrame::_idcount = 0;
VFrame::VFrame():_parent(0),_stack(new (collect) VStack()),_id(0) {
    _idcount++;
    _id = _idcount;
}
VFrame::VFrame(VFrame* parent):_parent(parent),_stack(parent->stack()),_id(0) {
    _idcount++;
    _id = _idcount;
}
int VFrame::id() {
    return _id;
}
bool VFrame::hasKey(char* key) {
    QMap::iterator i = _dict.find(key);
    if (i != _dict.end())
        return true;
    return false;
}
Quote* VFrame::lookup(char* key) {
    if (hasKey(key))
        return _dict[key];
    if (_parent)
        return _parent->lookup(key);
    return 0;
}
Quote* VFrame::words() {
    QuoteStream_ nts = new (collect) QuoteStream();
    for(QMap::iterator i = _dict.begin(); i!= _dict.end(); i++) {
        nts->add(new (collect) Term(TSymbol, i->first));
    }
    return new (collect) CmdQuote(nts); 
}
void VFrame::def(char* sym, Quote* q) {
    Char_ s = Sym::lookup(sym);
    if (singleassign() && hasKey(s))
        throw VException("err:symbol_already_bound", new (collect) Term(TSymbol, s), s);
    _dict[s] = q;
}

VFrame* VFrame::parent() {
    return _parent;
}
VFrame* VFrame::child() {
    return new (collect) VFrame(this);
}
VStack* VFrame::stack() {
    return _stack;
}
void VFrame::dump() {
    _stack->dump();
}
void VFrame::reinit() {
    _stack->clear();
}
char* VFrame::to_s() {
    return "<frame>";
}
