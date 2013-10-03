#ifndef dolphin_connection_hpp
#define dolphin_connection_hpp

#include <stdint.h>

#include "sisci_error.h"
#include "sisci_api.h"

class DolphinConnection {
    public:
        uint32_t node_id;
        uint32_t seg_id;
        uint32_t intr_num;

        sci_remote_segment_t   seg;
        sci_remote_interrupt_t intr;
};

#endif
