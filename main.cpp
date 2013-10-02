#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "FileReader.hpp"
#include "FileWriter.hpp"

int main(int argc, char **argv) {
    // A filename must be specified!
    if (argc < 3) return EXIT_FAILURE;

    // Create a file reader and writer
    FileReader *in = new FileReader(argv[1]);
    FileWriter *out = new FileWriter(argv[2], in);

    // Run pipeline
    out->run();

    // Free memory
    delete out;
    delete in;

    return EXIT_SUCCESS;
}
