#include "iso14229.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SERVER_PHYS_ADDR   0x7E0  // server's physical (1:1) receive address. (0x7E0–0x7E7: physical request IDs, one range slot per ECU (tester → ECU #0 through ECU #7))
#define CLIENT_PHYS_ADDR   0x7E8  // this client's physical (1:1) receive address. (0x7E8–0x7EF: physical response IDs, matching 1:1 with the request range (ECU #0 responds on 0x7E8, ECU #1 on 0x7E9, etc. — always request_id + 8)))
#define FUNCTIONAL_ADDR    0x7DF  // broadcast/functional address (all servers listen here). (0x7DF: the functional (broadcast) request ID)

#define DID_TEST_SCRATCH   0xDEAD // custom scratch DID: readable/writable test value
#define DID_VIN            0xF190 // standard UDS DID: Vehicle Identification Number

static UDSClient_t client;
static UDSTpIsoTpSock_t tp;
static bool done = false;

static int fn(UDSClient_t *client, UDSEvent_t ev, void *arg) {
    switch (ev) {
        case UDS_EVT_ResponseReceived:
            printf("Response received (%u bytes): ", client->recv_size);
            for (int i = 0; i < client->recv_size; i++) {
                printf("%02X ", client->recv_buf[i]);
            }
            printf("\n");
            done = true;
            break;

        case UDS_EVT_Err: {
            UDSErr_t *err = (UDSErr_t *)arg;
            printf("Error: %s\n", UDSErrToStr(*err));
            done = true;
            break;
        }

        default:
            break;
    }
    return 0;
}

int main(int ac, char **av) {
    if (ac != 2) {
        fprintf(stderr, "usage: %s <can interface>\n", av[0]);
        exit(-1);
    }

    if (UDSTpIsoTpSockInitClient(&tp, av[1], CLIENT_PHYS_ADDR, SERVER_PHYS_ADDR, FUNCTIONAL_ADDR)) {
        fprintf(stderr, "UDSTpIsoTpSockInitClient failed\n");
        exit(-1);
    }

    if (UDSClientInit(&client)) {
        fprintf(stderr, "UDSClientInit failed\n");
        exit(-1);
    }

    client.tp = (UDSTp_t *)&tp;
    client.fn = fn;

    uint16_t did = DID_TEST_SCRATCH;
    printf("Sending RDBI request for DID 0x%04X...\n", did);
    UDSSendRDBI(&client, &did, 1);

    while (!done) {
        UDSClientPoll(&client);
    }

    printf("Client exiting\n");
    return 0;
}

    