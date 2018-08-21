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
 *   	This file contains the header of function declarations and variable
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
 *      Gary Xiao		, garyh0205@hotmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#ifndef pkt_Queue_H
#define pkt_Queue_H

#define Gateway   "0000000000000000"
#define Broadcast "000000000000FFFF"

#define MAX_PKT_LENGTH 1024

enum {UNKNOWN, Data, Local_AT};

enum{ pkt_Queue_SUCCESS = 0, pkt_Queue_FULL = -1, queue_len_error = -2};

/* packet format */
typedef struct pkt {

    //"Data" type
    int type;

    // Brocast:     000000000000FFFF;
    // Coordinator: 0000000000000000
    unsigned char address[8];

    // Data
    char *content;

} sPkt;

typedef sPkt* pPkt;

typedef struct pkt_header {

    // front store the location of the first of thr Pkt Queue
    // rear  store the location of the end of the Pkt Queue
    int front;

    int rear;

    sPkt Queue[MAX_PKT_LENGTH];

    unsigned char address[8];

    pthread_mutex_t mutex;

} spkt_ptr;

typedef spkt_ptr * pkt_ptr;

/* init_Packet_Queue
 *
 *      Initialize Queue for packets
 *
 * Parameter:
 *
 *      pkt_queue : A struct stored pointers of the first and the last of packet.
 *
 * Return Value:
 *
 *      int: If return 0, everything work successful.
 *           If not 0, Something wrong during init mutex.
 *
 */
int init_Packet_Queue(pkt_ptr pkt_queue);

/*
 * Free_Packet_Queue
 *     Release all the packets in the packet queue, the header and
 *     the tail of the packet queue and release the struct stored the pointer of
 *     the packet queue.
 * Parameter:
 *     pkt_queue : A struct stored the first and the last of the packet queue.
 * Return Value:
 *      int: If return 0, everything work successful.
 *           If not 0, Something wrong during delpkt or destroy mutex.
 */
int Free_Packet_Queue(pkt_ptr pkt_queue);

/*
 * addpkt
 *
 *     Add new packet into the packet queue we assigned.
 *
 * Parameter:
 *
 *     pkt_Queue: The Queue we store pkt.
 *     type      : Record the type of packets working environment.
 *     raw_addr  : The destnation address of the packet.
 *     content   : The content we decided to send.
 *
 * Return Value:
 *
 *     int: If return 0, everything work successfully.
 *          If return pkt_Queue_FULL, the pkt is FULL.
 *          If not 0, Somthing Wrong.
 *
 */
int addpkt(pkt_ptr pkt_queue, int type, char *raw_addr, char *content);

/*
 * delpkt
 *
 *     delete the first of the packet queue we assigned.
 *
 * Parameter:
 *
 *     pkt_Queue: The Queue we store pkt.
 *
 * Return Value:
 *
 *     int: If return 0, everything work well.
 *
 */
int delpkt(pkt_ptr pkt_queue);

/*
 * type_to_str
 *
 *     TO convert type to it's original type name.
 *
 * Parameter:
 *
 *     type: A variable stored packet needed send type.
 *
 * Return Value:
 *
 *     Return a char pointer which is it's type name.
 *
 */
char* type_to_str(int type);

/*
 * str_to_type
 *
 *     TO convert type name to type num.
 *
 * Parameter:
 *
 *     conType: A string to tell the connection type.
 *
 * Return Value:
 *
 *     Return a int which is it's type num.
 *
 */
int str_to_type(const char* conType);

/*
 * print_address
 *
 *     Convert hex type address to char type address.
 *
 * Parameter:
 *
 *     address: A address stored in Hex.
 *
 * Return Value:
 *
 *     char_addr: A address stored in char convert from address.
 *
 */
char* print_address(unsigned char* address);

/* display_pkt
 *
 *     display the packet we decide to see.
 *
 * Parameter:
 *
 *     content: The title we want to show in front of the packet content.
 *     pkt: The packet we want to see it's content.
 *     pkt_num: chose whitch pkts we want to display.
 *
 * Return Value:
 *
 *     None
 *
 */
void display_pkt(char* content, pkt_ptr pkt_queue, int pkt_num);

/* get_pkt
 *
 *     get the first of the pkt_queue.
 *
 * Parameter:
 *
 *     pkt_Queue: The Queue we store pkt.
 *
 * Return Value:
 *
 *     pPkt: the pkt address.
 *
 */
pPkt get_pkt(pkt_ptr pkt_queue);

/*
 * Fill_address
 *
 *     Convert the address from raw(char) to addr(Hex).
 *
 * Parameter:
 *
 *     raw: The original char type address.
 *     addr: The destnation variable to store the converted result.
 *
 * Return Value:
 *
 *     None
 *
 */
void Fill_Address(char *raw, unsigned char* addr);

/*
 * address_compare
 *
 *      Compare the address whether is the same.
 *
 * Parameter:
 *
 *      addr1: the address we want to compare.
 *      addr2: the address we want to compare.
 *
 * Return Value:
 *
 *      bool: if true, the same.
 *
 */
bool address_compare(unsigned char* addr1,unsigned char* addr2);

/*
 * address_copy
 *
 *      Compare the address whether is the same.
 *
 * Parameter:
 *
 *      addr1: the src address we copy from.
 *      addr2: the dest address we copy to.
 *
 * Return Value:
 *
 *      None
 *
 */
void address_copy(unsigned char* src_addr, unsigned char* dest_addr);

/*
 * is_null
 *
 *      check if pkt_Queue is null.
 *
 * Parameter:
 *
 *      pkt_Queue: the pkt we stored in the Queue.
 *
 * Return Value:
 *
 *      bool: true if null.
 *
 */
bool is_null(pkt_ptr pkt_Queue);

/*
 * is_null
 *
 *      check if pkt_Queue is full.
 *
 * Parameter:
 *
 *      pkt_Queue: the pkt we stored in the Queue.
 *
 * Return Value:
 *
 *      bool: true if full.
 *
 */
bool is_full(pkt_ptr pkt_Queue);

/*
 * queue-len
 *
 *      Count the length of the queue.
 *
 * Parameter:
 *
 *      pkt_Queue: The queue we want to count.
 *
 * Return Value:
 *
 *      int: the length of the Queue.
 *
 */
int queue_len(pkt_ptr pkt_queue);

#endif
