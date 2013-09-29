#include "producer.hpp"

#include <stdlib.h>

void Producer::finished_processing_frame() {
    struct FrameBuffer *f = (struct FrameBuffer *)malloc(sizeof(struct FrameBuffer));
    f->buffer = (uint8_t *)"Hello world!";

    out->push_frame(f);
}

void Producer::start() {
    struct FrameBuffer *f = (struct FrameBuffer *)malloc(sizeof(struct FrameBuffer));
    f->buffer = (uint8_t *)"Hello world!";

    out->push_frame(f);
}
