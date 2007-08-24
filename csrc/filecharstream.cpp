#include <sstream>
#include <fstream>
#include <string>
#include "type.h"
#include "term.h"
#include "vexception.h"
#include "filecharstream.h"
#include "buffcharstream.h"
FileCharStream::FileCharStream(char* filename):BuffCharStream(0) {
    std::stringstream os;
    std::filebuf fb;
    std::string buffer;
    std::filebuf* f = fb.open(filename, std::ios::in);
    if (!f)
        throw VException("err:fopen", new (collect) Term(TSymbol, filename), filename);
    std::istream is(&fb);
    while(!getline(is,buffer).eof())
        os << buffer << "\n";
    _len = os.str().length();
    _buf = dup_str(os.str().c_str());
}

