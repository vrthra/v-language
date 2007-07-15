#ifndef TOKEN_H
#define TOKEN_H
class Quote;
class VFrame;
class Token {
    public:
        virtual char* value()=0;
        virtual Type type();
        virtual bool bvalue();
        virtual char cvalue();
        virtual int ivalue();
        virtual double dvalue();
        virtual char* svalue();
        virtual Quote* qvalue();
        virtual VFrame* fvalue();
};
#endif
