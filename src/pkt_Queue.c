/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Name:

     pkt_Queue.c

  File Description:

     This file contains functions for the waiting queue.

  Version:

     2.0, 20190201

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

#include "pkt_Queue.h"


/* Queue initialize and free */


int init_Packet_Queue(pkt_ptr pkt_queue){

    pthread_mutex_init( &pkt_queue -> mutex, 0);

    pthread_mutex_lock( &pkt_queue -> mutex);

    pkt_queue -> is_free = false;

    pkt_queue -> front = -1;

    pkt_queue -> rear  = -1;

    for(int num = 0 ; num < MAX_QUEUE_LENGTH ; num ++)
        pkt_queue -> Queue[num].type = NONE;

    pthread_mutex_unlock( &pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;

}


int Free_Packet_Queue(pkt_ptr pkt_queue){

    pthread_mutex_lock( &pkt_queue -> mutex);

    pkt_queue -> is_free = true;

    while ( !(is_null(pkt_queue)))
        delpkt(pkt_queue);

    pthread_mutex_unlock( &pkt_queue -> mutex);

    pthread_mutex_destroy( &pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;

}


/* New : add pkts */


int addpkt(pkt_ptr pkt_queue, unsigned int type, char *raw_addr, char *content
                                                           , int content_size) {

    if(content_size > MESSAGE_LENGTH)
        return MESSAGE_OVERSIZE;

    pthread_mutex_lock( &pkt_queue -> mutex);

    if(pkt_queue -> is_free == true){
        pthread_mutex_unlock( &pkt_queue -> mutex);
        return pkt_Queue_is_free;
    }

#ifdef debugging
    printf("--------- Content ---------\n");
    printf("type               : %s\n", type_to_str(type));

    printf("address            : ");

    print_content(raw_addr, NETWORK_ADDR_LENGTH);

    printf("\n");
    printf("--------- content ---------\n");

    print_content(content, content_size);

    printf("\n");
    printf("---------------------------\n");
#endif

    if(is_full(pkt_queue)){
        pthread_mutex_unlock( &pkt_queue -> mutex);
        return pkt_Queue_FULL;
    }
    else if(is_null(pkt_queue)){
        pkt_queue -> front = 0;
        pkt_queue -> rear  = 0;
    }
    else if( pkt_queue -> rear == MAX_QUEUE_LENGTH - 1){
        pkt_queue -> rear = 0;
    }
    else{
        pkt_queue -> rear ++;
    }

    int current_idx = pkt_queue -> rear;

    pkt_queue -> Queue[current_idx].type = type;

    char_to_hex(raw_addr, pkt_queue -> Queue[current_idx].address,
                NETWORK_ADDR_LENGTH);

    memset(pkt_queue -> Queue[current_idx].content, 0
         , MESSAGE_LENGTH * sizeof(char));

    strncpy(pkt_queue -> Queue[current_idx].content, content
          , content_size);

    pkt_queue -> Queue[current_idx].content_size = content_size;

#ifdef debugging
    display_pkt("addedpkt", pkt_queue, current_idx);

    printf("= pkt_queue len  =\n");

    printf("%d\n", queue_len(pkt_queue));

    printf("==================\n");
#endif

    pthread_mutex_unlock( &pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;

}


sPkt get_pkt(pkt_ptr pkt_queue){

    pthread_mutex_lock( &pkt_queue -> mutex);

    sPkt tmp;

    memset(&tmp, 0, sizeof(tmp));

    if(is_null(pkt_queue)){
        pthread_mutex_unlock( &pkt_queue -> mutex);
        tmp.type = NONE;
        return tmp;
    }

#ifdef debugging
    display_pkt("Get_pkt", pkt_queue, pkt_queue -> front);
#endif

    tmp = pkt_queue -> Queue[pkt_queue -> front];

    delpkt(pkt_queue);

    pthread_mutex_unlock( &pkt_queue -> mutex);

    return tmp;
}


/* Delete : delete pkts */


int delpkt(pkt_ptr pkt_queue) {

    if(is_null(pkt_queue)) {
        return pkt_Queue_SUCCESS;
    }

    int current_idx = pkt_queue -> front;

#ifdef debugging
    display_pkt("deledpkt", pkt_queue, current_idx);
#endif

    memset(pkt_queue -> Queue[current_idx].content, 0
         , MESSAGE_LENGTH * sizeof(char));

    pkt_queue -> Queue[current_idx].type = NONE;

    if(current_idx == pkt_queue -> rear){

        pkt_queue -> front = -1;

        pkt_queue -> rear  = -1;

    }
    else if(current_idx == MAX_QUEUE_LENGTH - 1)
        pkt_queue -> front = 0;
    else
        pkt_queue -> front += 1;

#ifdef debugging
    printf("= pkt_queue len  =\n");

    printf("%d\n", queue_len(pkt_queue));

    printf("==================\n");
#endif

    return pkt_Queue_SUCCESS;

}


int display_pkt(char *display_title, pkt_ptr pkt_queue, int pkt_num){
    char char_addr[NETWORK_ADDR_LENGTH];
    char address_char[NETWORK_ADDR_LENGTH];

    if(pkt_num < 0 && pkt_num >= MAX_QUEUE_LENGTH){
        return pkt_Queue_display_over_range;
    }

    pPkt current_pkt = &pkt_queue -> Queue[pkt_num];

    memset(char_addr, 0, sizeof(char_addr));
    hex_to_char(current_pkt -> address, NETWORK_ADDR_LENGTH_HEX, char_addr);

    printf("==================\n");

    printf("%s\n", display_title);

    printf("==================\n");

    printf("====== type ======\n");

    printf("%s\n", type_to_str(current_pkt -> type));

    printf("===== address ====\n");

    memset(address_char, 0, sizeof(address_char));
    hex_to_char(current_pkt -> address, NETWORK_ADDR_LENGTH_HEX, address_char);

    print_content(address_char, NETWORK_ADDR_LENGTH);

    printf("\n");

    printf("==== content =====\n");

    print_content(current_pkt -> content, current_pkt -> content_size);

    printf("\n");
    printf("==================\n");

    return pkt_Queue_SUCCESS;

}


/* Tools */


char *type_to_str(int type){

    switch(type){

        case Data:
            return "Data";
            break;

        case Local_AT:
            return "Local AT";
            break;

        case UDP:
            return "UDP";
            break;

        default:
            return "UNKNOWN";
    }
}


int str_to_type(const char *conType){

    if(memcmp(conType, "Transmit Status"
     , strlen("Transmit Status") * sizeof(char)) == 0)
        return Data;
    else if(memcmp(conType, "Data", strlen("Data") * sizeof(char)) == 0)
        return Data;
    else
        return UNKNOWN;

}


void char_to_hex(char *raw, unsigned char *raw_hex, int size){

    for(int i = 0 ; i < (size/2) ; i ++){

        char tmp[2];

        tmp[0] = raw[i * 2];
        tmp[1] = raw[i * 2 + 1];
        raw_hex[i] = strtol(tmp,(void *) NULL, 16);
    }
}


int hex_to_char(unsigned char *hex, int size, char *buf){
    int ret = 0;
    int len;

    for(int len = 0;len < size;len ++)
        sprintf( &buf[len * 2], "%02x", hex[len]);

    return ret;
}


void array_copy(unsigned char *src, unsigned char *dest, int size){

    memcpy(dest, src, size);

    return;

}


bool address_compare(unsigned char *addr1,unsigned char *addr2){

    if (memcmp(addr1, addr2, 8) == 0)
        return true;

    return false;
}


bool is_null(pkt_ptr pkt_queue){

    if (pkt_queue->front == -1 && pkt_queue->rear == -1){

        return true;
    }
    return false;
}


bool is_full(pkt_ptr pkt_queue){

    if(pkt_queue -> front == pkt_queue -> rear + 1){
        return true;
    }
    else if(pkt_queue -> front == 0 && pkt_queue -> rear == MAX_QUEUE_LENGTH - 1){
        return true;
    }
    else{
        return false;
    }
}


int queue_len(pkt_ptr pkt_queue){

    if (pkt_queue -> front == 0 && pkt_queue -> rear == 0){
        return 1;

    }
    else if(pkt_queue -> front == -1 && pkt_queue -> rear == -1){
        return 0;

    }
    else if (pkt_queue -> front == pkt_queue -> rear){
        return 1;
    }
    else if (pkt_queue -> rear > pkt_queue -> front){

        int len = (pkt_queue -> rear - pkt_queue -> front + 1);
        return len;
    }
    else if (pkt_queue -> front > pkt_queue -> rear){

        int len = ((MAX_QUEUE_LENGTH - pkt_queue -> front)
                    + pkt_queue -> rear + 1);
        return len;
    }
    else{
        return queue_len_error;
    }
    return queue_len_error;
}


void print_content(char *content, int size){

    for(int loc = 0; loc < size; loc ++)
        printf("%c", content[loc]);
}


void generate_identification(char *identification, int size){

    char str[] = "0123456789ABCDEF";

    memset(identification, 0 , size * sizeof(char));

    struct timeval tv;

    struct timezone tz;

    gettimeofday (&tv , &tz);

    /* Seed number for rand() */
    srand((unsigned int) (tv.tv_sec * 1000 + tv.tv_usec));

    for(int length = 0;length < size;length ++) {

        identification[length] = str[rand() % 16];
        srand(rand());
    }
}
