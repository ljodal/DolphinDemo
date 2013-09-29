#ifndef controller_hpp
#define controller_hpp

#include "module.hpp"

class Controller : public Module {
    public:
        Module *in;
        Module *out;
        Controller(Module *i, Module *o);
        void push_frame(struct FrameBuffer *frame, bool gpu = false);
        void finished_processing_frame();
};

#endif
