#ifndef filereader_hpp
#define filereader_hpp

#include <stdio.h>

#include "interface.hpp"

class FileReader : public Interface {
    public:
        FileReader(char *filepath);
        ~FileReader();
        const void *getResource();

    private:
        FILE *fp;
};

#endif
