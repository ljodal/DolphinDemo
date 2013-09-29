#ifndef module_hpp
#define module_hpp

#include <stdint.h>

#define WIDTH   2046
#define HEIGHT  1086
#define BPP     4

/**
 * Struct to store information about a frame.
 *
 * To make it easy to transfer the data through e.g. dolphin,
 * the struct and the buffer should be allocated in one
 * continious memory block, with the struct placed first.
 */
extern "C" struct FrameBuffer {
    uint8_t *buffer; // Pointer to the image data

    uint32_t width;  // Image width
    uint32_t height; // Image height

    uint32_t stride; // Padded width of the image
    uint32_t bpp;    // Bits per pixel
};

class Module {
    public:
        /**
        * Receive an incoming frame from a module
        */
        virtual void push_frame(struct FrameBuffer *frame, bool gpu = false) {};

        /**
        * Called by a module when it has finished
        * processing a frame. This function will
        * remove the first of the frames in the
        * queue and push a new one to the module.
        */
        virtual void finished_processing_frame() {};
};

#endif
