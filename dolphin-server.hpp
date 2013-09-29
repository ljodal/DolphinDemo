#ifndef dolphin_server_hpp
#define dolphin_server_hpp

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sisci_error.h"
#include "sisci_api.h"

#include "module.hpp"

// TODO
// Remove hardcoded values

class DolphinServer : public Module {
    public:
        DolphinServer(uint32_t n, uint32_t r, size_t b);
        bool setup();
        bool connect();
        void run();
        bool shutdown();

    private:
        uint32_t node_id;
        uint32_t remote_id;
        uint32_t seg_id;
        uint32_t intr_dma;
        uint32_t intr_ready;

        volatile uint8_t *buffer;

        sci_desc_t sd;
        sci_local_segment_t seg;
        sci_map_t map;
        sci_local_interrupt_t intr;
        sci_remote_interrupt_t intr_r;

        uint32_t buffer_size;
};

#endif
