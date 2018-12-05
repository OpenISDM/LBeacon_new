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
 *      BeDIS
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
 *      BeDIS uses LBeacons to deliver 3D coordinates and textual
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

int init_Packet_Queue(pkt_ptr pkt_queue){

    int ret;

    pthread_mutex_init(&pkt_queue -> mutex, 0);

    pkt_queue -> front = -1;

    pkt_queue -> rear  = -1;

    return pkt_Queue_SUCCESS;

}

int Free_Packet_Queue(pkt_ptr pkt_queue){

    int ret;

    while (!(is_null(pkt_queue))){

        delpkt(pkt_queue);

    }

    pthread_mutex_destroy(&pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;

}

int addpkt(pkt_ptr pkt_queue, int type, char *raw_addr, char *content ) {

    int ret;

    pthread_mutex_lock(&pkt_queue -> mutex);

    printf("------Content------\n");
    printf("type    : %s\n", type_to_str(type));
    printf("address : %s\n", raw_addr);
    printf("content : %s\n", content);
    printf("-------------------\n");

    if(is_null(pkt_queue)){

        pkt_queue -> front = 0;

        pkt_queue -> rear  = 0;

    }

    else{

        if(is_full(pkt_queue)){

            pthread_mutex_unlock(&pkt_queue -> mutex);

            return pkt_Queue_FULL;

        }

        else{

            if( pkt_queue -> rear == MAX_PKT_LENGTH - 1){

                pkt_queue -> rear = 0;

            }

            else{

                pkt_queue -> rear ++ ;

            }

        }

    }

    pkt_queue -> Queue[pkt_queue -> rear].type = type;

    Fill_Address(raw_addr, pkt_queue -> Queue[pkt_queue -> rear].address);

    int cont_len = strlen(content);

    pkt_queue -> Queue[pkt_queue -> rear].content =
                                          malloc((cont_len + 1) * sizeof(char));

    memset(pkt_queue -> Queue[pkt_queue -> rear].content, 0
         , sizeof((cont_len + 1)*sizeof(char)));

    strncpy(pkt_queue -> Queue[pkt_queue -> rear].content, content, cont_len);

    pkt_queue -> Queue[pkt_queue -> rear].content[cont_len] = '\0';

    display_pkt("Addedpkt", pkt_queue, pkt_queue -> rear);


    char len[10];
    memset(len, 0, 10);
    sprintf(len, "%d\0", queue_len(pkt_queue));

    printf("= pkt_queue len  =\n");
    printf("%d\n", queue_len(pkt_queue));
    printf("==================\n");

    pthread_mutex_unlock(&pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;
}

int delpkt(pkt_ptr pkt_queue) {

    pthread_mutex_lock(&pkt_queue -> mutex);

    if(is_null(pkt_queue)) {

        pthread_mutex_unlock(&pkt_queue -> mutex);

        return pkt_Queue_SUCCESS;

    }

    display_pkt("deledpkt", pkt_queue, pkt_queue -> front);

    free(pkt_queue -> Queue[pkt_queue -> front].content);

    if(pkt_queue -> front == pkt_queue -> rear){

        pkt_queue -> front = -1;

        pkt_queue -> rear  = -1;

    }
    else{

        if(pkt_queue -> front == MAX_PKT_LENGTH - 1){

            pkt_queue -> front = 0;

        }

        else{

            pkt_queue -> front += 1;

        }

    }

    char len[10];

    memset(len, 0, 10);

    sprintf(len, "%d\0", queue_len(pkt_queue));

    printf("= pkt_queue len  =\n");
    printf("%d\n", queue_len(pkt_queue));
    printf("==================\n");

    pthread_mutex_unlock(&pkt_queue -> mutex);

    return pkt_Queue_SUCCESS;
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

int str_to_type(const char* conType){

    if(memcmp(conType, "Transmit Status"
     , strlen("Transmit Status")* sizeof(char)) == 0){

        return Data;

    }

    else if(memcmp(conType, "Data", strlen("Data")* sizeof(char)) == 0){

        return Data;

    }

    else{

        return UNKNOWN;

    }

}

void Fill_Address(char *raw,unsigned char* addr){

    for(int i = 0;i < 8;i++){

        char tmp[2];

        tmp[0] = raw[i*2];

        tmp[1] = raw[i*2+1];

        addr[i] = strtol(tmp,(void*) NULL, 16);

    }

}

bool address_compare(unsigned char* addr1,unsigned char* addr2){

    if (memcmp(addr1, addr2, 8) == 0){

        return true;

    }

    return false;

}

void address_copy(unsigned char* src_addr, unsigned char* dest_addr){

    memcpy(dest_addr, src_addr, 8);

    return;

}

void display_pkt(char* content, pkt_ptr pkt_queue, int pkt_num){

    if(pkt_num == -1)

        return;

    char* char_addr = print_address(pkt_queue -> Queue[pkt_num].address);

    printf("==================\n");
    printf("%s\n", content);
    printf("==================\n");
    printf("== type ==\n");
    printf("%s\n", type_to_str(pkt_queue -> Queue[pkt_num].type));
    printf("== address ==\n");
    printf("%s\n", char_addr);
    printf("== content ==\n");
    printf("%s\n", pkt_queue -> Queue[pkt_num].content);
    printf("==================\n");

    free(char_addr);

    return;
}

pPkt get_pkt(pkt_ptr pkt_queue){

    if(is_null(pkt_queue)){

        return NULL;

    }

    display_pkt("Get_pkt", pkt_queue, pkt_queue -> front);

    return &(pkt_queue -> Queue[pkt_queue -> front]);

}

bool is_null(pkt_ptr pkt_queue){

    if (pkt_queue->front == -1 && pkt_queue->rear == -1){

        return true;

    }

    return false;

}

bool is_full(pkt_ptr pkt_Queue){

    if(pkt_Queue -> front == pkt_Queue -> rear + 1){

        return true;

    }

    else if(pkt_Queue -> front == 0 && pkt_Queue -> rear == MAX_PKT_LENGTH - 1){

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

        return (pkt_queue -> rear - pkt_queue -> front + 1);

    }

    else if (pkt_queue -> front > pkt_queue -> rear){

        return ((MAX_PKT_LENGTH - pkt_queue -> front) + pkt_queue -> rear + 1);

    }

    else{
        return queue_len_error;
    }
}
