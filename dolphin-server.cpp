#include "dolphin-server.hpp"

#include <stdio.h>


// TODO
// Find actual local adapter number

#define NO_FLAGS      0
#define NO_CALLBACK   NULL

// Interrupts
#define DMA_READY     7
#define SRV_READY     1

#define ADAPTER_NUM   1
#define OFFSET        0

extern "C" {
    typedef struct {
        uint32_t size;
        void *data;
    } package;
    sci_callback_action_t  callback(void *arg,
                                    sci_local_segment_t segment,
                                    sci_segment_cb_reason_t reason,
                                    unsigned int nodeId, 
                                    unsigned int localAdapterNo,
                                    sci_error_t error) {
        fprintf(stderr, "Segment callback, reason: %d\n", reason);

        return SCI_CALLBACK_CONTINUE;
    }
}

DolphinServer::DolphinServer(uint32_t n, uint32_t r, size_t b): node_id(n), remote_id(r), buffer_size(b) {
    seg_id = (node_id << 15) | remote_id;

    // Set up interrupt numbers
    intr_dma = (node_id << 15) | DMA_READY;
    intr_ready = (remote_id << 15) | SRV_READY;
}

bool DolphinServer::setup() {
    sci_error_t error;

    // Open the dolphin device
    SCIOpen(&sd, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIOpen failed - Error code 0x%x\n",error);
        return false;
    }

    // Create a segment
    SCICreateSegment(sd, &seg, seg_id, buffer_size, callback, NULL, SCI_FLAG_USE_CALLBACK, &error);
    //SCICreateSegment(sd, &seg, seg_id, buffer_size, NULL, NULL, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCICreateSegment failed - Error code 0x%x\n",error);
        return false;
    }

    // Prepare the segment
    SCIPrepareSegment(seg, ADAPTER_NUM, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIPrepareSegment failed - Error code 0x%x\n",error);
        return false;
    }

    // Map the segment into user space
    buffer = (volatile uint8_t *)SCIMapLocalSegment(seg, &map, OFFSET, buffer_size, NULL, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr, "SCIMapLocalSegment failed - Error code 0x%x\n", error);
        return false;
    }

    // Set up the "dma finished" interrupt
    SCICreateInterrupt(sd, &intr, ADAPTER_NUM, &intr_dma, NO_CALLBACK, NULL, SCI_FLAG_FIXED_INTNO, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCICreateInterrupt failed - Error code 0x%x\n",error);
        return false;
    }

    // Publish the segment
    SCISetSegmentAvailable(seg, ADAPTER_NUM, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCISetSegmentAvailable failed - Error code 0x%x\n",error);
        return false;
    }

    // Everything went fine, dolphin should now be
    // ready to use
    return true;
}

bool DolphinServer::connect() {
    sci_error_t error;

    fprintf(stderr, "Opening connections... ");

    do {
        SCIConnectInterrupt(sd,
                            &intr_r,
                            remote_id,
                            ADAPTER_NUM,
                            intr_ready,
                            SCI_INFINITE_TIMEOUT,
                            NO_FLAGS,
                            &error);

        if (error != SCI_ERR_OK) {
            fprintf(stderr, "SCIConnectInterrupt failed - error code 0x%x\n", error);
        }
        usleep(100000);
    } while (error != SCI_ERR_OK);

    fprintf(stderr, "Done!\n");

    return true;
}

void DolphinServer::run() {
    sci_error_t error;

    fprintf(stderr, "Waiting for incomming data!\n");

    for(;;) {
        SCIWaitForInterrupt(intr, SCI_INFINITE_TIMEOUT, NO_FLAGS, &error);
        if (error != SCI_ERR_OK) {
            printf("\n");
            fprintf(stderr,"SCIWaitForInterrupt failed - Error code 0x%x\n",error);
            return;
        }

        // Got buffer
        fprintf(stderr, "Received data, \"processing\", done\n");
        package *p = (package *)buffer;
        fprintf(stderr, "%d bytes\n", p->size);

        // Send interrupt back
        SCITriggerInterrupt(intr_r, NO_FLAGS, &error);
        if (error != SCI_ERR_OK) {
            fprintf(stderr,"SCITriggerInterrup failed - Error code 0x%x\n",error);
            return;
        }
    }
}

bool DolphinServer::shutdown() {
    // TODO
    // Error checking
    sci_error_t error;

    SCIDisconnectInterrupt(intr_r, NO_FLAGS, &error);

    SCIUnmapSegment(map, NO_FLAGS, &error);
    SCIRemoveSegment(seg, NO_FLAGS, &error);
    SCIClose(sd, NO_FLAGS, &error);

    // For now, assume everything went fine
    return true;
}
