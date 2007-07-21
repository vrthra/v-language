#ifndef FILECHARSTREAM_H
#define FILECHARSTREAM_H
#include "buffcharstream.h"
class FileCharStream : public BuffCharStream {
    public:
        FileCharStream(char* filename);
};
#endif
