#ifndef CHARSTREAM_H
#define CHARSTREAM_H
#include "common.h"
struct Lexer;
struct CharStream : public virtual Obj {
   virtual char read()=0;
   virtual char current()=0;
   virtual char peek()=0;
   virtual void lexer(Lexer* l)=0;
   virtual bool eof()=0;
};
#endif
