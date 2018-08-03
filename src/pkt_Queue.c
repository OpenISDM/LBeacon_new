
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
 *      This file contains the program for the waiting queue.
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
 *      Gary Xiao       , garyh0205@hotmail.com
 */

#include "pkt_Queue.h"

void init_Packet_Queue(pkt_ptr pkt_queue){

    pkt_queue->locker = false;

    bool status;
    do{
        status = pkt_queue -> locker;
        pkt_queue -> locker = true;
    }while(status == false);

    pkt_queue -> front.next = pkt_queue -> rear.next = NULL;

    pkt_queue -> locker = false;

}

void Free_Packet_Queue(pkt_ptr pkt_queue){
    while (!(is_null(pkt_queue))){
        delpkt(pkt_queue);
    }
    printf("pkt_queue released\n");
}

void addpkt(pkt_ptr pkt_queue, int type, char *raw_addr, char *content ) {
    bool status;
    do{
        status = pkt_queue->locker;
        pkt_queue->locker = true;
    }while(status == true);

    printf("addpkt start\n");

    pPkt newpkt = malloc(sizeof(sPkt));

    printf("------Content------\n");
    printf("type    : %s\n", type_to_str(type));
    printf("address : %s\n", raw_addr);
    printf("content : %s\n", content);
    printf("-------------------\n");

    if(is_null(pkt_queue)) {
        printf("queue is null\n");
        pkt_queue->front.next = newpkt;
        pkt_queue->rear.next = newpkt;
    }
    newpkt->type = type;
    Fill_Address(raw_addr, newpkt->address);
    int cont_len = strlen(content);
    newpkt->content = malloc((cont_len+1) * sizeof(char));
    memset(newpkt->content, 0, sizeof((cont_len + 1)*sizeof(char)));
    strncpy(newpkt->content, content, cont_len);
    newpkt->content[cont_len] = '\0';
    newpkt->next = NULL;
    if(pkt_queue->rear.next != NULL)
        pkt_queue->rear.next->next = newpkt;
    pkt_queue->rear.next = newpkt;

    display_pkt("Addedpkt", newpkt);
    pkt_queue->locker = false;

    return;
}

 void delpkt(pkt_ptr pkt_queue) {
    bool status;
    do{
        status = pkt_queue->locker;
        pkt_queue->locker = true;
    }while(status == true);

    if(is_null(pkt_queue)) {
        printf("Packet Queue is empty!\n");
        pkt_queue->locker = false;
        return;
    }

    sPkt tmpnode;
    tmpnode.next = pkt_queue->front.next;
    if(pkt_queue->front.next == pkt_queue->rear.next){
        pkt_queue->front.next = NULL;
        pkt_queue->rear.next = NULL;
    }
    else{
        pkt_queue->front.next = pkt_queue->front.next->next;
    }

    display_pkt("deledpkt", tmpnode.next);
    free(tmpnode.next->content);
    tmpnode.next->next = NULL;
    free(tmpnode.next);
    pkt_queue->locker = false;

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

bool address_compare(unsigned char* addr1,unsigned char* addr2){
    if (memcmp(addr1, addr2, 8) == 0){
        return true;
    }
    return false;
}

void address_copy(unsigned char* src_addr, unsigned char* dest_addr){
    memcpy(dest_addr, src_addr, 8);
}

void display_pkt(char* content, pPkt pkt){
    if(pkt == NULL)
        return;
    char* char_addr = print_address(pkt->address);
    printf("------ %12s ------\n",content);
    printf("type    : %s\n", type_to_str(pkt->type));
    printf("address : %s\n", char_addr);
    printf("content : %s\n", pkt->content);
    printf("--------------------------\n");
    free(char_addr);
    return;
}

bool is_null(pkt_ptr pkt_queue){
    if (pkt_queue->front.next == NULL && pkt_queue->rear.next == NULL){
        return true;
    }
    return false;
}

int queue_len(pkt_ptr pkt_queue){
    int count = 0 ;
    if (is_null(pkt_queue)){
        printf("Queue is NULL\n");
        return 0;
    }
    pPkt tmpnode;
    tmpnode = pkt_queue->front.next;
    while(tmpnode != pkt_queue->rear.next){
        tmpnode = tmpnode->next;
        count ++;
    }
    count ++;
    return count;
}