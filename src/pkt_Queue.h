/*
 * Copyright (c) 2016 Academia Sinica, Institute of Information Science
 *
 * License:
 *
 *      GPL 3.0 : The content of this file is subject to the terms and
 *      cnditions defined in file 'COPYING.txt', which is part of this
 *      source code package.
 *
 * Project Name:
 *
 *      BeDIPS
 *
 * File Description:
 *
 *      This file contains the header of function declarations and variable
 *      used in pkt_Queue.h
 *
 * File Name:
 *
 *      pkt_Queue.h
 *
 * Abstract:
 *
 *      BeDIPS uses LBeacons to deliver 3D coordinates and textual
 *      descriptions of their locations to users' devices. Basically, a
 *      LBeacon is an inexpensive, Bluetooth Smart Ready device. The 3D
 *      coordinates and location description of every LBeacon are retrieved
 *      from BeDIS (Building/environment Data and Information System) and
 *      stored locally during deployment and maintenance times. Once
 *      initialized, each LBeacon broadcasts its coordinates and location
 *      description to Bluetooth enabled user devices within its coverage
 *      area.
 *
 * Authors:
 *      Gary Xiao       , garyh0205@hotmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define Gateway   "0000000000000000"
#define Broadcast "000000000000FFFF"

enum {Data, Local_AT};

/* packet format in the Queue */
struct pkt {

    //"Data"
    int type;

    // Brocast:     000000000000FFFF;
    // Coordinator: 0000000000000000
    unsigned char address[8];

    // Data
    char *content;

  struct pkt *next;
};

typedef struct pkt sPkt;
typedef sPkt* pPkt;

typedef enum {Lock_Queue,unLock_Queue} Locker;

struct pkt_header {

    // front point to the first of thr Pkt Queue
    // rear  point to the end of the Pkt Queue
    pPkt front;
    pPkt rear;
    Locker locker;
    int len;
};

typedef struct pkt_header spkt_ptr;
typedef spkt_ptr* pkt_ptr;

/* Create Packet Queue Header */
void init_Packet_Queue(pkt_ptr pkt_queue);

/* Free the Packet Queue */
void Free_Packet_Queue(pkt_ptr pkt_queue);

/* Add new Packet to the end of Queue */
void addpkt(pkt_ptr pkt_queue, int type, char *raw_addr, char *content);

/* Delete the end of Queue */
void delpkt(pkt_ptr pkt_queue);

/* Delete all the packet in the queue */
void delallpkt(pkt_ptr pkt_queue);

char* type_to_str(int type);

/* Print the address of the zigbee */
char* print_address(unsigned char* address);

/* Display the content of the packet */
void display_pkt(char* content, pPkt pkt);

/* Fill the address from raw(char) to addr(Hex) */
void Fill_Address(char *raw, unsigned char* addr);
