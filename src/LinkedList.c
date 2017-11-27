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


/*
*  list_insert_:
*
*  This function changes the links between the original nodes and the added 
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
void list_insert_(List_Entry *new_node, List_Entry *prev, List_Entry *next) {

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
void list_insert_first(List_Entry *new_node, List_Entry *head) {

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

void list_insert_tail(List_Entry *new_node, List_Entry *head) {

    list_insert_(new_node, head->prev, head);

}

/*
*  remove_node__:
*
*  This function changes the links between the original nodes and the node  
*  which is going to be deleted.
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
void remove_node__(List_Entry *prev, List_Entry *next) {

    next->prev = prev;
    prev->next = next;

}


/*
*  remove_node_:
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
void remove_node_(List_Entry *removed_node_ptrs) {

    remove_node__(removed_node_ptrs->prev, removed_node_ptrs->next);


}


/*********************************************************************/


/*
*  insert_node_head:
*
*  This function creates a space for the struct of node is going to be 
*  added at the head of the list and returns the node.
*
*  Parameters:
*
*  entry - the head of the list for determining which list is goning to add
*  to.
*   
*  Return value:
*
*  node - a new node added to the list.
*/
struct Node *insert_node_head(List_Entry *entry) {
    struct Node *node;
    node = (struct Node*)malloc(sizeof(struct Node));
    list_insert_first(&node->ptrs, entry);
    
    return node;

}

/*
*  insert_node_tail:
*
*  This function creates a space for the struct of node is going to be 
*  added at the last of the list and returns the node.
*
*  Parameters:
*
*  entry - the head of the list for determining which list is goning to add
*  to.
*   
*  Return value:
*
*  node - a new node added to the list.
*/
struct Node *insert_node_tail(List_Entry *entry) {
    struct Node *node;
    node = (struct Node*)malloc(sizeof(struct Node));
    list_insert_tail(&node->ptrs, entry);

    return node;

}


/*
*  list_remove_node:
*
*  This function calls the function of remove_node_ to delete a node in the
*  list 
*  and release the memory.
*
*  Parameters:
*
*  removed_node - the struct of node for the specified node is going to be
*  removed.
*  
*  Return value:
*
*  None
*/
void list_remove_node(Node *removed_node) {

    remove_node_(&removed_node->ptrs);
    free(removed_node);

}

/*
*  list_remove_head:
*
*  This function calls the function of remove_node_ to delete a node at the
*  first of the list and release the memory.
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
void list_remove_head(List_Entry *entry) {
    printf("Removing \n");
    struct Node *node = ListEntry(entry->next, Node, ptrs);
    remove_node_(entry->next);
    free(node);
}


/*
*  remove_last:
*
*  This function calls the function of remove_node_ to delete a node at the  
*  last of the list and release the memory.
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
void remove_last(List_Entry *entry) {

    struct Node *node = ListEntry(entry->prev, Node, ptrs);
    remove_node_(entry->prev);
    free(node);

}


/*
 *  print_linked_list:
 *
 *  This function prints all the MAC addresses in the list. When printing, 
 *  the MAC addresses will be in the order of starting from the first node
 *  to the last in the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be
 *  printed.
 *
 *  Return value:
 *
 *  None
 */
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

/*
 *  get_head_content:
 *
 *  This function peeks at the head of the list. If the list is empty, it 
 *  returns NULL because it doesn't exist. Otherwise, this function returns
 *  the MAC address of the node at the head of the list.
 *
 *  Parameters:
 *
 *  entry - the head of the list for determining which list is goning to be 
 *  modified.
 *
 *  Return value:
 *
 *  return_value - MAC address of the fist node.
 */
char *get_head_entry(List_Entry *entry) {

    if (get_list_length(entry) == 0 ) {
        return NULL;
    }

    struct Node *node = ListEntry(entry->next, Node, ptrs);
    ScannedDevice *data;
    data = (struct ScannedDevice *) node->data;
    char *address = &data->scanned_mac_address[0];
    

    return address;
}


/*
 *  get_list_length:
 *
 *  This function gets the length of the list. Starting from the first node,
 *  we increment the length variable node by node.
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
int get_list_length(List_Entry * entry) {

    struct List_Entry *listptrs;
    int list_length = 0;

    for (listptrs = (entry)->next; listptrs != (entry); listptrs = listptrs->next) {
        list_length++;
    }

    return list_length;
}



