#ifndef dolphin_client_hpp
#define dolphin_client_hpp

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "sisci_error.h"
#include "sisci_api.h"

#include "interface.hpp"
#include "dolphin-connection.hpp"

class DolphinClient {
    public:
        DolphinClient(uint32_t n, uint32_t r, size_t b);
        bool setup();
        bool connect();
        bool shutdown();
        //bool multicast(uint8_t *data, size_t length);
        bool run(Interface *in);

    private:
        uint32_t node_id;
        uint32_t remote_id;
        uint32_t seg_id;
        uint32_t intr_ready;

        volatile uint8_t *buffer;

        sci_desc_t sd;
        sci_local_segment_t seg;
        sci_map_t map;
        sci_dma_queue_t dma_queue;
        sci_local_interrupt_t intr;

        size_t buffer_size;

        DolphinConnection *connection;

        void next_callback(void *ptr) {
            fprintf(stderr, "Callback!\n");
        }
};

#endif
