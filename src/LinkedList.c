/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

     GPL 3.0 : The content of this file is subject to the terms and conditions
     defined in file 'COPYING.txt', which is part of this source code package.

  Project Name:

     BeDIS

  File Description:

     This file contains the generic implementation of a double-linked-list
     data structure.It has functions for inserting a node to the front of the
     list and deleting a specific node. It can also check the length of the
     list. Any datatype of data could be stored in this list.


  File Name:

     LinkedList.c

  Version:

     2.0

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

     Han Wang, hollywang@iis.sinica.edu.tw
     Jake Lee, jakelee@iis.sinica.edu.tw
     Han Hu, hhu14@illinois.edu
     Jeffrey Lin, lin.jeff03@gmail.com
     Howard Hsu, haohsu0823@gmail.com
*/


#include "LinkedList.h"


inline void init_entry(List_Entry *entry){

    entry->next = entry;
    entry->prev = entry;

}


inline bool is_entry_list_empty(List_Entry *entry){

	return is_isolated_node(entry);
}


inline bool is_isolated_node(List_Entry *node){

    if(node == node->next){
        return true;
    }
    return false;
}


inline void insert_entry_list(List_Entry *new_node, List_Entry *prev,
                         List_Entry *next) {

    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;

}


inline void insert_list_first(List_Entry *new_node, List_Entry *head) {

    insert_entry_list(new_node, head, head->next);

}


inline void insert_list_tail(List_Entry *new_node, List_Entry *head) {

    insert_entry_list(new_node, head->prev, head);
}


inline void remove_entry_list(List_Entry *prev, List_Entry *next) {

    next->prev = prev;
    prev->next = next;

}


inline void remove_list_node(List_Entry *removed_node_ptrs) {

    remove_entry_list(removed_node_ptrs->prev, removed_node_ptrs->next);

    removed_node_ptrs->prev = removed_node_ptrs;
    removed_node_ptrs->next = removed_node_ptrs;

}


inline int get_list_length(List_Entry *entry) {

    struct List_Entry *listptrs;
    int list_length = 0;

    for (listptrs = (entry)->next; listptrs != (entry);
        listptrs = listptrs->next) {
        list_length++;
    }

    return list_length;
}
