#include "vframe.h"

VFrame::VFrame() {
}
VFrame::VFrame(VFrame* parent) {
}
QMap& VFrame::dict() {
    return _id;
}
char* VFrame::id() {
    return 0;
}
Quote* VFrame::lookup(char* key) {
    return 0;
}
VFrame* VFrame::parent() {
    return 0;
}
VFrame* VFrame::child() {
    return 0;
}
VStack* VFrame::stack() {
    return 0;
}
void VFrame::dump() {
}
void VFrame::reinit() {
}
