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

    pkt_queue -> locker = false;

    bool status;

    do{

        status = pkt_queue -> locker;

        pkt_queue -> locker = true;

    } while(status == true);

    init_log_file(&pkt_queue -> xbee_log, ".", "xbee_log.txt");

    pkt_queue -> front = -1;

    pkt_queue -> rear  = -1;

    pkt_queue -> locker = false;

    add_log(&pkt_queue -> xbee_log, collect_info, "init_Packet_Queue Success.", false);

}

void Free_Packet_Queue(pkt_ptr pkt_queue){

    while (!(is_null(pkt_queue))){

        delpkt(pkt_queue);

    }

    add_log(&pkt_queue -> xbee_log, collect_info, "Free_Packet_Queue Success.", false);

}

void addpkt(pkt_ptr pkt_queue, int type, char *raw_addr, char *content ) {

    bool status;

    do{

        status = pkt_queue -> locker;

        pkt_queue -> locker = true;

    } while(status == true);

    add_log(&pkt_queue -> xbee_log, collect_info, "==================", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "Addpkt Start.", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "==================", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "== type ==", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, type_to_str(type), false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "== address ==", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, raw_addr, false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "== content ==", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, content, false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "==================", false);

    if(is_null(pkt_queue)){

        add_log(&pkt_queue -> xbee_log, collect_info, "pkt_Queue is NULL.", false);

        pkt_queue -> front = 0;

        pkt_queue -> rear  = 0;

    }

    else{

        if(is_full(pkt_queue)){

            add_log(&pkt_queue -> xbee_log, collect_info, "pkt_Queue is FULL.", false);

            pkt_queue->locker = false;

            return;

        }

        else{

            if( pkt_queue -> rear == MAX_PKT_LENGTH - 1){

                add_log(&pkt_queue -> xbee_log, collect_info, "pkt_Queue rear pointer return to the first of of the packet Queue.", false);

                pkt_queue -> rear = 0;

            }

            else{

                add_log(&pkt_queue -> xbee_log, collect_info, "pkt_Queue rear pointer point to next location.", false);

                pkt_queue -> rear ++ ;

            }
        }

        add_log(&pkt_queue -> xbee_log, collect_info, "addpkt Success.", false);

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
    add_log(&pkt_queue -> xbee_log, collect_info, "= pkt_queue len  =", false);
    add_log(&pkt_queue -> xbee_log, collect_info, len, false);
    add_log(&pkt_queue -> xbee_log, collect_info, "==================", false);

    pkt_queue->locker = false;

    return;
}

 void delpkt(pkt_ptr pkt_queue) {

    bool status;

    do{

        status = pkt_queue -> locker;

        pkt_queue -> locker = true;

    } while(status == true);

    if(is_null(pkt_queue)) {

        add_log(&pkt_queue -> xbee_log, collect_info, "Packet Queue is empty!", false);

        pkt_queue -> locker = false;

        return;

    }

    display_pkt("deledpkt", pkt_queue, pkt_queue -> front);

    free(pkt_queue -> Queue[pkt_queue -> front].content);

    if(pkt_queue -> front == pkt_queue -> rear){

        add_log(&pkt_queue -> xbee_log, collect_info, "Packet Queue Reset to NULL", false);

        pkt_queue -> front = -1;

        pkt_queue -> rear  = -1;

    }
    else{

        if(pkt_queue -> front == MAX_PKT_LENGTH - 1){

            add_log(&pkt_queue -> xbee_log, collect_info, "pkt_Queue front pointer return to the first of of the packet Queue.", false);

            pkt_queue -> front = 0;

        }

        else{

            pkt_queue -> front += 1;

            add_log(&pkt_queue -> xbee_log, collect_info, "pkt_Queue front pointer point to next location.", false);

        }

    }

    add_log(&pkt_queue -> xbee_log, collect_info, "delpkt Success.", false);

    char len[10];

    memset(len, 0, 10);

    sprintf(len, "%d\0", queue_len(pkt_queue));

    add_log(&pkt_queue -> xbee_log, collect_info, "= pkt_queue len  =", false);
    add_log(&pkt_queue -> xbee_log, collect_info, len, false);
    add_log(&pkt_queue -> xbee_log, collect_info, "==================", false);

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

int str_to_type(const char* conType){

    if(memcmp(conType, "Transmit Status"
            , strlen("Transmit Status")* sizeof(char)) == 0){

        return Data;

    }

    else if(memcmp(conType, "Data"
            , strlen("Data")* sizeof(char)) == 0){

        return Data;

    }

    else{

        return -1;

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

}

void display_pkt(char* content, pkt_ptr pkt_queue, int pkt_num){

    if(pkt_num == -1)

        return;

    char* char_addr = print_address(pkt_queue -> Queue[pkt_num].address);

    add_log(&pkt_queue -> xbee_log, collect_info, "==================", false);
    add_log(&pkt_queue -> xbee_log, collect_info, content, false);
    add_log(&pkt_queue -> xbee_log, collect_info, "==================", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "== type ==", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, type_to_str(pkt_queue -> Queue[pkt_num].type), false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "== address ==", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, char_addr, false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "== content ==", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, pkt_queue -> Queue[pkt_num].content, false);
    add_log(&pkt_queue -> xbee_log, collect_info, "", false);
    add_log(&pkt_queue -> xbee_log, collect_info, "==================", false);

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

        add_log(&pkt_Queue -> xbee_log, collect_info, "is_full return true.", false);

        return true;

    }

    else if(pkt_Queue -> front == 0 && pkt_Queue -> rear == MAX_PKT_LENGTH - 1){

        add_log(&pkt_Queue -> xbee_log, collect_info, "is_full return true.", false);

        return true;

    }

    else{

        add_log(&pkt_Queue -> xbee_log, collect_info, "is_full return false.", false);

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

    else{

        return pkt_queue->rear - pkt_queue->front + 1;

    }
}
