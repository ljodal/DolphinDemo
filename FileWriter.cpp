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

        fwrite(&r[1], 1, r->size, fp);

        free((void *)buf);
    };
}

FileWriter::~FileWriter() {
    if (!std) {
        fclose(fp);
    }
}
