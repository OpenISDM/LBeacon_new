/*
  2020 © Copyright (c) BiDaE Technology Inc. 
  Provided under BiDaE SHAREWARE LICENSE-1.0 in the LICENSE.

  Project Name:

     BeDIS

  File Name:

     pkt_Queue.h

  File Description:

     This file contains the header of function declarations used in pkt_Queue.c

  Version:

     2.0, 20190608

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

#ifndef pkt_Queue_H
#define pkt_Queue_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif


/* When debugging is needed */
//#define debugging

/* The size of the IP address in char */
#define NETWORK_ADDR_LENGTH 16

/* Maximum length of message to be sent over Wi-Fi in bytes 
   (The maximum UDP pkt size is 65535 bytes - 8 bytes UDP header - 
    20 bytes IP header = 65507 bytes) 
 */
#define MESSAGE_LENGTH 65507

/* The maximum length of the pkt Queue. */
#define MAX_QUEUE_LENGTH 512

enum{ 
    pkt_Queue_SUCCESS = 0, 
    pkt_Queue_FULL = -1, 
    queue_len_error = -2, 
    pkt_Queue_is_free = -3, 
    pkt_Queue_is_NULL = -4, 
    pkt_Queue_display_over_range = -5, 
    MESSAGE_OVERSIZE = -6
    };


/* packet format */
typedef struct pkt {

    /* If the pkt is not in use, the flag set to true */
    bool is_null;

    /* The IP adddress of the current pkt */
    unsigned char address[NETWORK_ADDR_LENGTH];

    /* The port number of the current pkt */
    unsigned int port;

    /* The content of the current pkt */
    char content[MESSAGE_LENGTH];

    /* The size of the current pkt */
    int  content_size;

} sPkt;

typedef sPkt *pPkt;


typedef struct pkt_header {

    /* front store the location of the first of thr Pkt Queue */
    int front;

    /* rear  store the location of the end of the Pkt Queue */
    int rear;

    /* The array is used to store pkts. */
    sPkt Queue[MAX_QUEUE_LENGTH];

    /* If the pkt queue is initialized, the flag will set to false */
    bool is_free;

    /* The mutex is used to read/write lock before processing the pkt queue */
    pthread_mutex_t mutex;

} spkt_ptr;

typedef spkt_ptr *pkt_ptr;


/* init_Packet_Queue

      Initialize the queue for storing pkts.

  Parameter:

      pkt_queue : The pointer points to the pkt queue.

  Return Value:

      int: If return 0, everything work successful.
           If not 0, Something wrong during init mutex.

 */
int init_Packet_Queue(pkt_ptr pkt_queue);


/*
  Free_Packet_Queue

      Release all the packets in the packet queue, the header and the tail of 
      the packet queue and release the struct stores the pointer of the packet 
      queue.

  Parameter:

      pkt_queue : The pointer points to the struct stores the first and the 
                  last of the packet queue.

  Return Value:

      int: If return 0, everything work successful.
           If not 0, Something wrong during delpkt or destroy mutex.
 */
int Free_Packet_Queue(pkt_ptr pkt_queue);


/*
  addpkt

      Add new packet into the packet queue. This function is only allow for the 
      data length shorter than the MESSAGE_LENGTH.

  Parameter:

      pkt_queue : The pointer points to the pkt queue we prepare to store the 
                  pkt.
      address   : The IP address of the packet.
      port      : The port number of the packet.
      content   : The content of the packet.
      content_size : The size of the content.

  Return Value:

      int: If return 0, everything work successfully.
           If return pkt_Queue_FULL, the pkt is FULL.
           If not 0, Somthing Wrong.

 */
int addpkt(pkt_ptr pkt_queue, char *address, unsigned int port, 
           char *content, int content_size);


/* get_pkt

      Get the first pkt of the pkt queue.

  Parameter:

      pkt_queue : The pointer points to the pkt queue we going to get 
                  a pkt from.

  Return Value:

      sPkt : return the content of the first pkt.

 */
sPkt get_pkt(pkt_ptr pkt_queue);


/*
  delpkt

      Delete the first of the packet queue.

  Parameter:

      pkt_queue: The pointer points to the pkt queue.

  Return Value:

      int: If return 0, work successfully.

 */
int delpkt(pkt_ptr pkt_queue);


/* display_pkt

      Display the packet we decide to see.

  Parameter:

      display_title : The title we want to show in front of the packet content.
      pkt           : The packet we want to see it's content.
      pkt_num       : Choose whitch pkts we want to display.

  Return Value:

      int

 */
int display_pkt(char *display_title, pkt_ptr pkt_queue, int pkt_num);


/*
  is_null

      check if pkt_Queue is null.

  Parameter:

      pkt_queue : The pointer points to the pkt queue.

  Return Value:

      bool : true if null.

 */
bool is_null(pkt_ptr pkt_queue);


/*
  is_null

      check if pkt_Queue is full.

  Parameter:

      pkt_queue : The pointer points to the pkt queue.

  Return Value:

      bool : true if full.

 */
bool is_full(pkt_ptr pkt_queue);


/*
  queue-len

      Count the length of the queue.

  Parameter:

      pkt_queue : The queue we want to count.

  Return Value:

      int : The length of the Queue.

 */
int queue_len(pkt_ptr pkt_queue);


/*

  print_content

      print the content.

  Parameter:

      content : The pointer of content we desire to read.
      size    : The size of the content.

  Return Value:

      NONE

 */
void print_content(char *content, int size);


#endif