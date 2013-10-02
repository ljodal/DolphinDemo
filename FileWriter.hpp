#ifndef filewriter_hpp
#define filewriter_hpp

#include <stdio.h>

#include "interface.hpp"

class FileWriter {
    public:
        FileWriter(char *filepath, Interface *in);
        ~FileWriter();
        void run();

    private:
        Interface *in;
        bool std;
        FILE *fp;
};

#endif
