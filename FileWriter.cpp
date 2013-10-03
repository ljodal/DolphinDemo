#include "FileWriter.hpp"

#include <stdlib.h>
#include <string.h>

FileWriter::FileWriter(char *filepath, Interface *i) {
    in = i;
    if (strcmp(filepath,"-") == 0) {
        fp = stdout;
        std = false;
    } else {
        fp = fopen(filepath, "w");

        if (fp == NULL) {
            fprintf(stderr, "Unable to open file!\n");
            exit(EXIT_FAILURE);
        }

        std = true;
    }
}

void FileWriter::run() {
    for (;;) {
        const void *buf = in->getResource();
        const struct resource *r = (const struct resource *)buf;

        // We've reached the end of the file
        if (buf == NULL) return;

        fprintf(stderr, "Writing to file %zu\n", r->size);

        fwrite(((char *)buf)+sizeof(struct resource), r->size, 1, fp);

        fprintf(stderr, "Wrote to file\n");

        free((void *)buf);
    };
}

FileWriter::~FileWriter() {
    if (!std) {
        fclose(fp);
    }
}
