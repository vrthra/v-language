#include <sstream>
#include <fstream>
#include "filecharstream.h"
#include "buffcharstream.h"

FileCharStream::FileCharStream(char* filename) {
    std::sstream os;
    std::filebuf fb;
    fb.open(filename, std::ios::in);
    istream is(&fb);
    while(!is.eof()) {
        os << fb.getline();
    }
    _buf = new char[os.str().length()];
    std::strcpy(_buf, os.str()->cstr());
    BuffCharStream(_buf);
}

