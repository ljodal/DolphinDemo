#include "controller.hpp"

Controller::Controller(Module *i, Module *o) {
    in = i;
    out = o;
}

void Controller::push_frame(struct FrameBuffer *frame, bool gpu) {
    // Store to cache
    // Check if the other end is ready to process a frame
    // If so, push it

    out->push_frame(frame, gpu);
}

void Controller::finished_processing_frame() {
    // Check if cache is empty
    // If not, push a new frame to the module

    in->finished_processing_frame();
}
