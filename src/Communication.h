#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xbee_API.h"

/* Struct of parameters for Zigbee Initialization */
typedef struct Zigbee {

    /* Struct of xbee main part which is defined in "libxbee" library */
    struct xbee *xbee; 

    /* Struct of xbee connector which is defined in "libxbee" library */
    struct xbee_con *con;

    /* Struct of queue of packet which is defined in pkt_Queue.h */
    pkt_ptr pkt_Queue;
    
} Zigbee;


Zigbee zigbee;

int zigbee_init(Zigbee zigbee);
int zigbee_connection(Zigbee zigbee, char *message);
void zigbee_free(Zigbee zigbee);