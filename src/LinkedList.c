/*
* Copyright (c) 2016 Academia Sinica, Institute of Information Science
*
* License:
*
*      GPL 3.0 : The content of this file is subject to the terms and
*      conditions defined in file 'COPYING.txt', which is part of this source
*      code package.
*
* Project Name:
*
*      BeDIPS
*
* File Description:
*
*      This file contains the implementation of a linked list data structure.
*      It has functions for inserting a node to the front of the linked list
*      and deleting a specific node. It can also check the length of the
*      linked list and print the information stored on all nodes in the list.
*      The purpose of this linked list implementation is to allow scanned MAC
*      addresses and its timestamp to be stored as a way of keeping track
*      which devices have just been scanned.
*
* File Name:
*
*      LinkedList.c
*
* Abstract:
*
*      BeDIPS uses LBeacons to deliver 3D coordinates and textual
*      descriptions of their locations to users' devices. Basically, a LBeacon
*      is an inexpensive, Bluetooth Smart Ready device. The 3D coordinates and
*      location description of every LBeacon are retrieved from BeDIS
*      (Building/environment Data and Information System) and stored locally
*      during deployment and maintenance times. Once initialized, each LBeacon
*      broadcasts its coordinates and location description to Bluetooth
*      enabled user devices within its coverage area.
*
* Authors:
*
*      Jake Lee, jakelee@iis.sinica.edu.tw
*      Johnson Su, johnsonsu@iis.sinica.edu.tw
*      Shirley Huang, shirley.huang.93@gmail.com
*      Han Hu, hhu14@illinois.edu
*      Jeffrey Lin, lin.jeff03@gmail.com
*      Howard Hsu, haohsu0823@gmail.com
*      Han Wang, hollywang@iis.sinica.edu.tw 
*/

#include "LinkedList.h"




void __list_add(List_Entry *new_node, List_Entry *prev, List_Entry *next) {

    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;

}

void list_add_first(List_Entry *new_node, List_Entry *head) {

    __list_add(new_node, head, head->next);

}

void list_add_tail(List_Entry *new_node, List_Entry *head) {

    __list_add(new_node, head->prev, head);

}


void __delete_node(List_Entry *prev, List_Entry *next) {

    next->prev = prev;
    prev->next = next;

}

void delete_node(List_Entry *removed_node_ptrs) {

    __delete_node(removed_node_ptrs->prev, removed_node_ptrs->next);


}


/*********************************************************************/

struct Node *add_node_first(List_Entry *entry) {
    struct Node *node;
    node = (struct Node*)malloc(sizeof(struct Node));
    list_add_first(&node->ptrs, entry);
    
    return node;

}


struct Node *add_node_tail(List_Entry *entry) {
    struct Node *node;
    node = (struct Node*)malloc(sizeof(struct Node));
    list_add_tail(&node->ptrs, entry);

    return node;

}


void remove_node(Node *removed_node) {

    delete_node(&removed_node->ptrs);
    free(removed_node);

}

void remove_first(List_Entry *entry) {
    printf("Removing \n");
    struct Node *node = ListEntry(entry->next, Node, ptrs);
    delete_node(entry->next);
    free(node);
}

void remove_last(List_Entry *entry) {

    struct Node *node = ListEntry(entry->prev, Node, ptrs);
    delete_node(entry->prev);
    free(node);

}

void print_list(List_Entry *entry) {

    
    if (get_list_length(entry) == 0 ) {
        return;
    }

    struct List_Entry *listptrs = NULL;
    struct Node *node;
    for (listptrs = (entry)->next; listptrs != (entry); listptrs = listptrs->next) {
        node = ListEntry(listptrs, Node, ptrs);
        ScannedDevice *data;
        data = (struct ScannedDevice *)node->data;
        printf("%s ", &data->scanned_mac_address[0]);
    }
    printf("\n");

}

char *get_first_content(List_Entry *entry) {

    if (get_list_length(entry) == 0 ) {
        return NULL;
    }

    struct Node *node = ListEntry(entry->next, Node, ptrs);
    ScannedDevice *data;
    data = (struct ScannedDevice *) node->data;
    char *address = &data->scanned_mac_address[0];
    

    return address;
}



int get_list_length(List_Entry * entry) {

    struct List_Entry *listptrs;
    int list_length = 0;

    for (listptrs = (entry)->next; listptrs != (entry); listptrs = listptrs->next) {
        list_length++;
    }

    return list_length;
}



