#include <sstream>
#include <fstream>
#include <string>
#include "filecharstream.h"
#include "buffcharstream.h"

FileCharStream::FileCharStream(char* filename):BuffCharStream(0) {
    std::stringstream os;
    std::filebuf fb;
    std::string buffer;
    fb.open(filename, std::ios::in);
    std::istream is(&fb);
    while(!getline(is,buffer).eof())
        os << buffer << "\n";
    _len = os.str().length();
    _buf = dup_str(os.str().c_str());
}

