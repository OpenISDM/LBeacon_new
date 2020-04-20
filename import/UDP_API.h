/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions  
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     UDP_API.h

  Version:

     2.0, 20190608

  File Description:

     This file contains the header of function declarations and variable used
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <errno.h>

#ifdef _MSC_VER
#include <winsock2.h>
#include <windows.h>
#pragma comment(lib,"WS2_32.lib")
#include <WS2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#endif


#include "pkt_Queue.h"


/* The time interval in seconds for Select() break the block */
#define UDP_SELECT_TIMEOUT 60

/* The time in milliseconds for the send thread to sleep when it is idle */
#define SEND_THREAD_IDLE_SLEEP_TIME 50

/* The time in milliseconds for the receive thread to sleep when it is idle */
#define RECEIVE_THREAD_IDLE_SLEEP_TIME 50

#define DELIMITER_SEMICOLON ";"

#define LENGTH_OF_SHA256 512

#define LENGTH_OF_ENCODED_WIFI_MESSAGE 8192

/* When debugging is needed */
//#define debugging

typedef struct {
    
#ifdef _WIN32
    WSADATA wsaData;

    WORD sockVersion;
#endif

    struct sockaddr_in si_server;

    int optval;

    int  send_socket, recv_socket;

    int recv_port;

    pthread_t udp_send_thread, udp_receive_thread;

   /* The flag set to true whwn the process need to stop */
    bool shutdown;

    spkt_ptr pkt_Queue, Received_Queue;

} sudp_config;

typedef sudp_config *pudp_config;


enum{
   socket_error = -1, 
   send_socket_error = -2, 
   recv_socket_error = -3,
   set_socketopt_error = -4,
   recv_socket_bind_error = -5,
   addpkt_msg_oversize = -6
   };


/*
  udp_initial

     For initialize UDP Socket including the send queue and the receive queue.

  Parameter:

     udp_config: The pointer points to the structure contains all variables for 
                 the UDP connection.

  Return Value:

     int : If return 0, everything work successfully.
           If not 0   , somthing wrong.
 */
int udp_initial(pudp_config udp_config, int recv_port);


/*
  udp_addpkt

     This function is used to add the packet to the assigned pkt queue.

  Parameter:

     udp_config : The pointer points to the structure contains all variables 
                  for the UDP connection.
     port       : The port number to be sent to.
     address    : The pointer points to the destnation address of the packet.
     content    : The pointer points to the content we decided to send.
     size       : The size of the content.

  Return Value:

     int : If return 0, everything work successfully.
           If not 0   , something wrong.
 */
int udp_addpkt(pudp_config udp_config, char *address, unsigned int port, 
               char *content, int size);


/*
  udp_getrecv

     This function is used for get received packet from the received queue.

  Parameter:

     udp_config : The pointer points to the  structure contains all variables   
                  for the UDP connection.

  Return Value:

     sPkt : return the first pkt content in the received queue.
 */
sPkt udp_getrecv(pudp_config udp_config);


/*
  udp_send_pkt_routine

     The thread for sending packets to the destination address.

  Parameter:

     udpconfig: The pointer points to the structure contains all variables for 
                the UDP connection.

  Return Value:

     None
 */
void *udp_send_pkt_routine(void *udpconfig);


/*
  udp_recv_pkt_routine

     The thread for receiving packets.

  Parameter:

     udpconfig: The pointer points to the structure contains all variables for  
                the UDP connection.

  Return Value:

     None
 */
void *udp_recv_pkt_routine(void *udpconfig);


/*
  udp_release

     Release All pkts and mutexes.

  Parameter:

     udp_config: The pointer points to the structure contains all variables for 
                 the UDP connection.

  Return Value:

     int : If return 0, everything work successfully.
           If not 0   , something wrong.
 */
int udp_release(pudp_config udp_config);


#endif