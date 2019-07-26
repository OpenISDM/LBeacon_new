/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     UDP_API.h

  File Description:

     This file contains the header of  function declarations and variable used
     in UDP_API.c

  Abstract:

     BeDIS uses LBeacons to deliver 3D coordinates and textual descriptions of
     their locations to users' devices. Basically, a LBeacon is an inexpensive,
     Bluetooth Smart Ready device. The 3D coordinates and location description
     of every LBeacon are retrieved from BeDIS (Building/environment Data and
     Information System) and stored locally during deployment and maintenance
     times. Once initialized, each LBeacon broadcasts its coordinates and
     location description to Bluetooth enabled user devices within its coverage
     area.

  Authors:

     Gary Xiao      , garyh0205@hotmail.com
 */
#ifndef UDP_API_H
#define UDP_API_H

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)
#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <errno.h>
#include "pkt_Queue.h"


#define UDP_SELECT_TIMEOUT 30    //second
#define SLEEP_TIME_IN_MS 100 //milliseconds

/* If need to debug. */
#define debugging

typedef struct {

    char send_ipv4_addr[NETWORK_ADDR_LENGTH];
    int send_portno;
    spkt_ptr send_pkt_queue;

    struct sockaddr_in si_recv;
    int recv_portno;
    int  recv_socket;
    spkt_ptr recv_pkt_queue;

} sudp_config_beacon;

typedef struct {

    struct sockaddr_in si_server;

    int  send_socket, recv_socket;

    int send_port;

    int recv_port;

    char Local_Address[NETWORK_ADDR_LENGTH];

    pthread_t udp_send, udp_receive;

    bool shutdown;

    spkt_ptr pkt_Queue, Received_Queue;

} sudp_config;

typedef sudp_config *pudp_config;

enum{File_OPEN_ERROR = -1, E_ADDPKT_OVERSIZE = -2};

enum{socket_error = -1, send_socket_error = -2, recv_socket_error = -3,
set_socketopt_error = -4,recv_socket_bind_error = -5,addpkt_msg_oversize = -6};


/*
  udp_initial

     For initialize UDP Socketm including it's buffer.

  Parameter:

     udp_config: A structure contain all variables for UDP.

  Return Value:

     int : If return 0, everything work successfully.
           If not 0   , somthing wrong.
 */
int udp_initial(pudp_config udp_config, int send_port, int recv_port);


/*
  udp_addpkt

     A function for add pkt to the assigned pkt_Queue.

  Parameter:

     udp_config : A structure contain all variables for UDP.
     raw_addr   : The destnation address of the packet.
     content    : The content we decided to send.
     size       : size of the content.

  Return Value:

     int : If return 0, everything work successfully.
           If not 0   , something wrong.
 */
int udp_addpkt(pudp_config udp_config, char *raw_addr, char *content, int size);


/*
  udp_getrecv

     A function for get pkt from the assigned pkt_Queue.

  Parameter:

     udp_config : A structure contain all variables for UDP.

  Return Value:

     sPkt : return the first pkt content in Received_Queue.
 */
sPkt udp_getrecv(pudp_config udp_config);


/*
  udp_send_pkt

     A thread for sending pkt to dest address.

  Parameter:

     udpconfig: A structure contain all variables for UDP.

  Return Value:

     None
 */
void *udp_send_pkt(void *udpconfig);


/*
  udp_recv_pkt

     A thread for recv pkt.

  Parameter:

     udpconfig: A structure contain all variables for UDP.

  Return Value:

     None
 */
void *udp_recv_pkt(void *udpconfig);


/*
  udp_release

     Release All pkt and mutex.

  Parameter:

     udp_config: A structure contain all variables for UDP.

  Return Value:

     int : If return 0, everything work successfully.
           If not 0   , something wrong.
 */
int udp_release(pudp_config udp_config);


/*
  udp_address_reduce_point

     Convert address from address to a address remove all points.

  Parameter:

     raw_addr: A array pointer point to the address we want to convert.
     address: the output array pointer to store the network address reduced
              point

  Return Value:

     0: success
 */
int udp_address_reduce_point(char *raw_addr, char *address);


/*
  udp_hex_to_address

     Convert address from hex format to char.

  Parameter:

     hex_addr: A array pointer point to the address we want to convert.
     dest_address: the output array pointer to store the converted address

  Return Value:

     0: success
 */
 int udp_hex_to_address(unsigned char *hex_addr, char *dest_address);


#endif
