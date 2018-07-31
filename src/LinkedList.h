/*
  Copyright (c) 2016 Academia Sinica, Institute of Information Science

  License:

      GPL 3.0 : The content of this file is subject to the terms and
      conditions defined in file 'COPYING.txt', which is part of this source
      code package.

  Project Name:

      BeDIPS

  File Description:

      This header file contains the function declarations and
      variables used in the LinkedList.c file.

  File Name:

      LinkedList.h

  Version:
 
      1.2

  Abstract:

      BeDIPS uses LBeacons to deliver 3D coordinates and textual
      descriptions of their locations to users' devices. Basically, a LBeacon
      is an inexpensive, Bluetooth Smart Ready device. The 3D coordinates and
      location description of every LBeacon are retrieved from BeDIS
      (Building/environment Data and Information System) and stored locally
      during deployment and maintenance times. Once initialized, each LBeacon
      broadcasts its coordinates and location description to Bluetooth
      enabled user devices within its coverage area.

  Authors:

      Han Wang, hollywang@iis.sinica.edu.tw
      Joey, joeyzhou@iis.sinica.edu.tw
     
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/* CONSTANTS */

/*Macro for calculating the offset of two addresses*/
#define offsetof(type, member) ((size_t) &((type *)0)->member)



/*Macro for geting the master struct from the sub struct */
#define ListEntry(ptr,type,member)  \
      ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*Macro for the method going through the list structure */
#define list_for_each(pos, head) \
      for (pos = (head)->next; pos != (head); pos = pos->next)

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)



/*Struct for the head of a list or doubly linked list entry used in link a
  node in to a list */
typedef struct List_Entry {   
   struct List_Entry *next;
   struct List_Entry *prev;
   
}List_Entry;



/* FUNCTIONS */

/*
  init_list:

  This function initializes the list.

  Parameters:
 
  entry - the head of the list for determining which list is goning to be 
          initialized.
   
  Return value:
 
  None
*/
 void init_entry(List_Entry *entry);


/*
  insert_entry_list:
 
  This function inserts a node at where specified by the previous and next
  pointers.

  Parameters:

  new_node - the struct of list entry for the node be added into the list.
  prev - the list entry pointing to the previous node of the new node.
  next - the list entry pointing to the next node of the new node.
    
  Return value:
 
  None
*/
  void insert_entry_list(List_Entry *new_node,
                         List_Entry *prev,
                         List_Entry *next);

/*
  insert_list_first:

  This function calls inserts a new node at the head of a specified list.

  Parameters:

  new_node - a pointer to the new node to be inserted into the list.
  head - The head of list.
    
  Return value:
 
  None
*/
 void insert_list_first(List_Entry *new_node, List_Entry *head);

/*
  insert_list_tail:
 
  This function inserts a new node at the tail of the specified list.

  Parameters:

  new_node - the list entry of the node be inserted into the list.
  head - The head of list.
   
  Return value:

  None
*/
 void insert_list_tail(List_Entry *new_node, List_Entry *head);

/*
  remove_entry_list:
 
  This function changes the links between the node and the node which 
  is going to be removed.

  Parameters:

  prev - the struct of list entry for the node which is going to be removed
         points to previously.
  next - the struct of list entry for the node which is going to be removed 
         points to next.
   
  Return value:

  None
*/
 void remove_entry_list(List_Entry *prev, List_Entry *next);


/*
  remove_list_node:

  This function calls the function of remove_node__ to delete a node in the 
  list.

  Parameters:

  removed_node_ptrs - the struct of list entry for the node is going to be
  removed.
  
   
  Return value:

  None
*/
 void remove_list_node(List_Entry *removed_node_ptrs);

/*
  get_list_length:

  This function returns the length of the list. 
 
  Parameters:
 
  entry - the head of the list for determining which list is goning to be 
  modified.
 
  Return value:
 
  length - number of nodes in the list.
 */
 int get_list_length(List_Entry *entry);


