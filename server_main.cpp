#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <getopt.h>

#include "producer.hpp"
#include "consumer.hpp"
#include "controller.hpp"
#include "dolphin-server.hpp"

#define NO_FLAGS      0

#define ADAPTER_NUM   1

#define BUFSIZE       9000000

DolphinServer *s;

void shutdown(int i) {
    s->shutdown();
    delete s;

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    int remote_node = -1;
    char *output = NULL;

    int opt = 0;
    static struct option long_options[] = {
        {"output", required_argument, 0,  'o' },
        {"remote", required_argument, 0,  'r' },
        {0,        0,                 0,   0  }
    };

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"i:r:",
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'o' : output = optarg;
                 break;
             case 'r' : remote_node = atoi(optarg);
                 break;
             default  : fprintf(stderr, "Error parsing arguments!\n");
                        exit(EXIT_FAILURE);
        }
    }

    if (remote_node == -1) {
        fprintf(stderr, "You must specify a remote node!\n");
        return EXIT_FAILURE;
    }

    if (output == NULL) {
        fprintf(stderr, "You must specify an output file!\n");
        return EXIT_FAILURE;
    }

    sci_error_t error;
    uint32_t node_id;

    /*
     * Handle interrupts
     */
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = shutdown;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);

    // Open the file
    FILE *out = fopen(output, "w");

    // Check that the file is indeed open
    if (out == NULL) {
        fprintf(stderr, "Output is NULL\n");
        return EXIT_FAILURE;
    }

    // TODO Remove
    fclose(out);

    /*
     * Initialize dolphin
     */
    // Initialize the dolphin library
    SCIInitialize(NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"SCIInitialize failed - Error code 0x%x\n",error);
        return EXIT_FAILURE;
    }

    /* Get local nodeId */
    SCIGetLocalNodeId(ADAPTER_NUM, &node_id, NO_FLAGS, &error);
    if (error != SCI_ERR_OK) {
        fprintf(stderr,"Could not find the local adapter %d\n", ADAPTER_NUM);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Node_id: %d\n", node_id);

    /*
     * Run dolphin
     */
    s = new DolphinServer(node_id, remote_node, BUFSIZE);
    s->setup();
    s->connect();
    s->run();

    delete s;

    return EXIT_FAILURE;
}
