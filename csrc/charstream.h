#ifndef CHARSTREAM_H
#define CHARSTREAM_H
struct Lexer;
struct CharStream {
   virtual char read()=0;
   virtual char current()=0;
   virtual char peek()=0;
   virtual void lexer(Lexer* l)=0;
};
#endif
