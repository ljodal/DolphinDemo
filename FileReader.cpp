#include "FileReader.hpp"

#include <stdlib.h>

FileReader::FileReader(char *filepath) {
    fp = fopen(filepath, "r");

    if (fp == NULL) {
        fprintf(stderr, "Unable to open file!\n");
        exit(EXIT_FAILURE);
    }
}

const void *FileReader::getResource() {
    fprintf(stderr, "Get resource\n");

    if (feof(fp)) {
        return NULL;
    }

    struct resource *r = (struct resource *)malloc(BUFSIZE);
    r->size = fread((void *)&r[1], 1, BUFSIZE-sizeof(struct resource), fp);

    return r;
}

FileReader::~FileReader() {
    fclose(fp);
}
