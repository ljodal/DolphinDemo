#ifndef consumer_hpp
#define consumer_hpp

#include "module.hpp"

class Consumer : public Module {
    public:
        Module *in;
        void push_frame(struct FrameBuffer *frame, bool gpu = false);
};

#endif
