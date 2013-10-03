#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <getopt.h>

#include "producer.hpp"
#include "consumer.hpp"
#include "controller.hpp"
#include "dolphin-client.hpp"
#include "FileReader.hpp"

#define NO_FLAGS      0
#define ADAPTER_NUM   0

DolphinClient *c;
FileReader *f;

extern "C" {
    typedef struct {
        uint32_t size;
        void *data;
    } package;
}

void shutdown(int i) {
    c->shutdown();
    delete c;
    delete f;

    // Close all dolphin related resources
    SCITerminate();

    exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
    int remote_node = -1;
    char *input = NULL;

    int opt = 0;
    static struct option long_options[] = {
        {"input",  required_argument, 0,  'i' },
        {"remote", required_argument, 0,  'r' },
        {0,        0,                 0,   0  }
    };

    int long_index =0;
    while ((opt = getopt_long(argc, argv,"i:r:",
                   long_options, &long_index )) != -1) {
        switch (opt) {
             case 'i' : input = optarg;
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

    if (input == NULL) {
        fprintf(stderr, "You must specify an input file!\n");
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

    fprintf(stderr, "Node id %d\n", node_id);

    /*
     * Run dolphin
     */
    c = new DolphinClient(node_id, remote_node, BUFSIZE);
    c->setup();
    c->connect();

    f = new FileReader(input);

    c->run(f);



    delete c;

    return EXIT_SUCCESS;
}
