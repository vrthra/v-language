#ifndef DEFS_H
#define DEFS_h
#include "ptr.h"

typedef P<void> Void_;
typedef P<char> Char_;

struct Token;
typedef P<Token> Token_;
struct Term;
typedef P<Term> Term_;
struct Lexer;
typedef P<Lexer> Lexer_;

struct Node;
typedef P<Node> Node_;
struct CNode;
typedef P<CNode> CNode_;

struct Quote;
typedef P<Quote> Quote_;
struct CmdQuote;
typedef P<CmdQuote> CmdQuote_;

struct LexIterator;
typedef P<LexIterator> LexIterator_;
struct QuoteIterator;
typedef P<QuoteIterator> QuoteIterator_;
struct TokenIterator;
typedef P<TokenIterator> TokenIterator_;

struct CharStream;
typedef P<CharStream> CharStream_;
struct BuffCharStream;
typedef P<BuffCharStream> BuffCharStream_;
struct FileCharStream;
typedef P<FileCharStream> FileCharStream_;
struct ConsoleCharStream;
typedef P<ConsoleCharStream> ConsoleCharStream_;
struct LexStream;
typedef P<LexStream> LexStream_;
struct TokenStream;
typedef P<TokenStream> TokenStream_;
struct QuoteStream;
typedef P<QuoteStream> QuoteStream_;

struct VFrame;
typedef P<VFrame> VFrame_;
struct VStack;
typedef P<VStack> VStack_;

struct Vx;
typedef P<Vx> Vx_;
struct VException;
typedef P<VException> VException_;
struct VSynException;
typedef P<VSynException> VSynException_;


struct PQuote;
typedef P<PQuote> PQuote_;

#endif
