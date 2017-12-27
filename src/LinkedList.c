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
*      Jake Lee, jakelee@iis.sinica.edu.tw
*      Johnson Su, johnsonsu@iis.sinica.edu.tw
*      Shirley Huang, shirley.huang.93@gmail.com
*      Han Hu, hhu14@illinois.edu
*      Jeffrey Lin, lin.jeff03@gmail.com
*      Howard Hsu, haohsu0823@gmail.com
*      Han Wang, hollywang@iis.sinica.edu.tw 
*/

#include "LinkedList.h"



/*
*  list_insert_:
*
*  This function changes the links between node and the added 
*  new node.
*
*  Parameters:
*
*  new_node - the struct of list entry for the node be added into the list.
*  prev - the struct of list entry which the new node points to previously.
*  next - the struct of list entry which the new node points to next.
*   
*  Return value:
*
*  None
*/
inline void list_insert_(List_Entry *new_node, List_Entry *prev, 
                         List_Entry *next) {

    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;

}


/*
*  list_insert_first:
*
*  This function calls the function of list_insert_ to add a new node at the 
*  first of the list.
*
*  Parameters:
*
*  new_node - the struct of list entry for the node be added into the list.
*  head - the struct of list entry which is the head of the list.
*   
*  Return value:
*
*  None
*/
inline void list_insert_first(List_Entry *new_node, List_Entry *head) {

    list_insert_(new_node, head, head->next);

}


/*
*  list_insert_tail:
*
*  This function calls the function of list_insert_ to add a new node at the 
*  last of the list.
*
*  Parameters:
*
*  new_node - the struct of list entry for the node be added into the list.
*  head - the struct of list entry which is the head of the list.
*   
*  Return value:
*
*  None
*/

inline void list_insert_tail(List_Entry *new_node, List_Entry *head) {

    list_insert_(new_node, head->prev, head);

}

/*
*  list_remove_:
*
*  This function changes the links between the node and the node which 
*  is going to be removed.
*
*  Parameters:
*
*  prev - the struct of list entry for the node which is going to be deleted 
*  points to previously.
*  next - the struct of list entry for the node which is going to be deleted 
*  points to next.
*   
*  Return value:
*
*  None
*/
inline void list_remove_(List_Entry *prev, List_Entry *next) {

    next->prev = prev;
    prev->next = next;

}


/*
*  list_remove_node:
*
*  This function calls the function of remove_node__ to delete a node in the 
*  list.
*
*  Parameters:
*
*  removed_node_ptrs - the struct of list entry for the node is going to be
*  removed.
*  
*   
*  Return value:
*
*  None
*/
inline void list_remove_node(List_Entry *removed_node_ptrs) {

    list_remove_(removed_node_ptrs->prev, removed_node_ptrs->next);


}


/*
 *  get_list_length:
 *
 *  This function returns the length of the list. 
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  length - number of nodes in the list.
 */
inline int get_list_length(List_Entry * entry) {

    struct List_Entry *listptrs;
    int list_length = 0;

    for (listptrs = (entry)->next; listptrs != (entry); 
         listptrs = listptrs->next) {
        list_length++;
    }

    return list_length;
}



/*
 *  get_list_head:
 *
 *  This function peeks at the head of the list. If the list is empty, it 
 *  returns NULL because it doesn't exist. Otherwise, this function returns
 *  the data of the node at the head of the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  return_value - data of the fist node.
 */
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



/*
 *  get_list_tail:
 *
 *  This function peeks at the tail of the list. If the list is empty, it 
 *  returns NULL because it doesn't exist. Otherwise, this function returns
 *  the data of the node at the tail of the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  return_value - data of the last node.
 */
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


/*
 *  free_list:
 *
 *  This function frees the resources of the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  None
 */
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


