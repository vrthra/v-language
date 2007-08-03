
all: v

v: lexiterator.o quotestream.o vstack.o consolecharstream.o lexer.o vframe.o cmdquote.o quoteiterator.o lexstream.o v.o term.o vexception.o filecharstream.o prologue.o buffcharstream.o
	g++ -o v lexiterator.o quotestream.o vstack.o consolecharstream.o lexer.o vframe.o cmdquote.o quoteiterator.o lexstream.o v.o term.o vexception.o filecharstream.o prologue.o buffcharstream.o -L /usr/local/lib -lgc

lexiterator.o: lexiterator.cpp lexiterator.h quoteiterator.h tokeniterator.h common.h quotestream.h tokenstream.h lexer.h token.h type.h num.h term.h cmdquote.h quote.h vexception.h vx.h
	g++ -c -g -I /usr/local/include lexiterator.cpp

quotestream.o: quotestream.cpp quotestream.h tokenstream.h common.h quoteiterator.h tokeniterator.h
	g++ -c -g -I /usr/local/include quotestream.cpp

vstack.o: vstack.cpp vstack.h term.h common.h type.h token.h num.h tokeniterator.h quotestream.h tokenstream.h cmdquote.h quote.h vexception.h vx.h
	g++ -c -g -I /usr/local/include vstack.cpp

consolecharstream.o: consolecharstream.cpp consolecharstream.h charstream.h common.h lexer.h v.h
	g++ -c -g -I /usr/local/include consolecharstream.cpp

lexer.o: lexer.cpp lexer.h common.h type.h term.h token.h num.h charstream.h vexception.h vx.h v.h
	g++ -c -g -I /usr/local/include lexer.cpp

vframe.o: vframe.cpp vframe.h common.h vstack.h term.h type.h token.h num.h vexception.h vx.h quotestream.h tokenstream.h cmdquote.h quote.h
	g++ -c -g -I /usr/local/include vframe.cpp

cmdquote.o: cmdquote.cpp cmdquote.h quote.h common.h vframe.h vstack.h term.h type.h token.h num.h cmd.h quotestream.h tokenstream.h lexstream.h buffcharstream.h charstream.h quoteiterator.h tokeniterator.h vexception.h vx.h prologue.h
	g++ -c -g -I /usr/local/include cmdquote.cpp

quoteiterator.o: quoteiterator.cpp quoteiterator.h tokeniterator.h common.h quotestream.h tokenstream.h
	g++ -c -g -I /usr/local/include quoteiterator.cpp

lexstream.o: lexstream.cpp lexstream.h quotestream.h tokenstream.h common.h lexiterator.h quoteiterator.h tokeniterator.h
	g++ -c -g -I /usr/local/include lexstream.cpp

v.o: v.cpp type.h term.h common.h token.h num.h vstack.h vframe.h v.h lexstream.h quotestream.h tokenstream.h filecharstream.h buffcharstream.h charstream.h consolecharstream.h vexception.h vx.h prologue.h cmdquote.h quote.h
	g++ -c -g -I /usr/local/include v.cpp

term.o: term.cpp term.h common.h type.h token.h num.h quote.h quotestream.h tokenstream.h vframe.h charstream.h vexception.h vx.h
	g++ -c -g -I /usr/local/include term.cpp

vexception.o: vexception.cpp vexception.h common.h vx.h token.h type.h num.h charstream.h cmdquote.h quote.h
	g++ -c -g -I /usr/local/include vexception.cpp

filecharstream.o: filecharstream.cpp filecharstream.h buffcharstream.h charstream.h common.h
	g++ -c -g -I /usr/local/include filecharstream.cpp

prologue.o: prologue.cpp prologue.h common.h token.h type.h num.h term.h cmd.h quote.h cmdquote.h vstack.h vframe.h quotestream.h tokenstream.h quoteiterator.h tokeniterator.h filecharstream.h buffcharstream.h charstream.h lexstream.h vexception.h vx.h v.h
	g++ -c -g -I /usr/local/include prologue.cpp

buffcharstream.o: buffcharstream.cpp buffcharstream.h charstream.h common.h
	g++ -c -g -I /usr/local/include buffcharstream.cpp


test:
	./v ../scripts/test.v

clean:
	rm *.o

