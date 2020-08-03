/*
  Copyright (c) BiDaE Technology Inc. All rights reserved.

  License:

    BiDaE SHAREWARE LICENSE
    Version 1.0, 31 July 2020

    Copyright (c) BiDaE Technology Inc. All rights reserved.
    The SOFTWARE implemented in the product is copyrighted and protected by 
    all applicable intellectual property laws worldwide. Any duplication of 
    the SOFTWARE other than for archiving or resetting purposes on the same 
    product without the written agreement from BiDaE Technology could be a 
    violation of law. For the avoidance of doubt, redistribution of this 
    SOFTWARE in source or binary form is prohibited. You agree to prevent 
    any unauthorized copying and redistribution of the SOFTWARE. 

    BiDaE Technology Inc. is the license steward. No one other than the 
    license steward has the right to modify or publish new versions of this 
    License. However, You may distribute the SOFTWARE under the terms of the 
    version of the License under which You originally received the Covered 
    Software, or under the terms of any subsequent version published by the 
    license steward.

    LIMITED WARRANTY:

    BiDaE Technology Inc. or its distributors, depending on which party sold 
    the SOFTWARE, warrants that the media on which the SOFTWARE is installed 
    will be free from defects in materials under normal and purposed use.

    BiDaE Technology Inc. or its distributor warrants, for your benefit alone, 
    that during the Warranty Period the SOFTWARE, shall operate substantially 
    in accordance with the functional specifications in the User's Manual. If, 
    during the Warranty Period, a defect in the SOFTWARE appears, You may 
    obtain a replacement of the SOFTWARE. Any replacement SOFTWARE will be 
    warranted for the remainder of the Warranty Period attached to the product.

    WHEN THE WARRANTY PERIOD HAS BEEN EXPIRED, THIS SOFTWARE IS PROVIDED 
    ''AS IS,'' AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
    PARTICULAR PURPOSE ARE DISCLAIMED. HENCEFORTH, IN NO EVENT SHALL BiDaE 
    TECHNOLOGY INC. OR ITS COLLABORATOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
    NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY 
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
    THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

  Project Name:

     BeDIS

  File Name:

     LinkedList.h

  File Description:

     This header file contains the function declarations and variables used in
     the LinkedList.c file.

  Version:

     2.0, 20190826

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

     Han Wang      , hollywang@iis.sinica.edu.tw
     Joey Zhou     , joeyzhou@iis.sinica.edu.tw
     Gary Xiao     , garyh0205@hotmail.com
     Chun-Yu Lai   , chunyu1202@gmail.com
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#if defined(_MSC_VER)
#define inline __inline
#endif


/* CONSTANTS */

/*Macro for calculating the offset of two addresses*/
#define offsetof(type, member) ((size_t) &((type *)0)->member)

/*Macro for geting the master struct from the sub struct */
#define ListEntry(ptr,type,member)  \
      ((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))

/*Macro for the method going through the list structure */
#define list_for_each(pos, head) \
    for (pos = (head)->next; pos != (head); pos = pos->next)

/*Macro for the method going through the list structure reversely */
#define list_for_each_reverse(pos, head) \
    for (pos = (head)->prev; pos != (head); pos = pos->prev)

#define list_for_each_safe(pos, n, head) \
    for (pos = (head)->next, n = pos->next; pos != (head); \
        pos = n, n = pos->next)

#define list_for_each_safe_reverse(pos, n, head) \
    for (pos = (head)->prev, n = pos->prev; pos != (head); \
        pos = n, n = pos->prev)

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

     entry: the head of the list for determining which list is goning to be
            initialized.

  Return value:

     None
*/
inline void init_entry(List_Entry *entry) {

    entry->next = entry;
    entry->prev = entry;
}

/*
  insert_entry_list:

     This function inserts a node at where specified by the previous and next
     pointers.

  Parameters:

     new_node: the struct of list entry for the node be added into the list.
     prev: the list entry pointing to the previous node of the new node.
     next: the list entry pointing to the next node of the new node.

  Return value:

     None
*/
inline void insert_entry_list(List_Entry *new_node, List_Entry *prev,
                              List_Entry *next) {

    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

/*
  is_isolated_node:

     This function checks if the input node is isolated.

  Parameters:

     node - the node to be checked

  Return value:

     bool - Return true if the node is isolated. Otherwise, false if returned.
*/
inline bool is_isolated_node(List_Entry *node) {

    return (node == node->next);
}

/*
  is_entry_list_empty:

     This function checks if the input List_Entry is empty.

  Parameters:

     entry - the struct of list entry to be checked

  Return value:

     bool - Return true if the list is empty. Otherwise, false if returned.
*/
inline bool is_entry_list_empty(List_Entry *entry) {

    return is_isolated_node(entry);
}

/*
  insert_list_first:

     This function calls inserts a new node at the head of a specified list.

  Parameters:

     new_node: a pointer to the new node to be inserted into the list.
     head: The head of list.

  Return value:

     None
*/
inline void insert_list_first(List_Entry *new_node, List_Entry *head) {

    insert_entry_list(new_node, head, head->next);
}


/*
  insert_list_tail:

     This function inserts a new node at the tail of the specified list.

  Parameters:

     new_node: the list entry of the node be inserted into the list.
     head: The head of list.

  Return value:

     None
*/
inline void insert_list_tail(List_Entry *new_node, List_Entry *head) {

    insert_entry_list(new_node, head->prev, head);
}

/*
  remove_entry_list:

     This function changes the links between the node and the node which
     is going to be removed.

  Parameters:

     prev: the struct of list entry for the node which is going to be removed
           points to previously.
     next: the struct of list entry for the node which is going to be removed
           points to next.

  Return value:

     None
*/
inline void remove_entry_list(List_Entry *prev, List_Entry *next) {

    next->prev = prev;
    prev->next = next;
}

/*
  remove_list_node:

     This function calls the function of removed_node_ptrs to delete a node in
     the list.

  Parameters:

     removed_node_ptrs - the struct of list entry for the node is going to be
     removed.


  Return value:

     None
*/
inline void remove_list_node(List_Entry *removed_node_ptrs) {

    remove_entry_list(removed_node_ptrs->prev, removed_node_ptrs->next);

    removed_node_ptrs->prev = removed_node_ptrs;
    removed_node_ptrs->next = removed_node_ptrs;
}


/*
  concat_list:

     This function concates two lists.

  Parameters:

     first_list_head: The head of list.
     second_list_head: The head of list to be append to the first_list_head.

  Return value:

     None
*/
void concat_list(List_Entry *first_list_head, List_Entry *second_list_head);

/*
  get_list_length:

     This function returns the length of the list.

  Parameters:

     entry: the head of the list for determining which list is goning to be
            modified.

  Return value:

     length: number of nodes in the list.
 */
int get_list_length(List_Entry *entry);


#endif
