/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIPS

  File Description:

     This file contains the header of  function declarations and variable used
     in UDP_API.c

  File Name:

     UDP_API.h

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
     Gary Xiao		, garyh0205@hotmail.com
 */

#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //exit(0)
#include <stdbool.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "pkt_Queue.h"

#ifndef UDP_API_H
#define UDP_API_H

#define UDP_LISTEN_PORT 8888    //The port on which to listen for incoming data

#define UDP_SELECT_TIMEOUT 5    //second

typedef struct udp_config{

    struct sockaddr_in si_server;

    int  send_socket, recv_socket;

    char Local_Address[NETWORK_ADDR_LENGTH];

    spkt_ptr pkt_Queue, Received_Queue;

    bool shutdown;

    pthread_t udp_send, udp_receive;

} sudp_config;

typedef sudp_config *pudp_config;

enum{File_OPEN_ERROR = -1, E_ADDPKT_OVERSIZE = -2};

/*
  udp_initial

     For initialize UDP Socketm including it's buffer.

  Parameter:

     udp_config: A structure contain all variables for UDP.

  Return Value:

     int : If return 0, everything work successfully.
           If not 0   , somthing wrong.
 */
int udp_initial(pudp_config udp_config);

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
int udp_addpkt(pkt_ptr pkt_queue, char *raw_addr, char *content, int size);

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
  udp_hex_to_address

     Convert address from hex format to char.

  Parameter:

     hex_addr: A array pointer point to the address we want to convert.

  Return Value:

     char : return char array of the converted address.
 */
char *udp_hex_to_address(unsigned char *hex_addr);

#endif
