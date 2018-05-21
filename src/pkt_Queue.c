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
 *   	This file contains the program for the waiting queue.
 *
 * File Name:
 *
 *      pkt_Queue.c
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

#include "pkt_Queue.h"

/* Initialize Queue                                                          */
void init_Packet_Queue(pkt_ptr pkt_queue) {
    pkt_queue->locker = Lock_Queue;
    pkt_queue->len    = 0;
    pkt_queue->front = malloc(sizeof(sPkt));
    pkt_queue->rear  = pkt_queue->front;
    pkt_queue->front->next = NULL;
    pkt_queue->locker = unLock_Queue;
}

void Free_Packet_Queue(pkt_ptr pkt_queue){

    delallpkt(pkt_queue);

    Locker status;
    do{
        status = pkt_queue->locker;
        pkt_queue->locker = Lock_Queue;
    }while(status != unLock_Queue);

    free(pkt_queue->front);
    free(pkt_queue);
}

/* A function for create new packet in queue                                 */
void addpkt(pkt_ptr pkt_queue, int type, char *raw_addr, char *content ) {
    Locker status;
    do{
        status = pkt_queue->locker;
        pkt_queue->locker = Lock_Queue;
    }while(status != unLock_Queue);


    printf("addpkt start\n");
    pPkt newpkt = malloc(sizeof(sPkt));

    printf("------Content------\n");
    printf("type    : %s\n", type_to_str(type));
    printf("address : %s\n", raw_addr);
    printf("content : %s\n", content);
    printf("-------------------\n");

    printf("determine queue is null or not\n");
    if(pkt_queue->len == 0) {
        printf("queue is null\n");
        (pkt_queue->front)->next = newpkt;
    }

    newpkt -> type = type;

    Fill_Address(raw_addr, newpkt->address);

    int cont_size = sizeof(content);
    newpkt->content = malloc(cont_size);

    strncpy(newpkt -> content, content, cont_size);
    printf("Set next NULL\n");
    newpkt->next = NULL;
    printf("Add to Queue\n");
    (pkt_queue->rear)->next = newpkt;
    printf("Add to Queue\n");
    pkt_queue->rear = newpkt;

    display_pkt("Addedpkt", pkt_queue->rear);
    pkt_queue->len+= 1;
    pkt_queue->locker = unLock_Queue;
    return;
}

/* A function for delete a sended Packet in queue                            */
void delpkt(pkt_ptr pkt_queue) {
    Locker status;
    do{
        status = pkt_queue->locker;
        pkt_queue->locker = Lock_Queue;
    }while(status != unLock_Queue);

    if(pkt_queue->len == 0) {
        printf("Packet Queue is empty!\n");
        pkt_queue->locker = unLock_Queue;
        return;
    }

    sPkt* tmpnode;
    tmpnode = (pkt_queue->front)->next;
    (pkt_queue->front)->next = tmpnode->next;
    display_pkt("deledpkt",tmpnode);
    free(tmpnode->content);
    free(tmpnode);
    pkt_queue->len-= 1;
    pkt_queue->locker = unLock_Queue;
    return;
}

void delallpkt(pkt_ptr pkt_queue) {
    while (pkt_queue->len != 0){
        delpkt(pkt_queue);
        printf("delall\n");
    }
    printf("End delall\n");
    return;
}

char* print_address(unsigned char* address){
    char* char_addr = malloc(sizeof(char)*17);
    memset(char_addr, 0, sizeof(char)*17);
    sprintf(char_addr, "%02x%02x%02x%02x%02x%02x%02x%02x", address[0]
    , address[1], address[2], address[3], address[4], address[5], address[6]
    , address[7]);
    return char_addr;
}

char* type_to_str(int type){
    switch(type){
        case Data:
            return "Data";
            break;
        case Local_AT:
            return "Local AT";
            break;
        default:
            return "UNKNOWN";
    }
}

/* Fill the address from raw(char) to addr(Hex)                              */
void Fill_Address(char *raw,unsigned char* addr){
    for(int i = 0;i < 8;i++){
        char tmp[2];
        tmp[0] = raw[i*2];
        tmp[1] = raw[i*2+1];
        addr[i] = strtol(tmp,(void*) NULL, 16);
        printf("%2x",addr[i]);
    }
    printf("\n");
}

void display_pkt(char* content, pPkt pkt){
    char* char_addr = print_address(pkt->address);
    printf("------ %12s ------\n",content);
    printf("type    : %s\n", type_to_str(pkt->type));
    printf("address : %s\n", char_addr);
    printf("content : %s\n", pkt->content);
    printf("--------------------------\n");
    free(char_addr);
}
