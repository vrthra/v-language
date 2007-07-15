#ifndef V_H
#define V_H

class V {
    public:
    static const char* version;
    static bool singleassign;
    static void banner();
    static void outln(char* var);
    static void outln(char* var, char* v);
    static void main(int argc, char* args[]);
};

#endif
