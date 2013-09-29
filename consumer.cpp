#include "consumer.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void Consumer::push_frame(struct FrameBuffer *frame, bool gpu) {
    fprintf(stderr, "%s\n", (char *)frame->buffer);

    free(frame);

    sleep(1);

    in->finished_processing_frame();
}
