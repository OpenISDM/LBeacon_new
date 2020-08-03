/*
  Copyright (c) BiDaE Technology Inc. All rights reserved.

  License:

    BiDaE SHAREWARE LICENSE
    Version 1.0, 31 July 2020

    Copyright (c) BiDaE Technology Inc. All rights reserved.
    The SOFTWARE implemented in the product is copyrighted and protected by 
    all applicable intellectual property laws worldwide. Any duplication of 
    the SOFTWARE other than for archiving or resetting purposes on the same 
    product without the written agreement from BiDaE Technology could be a 
    violation of law. For the avoidance of doubt, redistribution of this 
    SOFTWARE in source or binary form is prohibited. You agree to prevent 
    any unauthorized copying and redistribution of the SOFTWARE. 

    BiDaE Technology Inc. is the license steward. No one other than the 
    license steward has the right to modify or publish new versions of this 
    License. However, You may distribute the SOFTWARE under the terms of the 
    version of the License under which You originally received the Covered 
    Software, or under the terms of any subsequent version published by the 
    license steward.

    LIMITED WARRANTY:

    BiDaE Technology Inc. or its distributors, depending on which party sold 
    the SOFTWARE, warrants that the media on which the SOFTWARE is installed 
    will be free from defects in materials under normal and purposed use.

    BiDaE Technology Inc. or its distributor warrants, for your benefit alone, 
    that during the Warranty Period the SOFTWARE, shall operate substantially 
    in accordance with the functional specifications in the User's Manual. If, 
    during the Warranty Period, a defect in the SOFTWARE appears, You may 
    obtain a replacement of the SOFTWARE. Any replacement SOFTWARE will be 
    warranted for the remainder of the Warranty Period attached to the product.

    WHEN THE WARRANTY PERIOD HAS BEEN EXPIRED, THIS SOFTWARE IS PROVIDED 
    ''AS IS,'' AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. HENCEFORTH, IN NO EVENT SHALL BiDaE 
    TECHNOLOGY INC. OR ITS COLLABORATOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

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