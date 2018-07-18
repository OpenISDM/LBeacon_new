#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xbee_API.h"

/* The length of the message to be sent to the gateway */
#define MESSAGE_LENGTH 512

#define XBEE_TIMEOUT 2000000

#define XBEE_SUCCESSFULLY 1
#define XBEE_ERROR 0

/* Struct of parameters for Zigbee Initialization */
typedef struct Zigbee {

    /* Struct of xbee main part which is defined in "libxbee" library */
    struct xbee *xbee; 

    /* Struct of xbee connector which is defined in "libxbee" library */
    struct xbee_con *con;

    /* Struct of queue of packet which is defined in pkt_Queue.h */
    pkt_ptr pkt_Queue;

    char zig_message[MESSAGE_LENGTH];
    
} Zigbee;


int zigbee_init(Zigbee zigbee);
void *zigbee_send_file(Zigbee zigbee);
void zigbee_free(Zigbee zigbee);