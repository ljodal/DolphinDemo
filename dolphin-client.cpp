#include "dolphin-client.hpp"

#define NO_FLAGS      0
#define NO_CALLBACK   NULL

// Interrupts
#define DMA_READY     7
#define SRV_READY     1

#define ADAPTER_NUM   1
#define RECEIVERS     1
#define OFFSET        0

// Switch DMA interface based on the device
#ifndef ADAPTER_IS_PSB66
#define NEW_SISCI_DMA_INTERFACE
#endif

DolphinClient::DolphinClient(uint32_t n, uint32_t r, size_t b): node_id(n), remote_id(r), buffer_size(b) {
    // Set up interrupt numbers
    intr_ready = (node_id << 15) | SRV_READY;

    // Initialize a connection
    // TODO Create a setter for this
    connection = new DolphinConnection();
    connection->node_id = remote_id;
    connection->seg_id = (remote_id << 15) | node_id;
    connection->intr_num = (remote_id << 15) | DMA_READY;
}

bool DolphinClient::setup() {
    sci_error_t error;


    // Open the dolphin device
    SCIOpen(&sd, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIOpen failed - Error code 0x%x\n",error);
        return false;
    }

    SCICreateSegment(sd, &seg, seg_id, buffer_size, NO_CALLBACK, NULL, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCICreateSegment failed - Error code 0x%x\n",error);
        return false;
    }

    SCIPrepareSegment(seg, ADAPTER_NUM, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIPrepareSegment failed - Error code 0x%x\n",error);
        return false;
    }

    buffer = (volatile uint8_t *)SCIMapLocalSegment(seg, &map, OFFSET, buffer_size, NULL, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIMapLocalSegment failed - Error code 0x%x\n",error);
        return false;
    }

    SCICreateDMAQueue(sd, &dma_queue, ADAPTER_NUM, RECEIVERS, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCICreateDMAQueue failed - Error code 0x%x\n",error);
        return false;
    }

    // Set up the "ready for next" interrupt
    SCICreateInterrupt(sd, &intr, ADAPTER_NUM, &intr_ready, NO_CALLBACK, NULL, SCI_FLAG_FIXED_INTNO, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCICreateInterrupt failed - Error code 0x%x\n",error);
        return false;
    }
}

bool DolphinClient::shutdown() {
    // TODO
    // Error checking
    sci_error_t  error;

    //Disconnect segment
    SCIDisconnectSegment(connection->seg, NO_FLAGS, &error);

    // Disconnect interrupt
    SCIDisconnectInterrupt(connection->intr, NO_FLAGS, &error);

    delete connection;

    SCIRemoveDMAQueue(dma_queue, NO_FLAGS, &error);
    SCIUnmapSegment(map, NO_FLAGS, &error);
    SCIRemoveSegment(seg, NO_FLAGS, &error);
    SCIClose(sd, NO_FLAGS, &error);

    return true;
}

bool DolphinClient::connect() {
    sci_error_t error;

    fprintf(stderr, "Remote id: %d\n", connection->node_id);
    fprintf(stderr, "Segment id: %d\n", connection->seg_id);
    fprintf(stderr, "Interrupt num: %d\n", connection->intr_num);

    fprintf(stderr, "Opening connections... ");

    do {
        SCIConnectSegment(sd,
                          &connection->seg,
                          connection->node_id,
                          connection->seg_id,
                          ADAPTER_NUM,
                          NO_CALLBACK,
                          NULL,
                          SCI_INFINITE_TIMEOUT,
                          NO_FLAGS,
                          &error);

        /*
        if (error != SCI_ERR_OK) {
            fprintf(stderr, "SCIConnectSegment failed - error code, 0x%x\n", error);
        }
        */
        usleep(100000);
    } while (error != SCI_ERR_OK);

    do {
        SCIConnectInterrupt(sd,
                            &connection->intr,
                            connection->node_id,
                            ADAPTER_NUM,
                            connection->intr_num,
                            SCI_INFINITE_TIMEOUT,
                            NO_FLAGS,
                            &error);

        /*
        if (error != SCI_ERR_OK) {
            fprintf(stderr, "SCIConnectInterrupt failed - error code 0x%x\n", error);
        }
        */
        usleep(100000);
    } while (error != SCI_ERR_OK);

    fprintf(stderr, "Done!\n");

    return true;
}

bool DolphinClient::multicast(uint8_t *data, size_t length) {
    sci_error_t error;
    sci_dma_queue_state_t dma_queue_state;

    if (length > buffer_size) {
        fprintf(stderr, "DolphinClient::multicast failed - Data larger than buffer size!\n");
        fprintf(stderr, "Buffer: %d, length: %d\n", buffer_size, length);
        return false;
    }

    // First copy data to the buffer
    memcpy((void *)buffer, data, length);

#ifdef NEW_SISCI_DMA_INTERFACE
    DISStartDmaTransfer(dma_queue,
            seg,
            connection->seg,
            OFFSET,// Local offset
            buffer_size,
            OFFSET, // Remote offset
            NO_CALLBACK,
            NULL,
            NO_FLAGS,
            &error);

    if (error != SCI_ERR_OK) {
        fprintf(stderr,"DISStartDmaTransfer failed - Error code 0x%x\n",error);
        return false;
    }

#else
    SCIEnqueueDMATransfer(dma_queue, seg, connection->seg, OFFSET, OFFSET, buffer_size, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIEnqueueDMATransfer failed - Error code 0x%x\n",error);
        return false;
    }

    SCIPostDMAQueue(dma_queue, NO_CALLBACK, NULL, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIPostDMAQueue failed - Error code 0x%x\n",error);
        return false;
    }
#endif

    dma_queue_state = SCIWaitForDMAQueue(dma_queue, SCI_INFINITE_TIMEOUT, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIWaitForDMAQueue failed - Error code 0x%x\n",error);
        return false;
    }

#ifndef NEW_SISCI_DMA_INTERFACE
    SCIResetDMAQueue(dma_queue, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIResetDMAQueue failed - Error code 0x%x\n",error);
        return false;
    }
#endif

    SCITriggerInterrupt(connection->intr, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCITriggerInterrup failed - Error code 0x%x\n",error);
        return false;
    }

    fprintf(stderr, "Waiting for interrupt… ");

    SCIWaitForInterrupt(intr, SCI_INFINITE_TIMEOUT, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        printf("\n");
        fprintf(stderr,"SCIWaitForInterrupt failed - Error code 0x%x\n",error);
        return false;
    }

    fprintf(stderr, "done!\n");

    return true;
}
