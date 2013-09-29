#ifndef producer_hpp
#define producer_hpp

#include "module.hpp"

class Producer : public Module {
    public:
        Module *out;
        void finished_processing_frame();
        void start();
};

#endif
