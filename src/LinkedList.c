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
*      This file contains the generic implementation of a double-linked-list 
*      data structure.It has functions for inserting a node to the front of the 
*      list and deleting a specific node. It can also check the length of the
*      list. Any datatype of data could be stored in this list. 
*     
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
*      Han Wang, hollywang@iis.sinica.edu.tw 
*      Jake Lee, jakelee@iis.sinica.edu.tw
*      Johnson Su, johnsonsu@iis.sinica.edu.tw
*      Shirley Huang, shirley.huang.93@gmail.com
*      Han Hu, hhu14@illinois.edu
*      Jeffrey Lin, lin.jeff03@gmail.com
*      Howard Hsu, haohsu0823@gmail.com
*      
*/

#include "LinkedList.h"



inline void init_list(List_Entry *entry){
        
        entry->next = entry;
        entry->prev = entry;

}


inline void list_insert_(List_Entry *new_node, List_Entry *prev, 
                         List_Entry *next) {

    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;

}


inline void list_insert_first(List_Entry *new_node, List_Entry *head) {

    list_insert_(new_node, head, head->next);

}




inline void list_insert_tail(List_Entry *new_node, List_Entry *head) {

    list_insert_(new_node, head->prev, head);

}


inline void list_remove_(List_Entry *prev, List_Entry *next) {

    next->prev = prev;
    prev->next = next;

}


inline void list_remove_node(List_Entry *removed_node_ptrs) {

    list_remove_(removed_node_ptrs->prev, removed_node_ptrs->next);


}


inline int get_list_length(List_Entry * entry) {

    struct List_Entry *listptrs;
    int list_length = 0;

    for (listptrs = (entry)->next; listptrs != (entry); 
         listptrs = listptrs->next) {
        list_length++;
    }

    return list_length;
}




inline void *get_list_head(List_Entry *entry) {

    /*Check whether the list is empty */
    if (get_list_length(entry) == 0 ) {
        return NULL;
    }

    struct Node *node = ListEntry(entry->next, Node, ptrs);
    void *data;
    data = node->data;
    
    return data;
}




inline void *get_list_tail(List_Entry *entry) {

    /*Check whether the list is empty */
    if (get_list_length(entry) == 0 ) {
        return NULL;
    }

    struct Node *node = ListEntry(entry->prev, Node, ptrs);
    void *data;
    data = node->data;
    
    return data;
}






inline void print_list(List_Entry *entry, void (*fptr)(void *)){

    /*Check whether the list is empty */
    if (get_list_length(entry) == 0 ) {
        return;
    }

    struct List_Entry *listptrs;
    struct Node *node;

    list_for_each(listptrs, entry){

        node = ListEntry(listptrs, Node, ptrs);
        (*fptr)(node->data);

    }

    printf("\n");

}


inline void free_list(List_Entry *entry){

    /*Check whether the list is empty */
    if (get_list_length(entry) == 0 ) {
        return;
    }

    struct List_Entry *listptrs;
    Node *temp;

    list_for_each(listptrs, entry){
        
        temp = ListEntry(listptrs, Node, ptrs);
        free(temp);
        temp = NULL;

    }

    free(entry);

}


