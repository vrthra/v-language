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
        os << buffer;
    }
    _buf = new char[os.str().length() +1];
    std::strcpy(_buf, os.str().c_str());
    _buf[strlen(_buf)] = 0;
}

