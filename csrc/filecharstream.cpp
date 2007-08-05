#include <sstream>
#include <fstream>
#include "filecharstream.h"
#include "buffcharstream.h"

FileCharStream::FileCharStream(char* filename):BuffCharStream(0) {
    std::stringstream os;
    std::filebuf fb;
    char buffer[MaxBuf];
    fb.open(filename, std::ios::in);
    std::istream is(&fb);
    while(!is.eof()) {
        is.getline(buffer, MaxBuf);
        os << buffer << "\n";
    }
    _len = os.str().length();
    _buf = dup_str(os.str().c_str());
}

